//
// Created by zhijun on 2021/5/24.
// Edited by yiheng on 2023/2/18.
//

#include "system.h"
#include "SurfelMapping.h"
#include <iomanip>
#include <sys/stat.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

SurfelMapping::SurfelMapping()
: tick(0),
  currPose(Eigen::Matrix4f::Identity()),
  lastPose(Eigen::Matrix4f::Identity()),
  refFrameIsSet(false),
  beginCleanPoints(false),
  nearClipDepth(Config::nearClip()),
  farClipDepth(Config::farClip()),
  depthPad(Config::depthPad()),
  surfelFuseDistanceThreshFactor(Config::surfelFuseDistanceThreshFactor()),
  checker(new Checker)
{
    createTextures();
    createCompute();
    createFeedbackBuffers();
}

SurfelMapping::~SurfelMapping()
{
    for(auto it = textures.begin(); it != textures.end(); ++it)
    {
        delete it->second;
    }

    textures.clear();

    for(auto it = computePacks.begin(); it != computePacks.end(); ++it)
    {
        delete it->second;
    }

    computePacks.clear();

    for(auto it = feedbackBuffers.begin(); it != feedbackBuffers.end(); ++it)
    {
        delete it->second;
    }

    feedbackBuffers.clear();
}

void SurfelMapping::createTextures()
{
    int w = Config::W();
    int h = Config::H();

    textures[GPUTexture::RGB] = new GPUTexture(w, h,
                                               GL_RGB32F,
                                               GL_RGB,
                                               GL_UNSIGNED_BYTE,
                                               true);

    textures[GPUTexture::DEPTH_RAW] = new GPUTexture(w, h,
                                                     GL_R16UI,
                                                     GL_RED_INTEGER,
                                                     GL_UNSIGNED_SHORT);

    textures[GPUTexture::DEPTH_FILTERED] = new GPUTexture(w, h,
                                                          GL_R32F,
                                                          GL_RED,
                                                          GL_FLOAT);

    textures[GPUTexture::DEPTH_METRIC] = new GPUTexture(w, h,
                                                        GL_R32F,
                                                        GL_RED,
                                                        GL_FLOAT);

    textures[GPUTexture::SEMANTIC] = new GPUTexture(w, h,
                                                    GL_R8UI,
                                                    GL_RED_INTEGER,
                                                    GL_UNSIGNED_BYTE);

    textures["LAST"] = new GPUTexture(w, h,
                                      GL_R32F,
                                      GL_RED,
                                      GL_FLOAT);

}

void SurfelMapping::createCompute()
{
    computePacks[ComputePack::FILTER] = new ComputePack(loadProgramFromFile("empty.vert",
                                                                                  "quad.geom",
                                                                                  "depth_filter.frag"));

    computePacks[ComputePack::METRIC] = new ComputePack(loadProgramFromFile("empty.vert",
                                                                                  "quad.geom",
                                                                                  "depth_metric.frag"));

    computePacks[ComputePack::SMOOTH] = new ComputePack(loadProgramFromFile("empty.vert",
                                                                                    "quad.geom",
                                                                                    "depth_smooth.frag"));

    computePacks["MOVINGS"] = new ComputePack(loadProgramFromFile("empty.vert",
                                                                          "quad.geom",
                                                                          "depth_movings.frag"));
}

void SurfelMapping::createFeedbackBuffers()
{
    feedbackBuffers[FeedbackBuffer::RAW] = new FeedbackBuffer(loadProgramGeomFromFile("surfel_feedback.vert",
                                                                                            "surfel_feedback.geom"));

}

void SurfelMapping::processFrame(const unsigned char *rgb,
                                 const unsigned short *depth,
                                 const unsigned char * semantic,
                                 const Eigen::Matrix4f *gtPose)
{
    TICK("Run");

    textures[GPUTexture::RGB]->texture->Upload(rgb, GL_RGB, GL_UNSIGNED_BYTE);

    if(depth)
        textures[GPUTexture::DEPTH_RAW]->texture->Upload(depth, GL_RED_INTEGER, GL_UNSIGNED_SHORT);

    if(semantic)
        textures[GPUTexture::SEMANTIC]->texture->Upload(semantic, GL_RED_INTEGER, GL_UNSIGNED_BYTE);

    currPose = *gtPose;


    TICK("Preprocess");

    // convert to metric unit
    metriciseDepth();

    // optimize the edges & filter the unwanted semantic classes
    filterDepth();

    // filter the moving objects
    if(!refFrameIsSet)
    {
        // copy to LAST
        texcpy(textures["LAST"]->texture, textures[GPUTexture::DEPTH_FILTERED]->texture);

        lastPose = currPose;
        refFrameIsSet = true;

        historyPoses.push_back(currPose);
        ++tick;

        return;
    }

    removeMovings();

    TOCK("Preprocess");

    //First run
    if(tick == 0)
    {
        // compute surfel in current frame
        computeFeedbackBuffers();

        globalModel.initialize(*feedbackBuffers[FeedbackBuffer::RAW], currPose);

        globalModel.buildModelMap();  // build model map each time modelVbo is updated
    }
    else
    {
        computeFeedbackBuffers();  // todo: move to outside

//        unsigned int lastCount = globalModel.getModel().second;
//        std::cout << "Last Model Num: " << lastCount << '\n';

        TICK("Conflict");
        globalModel.processConflict(currPose,
                                    tick,
                                    textures[GPUTexture::DEPTH_METRIC],
                                    textures[GPUTexture::SEMANTIC],
                                    nearClipDepth,
                                    farClipDepth);

//        std::cout << "Conflict Num: " << globalModel.getConflict().second << '\n';

        globalModel.updateConflict();

        globalModel.backMapping();

//        std::cout << "Model Num after conflict: " << globalModel.getModel().second << " so removed: " << lastCount - globalModel.getModel().second << '\n';

        globalModel.buildModelMap();
        TOCK("Conflict");

        TICK("indexMap");
        indexMap.predictIndices(currPose, tick, globalModel.getModel(), farClipDepth, 200);
        TOCK("indexMap");

        globalModel.dataAssociate(currPose,
                                  tick,
                                  textures[GPUTexture::RGB],
                                  textures[GPUTexture::DEPTH_METRIC],
                                  textures[GPUTexture::SEMANTIC],
                                  indexMap.indexTex(),
                                  indexMap.vertConfTex(),
                                  indexMap.colorTimeTex(),
                                  indexMap.normalRadTex(),
                                  nearClipDepth,
                                  farClipDepth);

//        std::cout << "Data Association Num: " << globalModel.getData().second << '\n';

        globalModel.updateFuse();

        globalModel.backMapping();

//        std::cout << "Model Num after Update Fuse: " << globalModel.getModel().second << '\n';

        globalModel.concatenate();

//        std::cout << "New Model Num: " << globalModel.getUnstable().second << '\n'
//                  << "Total Model: " << globalModel.getModel().second << std::endl;

        globalModel.buildModelMap();  // build model map each time modelVbo is updated

        CheckGlDieOnError()
    }

    texcpy(textures["LAST"]->texture, textures[GPUTexture::DEPTH_FILTERED]->texture);
    lastPose = currPose;

    historyPoses.push_back(currPose);
    ++tick;

    TOCK("Run");
}

void SurfelMapping::metriciseDepth()
{
    std::vector<Uniform> uniforms;

    uniforms.emplace_back("minD", nearClipDepth);
    uniforms.emplace_back("maxD", farClipDepth);
    uniforms.emplace_back("cols", (float)Config::W());
    uniforms.emplace_back("stereoBorder", depthPad);
    uniforms.emplace_back("r0", (float)Config::rzero());
    //std::cout << depthPad << std::endl;

    computePacks[ComputePack::METRIC]->compute(textures[GPUTexture::DEPTH_METRIC]->texture,
                                               textures[GPUTexture::DEPTH_RAW]->texture,
                                               &uniforms);
}

void SurfelMapping::filterDepth()
{
    // filter unwanted classes and bad edges
    std::vector<Uniform> uniforms;
    std::vector<pangolin::GlTexture *> inputs;

    inputs.push_back(textures[GPUTexture::DEPTH_METRIC]->texture);
    inputs.push_back(textures[GPUTexture::SEMANTIC]->texture);

    uniforms.emplace_back("dSampler", 0);
    uniforms.emplace_back("sSampler", 1);
    uniforms.emplace_back("cols", (float)Config::W() );
    uniforms.emplace_back("rows", (float)Config::H() );
    uniforms.emplace_back("minD", nearClipDepth);
    uniforms.emplace_back("maxD", farClipDepth);
    uniforms.emplace_back("diffThresh", surfelFuseDistanceThreshFactor);  // threshold of support pixel
    uniforms.emplace_back("r0", (float)Config::rzero());

    computePacks[ComputePack::FILTER]->compute(textures[GPUTexture::DEPTH_FILTERED]->texture,
                                               inputs,
                                               &uniforms);

    // smooth depth
    float sigma_pixel = 4.5;
    float sigma_intensity = 30.;
    float sigma_pixel2_inv_half = 0.5f / (sigma_pixel * sigma_pixel);                 // 1 / 2*sigma^2
    float sigma_intensity2_inv_half = 0.5f / (sigma_intensity * sigma_intensity);

    inputs.clear();
    uniforms.clear();

    inputs.push_back(textures[GPUTexture::DEPTH_FILTERED]->texture);
    inputs.push_back(textures[GPUTexture::SEMANTIC]->texture);

    uniforms.emplace_back("dSampler", 0);
    uniforms.emplace_back("sSampler", 1);
    uniforms.emplace_back("cols", (float)Config::W() );
    uniforms.emplace_back("rows", (float)Config::H() );
    uniforms.emplace_back("minD", nearClipDepth);
    uniforms.emplace_back("maxD", farClipDepth);
    uniforms.emplace_back("stereoBorder", depthPad);
    uniforms.emplace_back("sigPix", sigma_intensity2_inv_half);
    uniforms.emplace_back("r0", (float)Config::rzero());

    computePacks[ComputePack::SMOOTH]->compute(textures[GPUTexture::DEPTH_METRIC]->texture,
                                               inputs,
                                               &uniforms);

    // filter unwanted classes and bad edges
    uniforms.clear();
    inputs.clear();

    inputs.push_back(textures[GPUTexture::DEPTH_METRIC]->texture);
    inputs.push_back(textures[GPUTexture::SEMANTIC]->texture);

    uniforms.emplace_back("dSampler", 0);
    uniforms.emplace_back("sSampler", 1);
    uniforms.emplace_back("cols", (float)Config::W() );
    uniforms.emplace_back("rows", (float)Config::H() );
    uniforms.emplace_back("minD", nearClipDepth);
    uniforms.emplace_back("maxD", farClipDepth);
    uniforms.emplace_back("diffThresh", surfelFuseDistanceThreshFactor);  // threshold of support pixel  todo tune
    uniforms.emplace_back("r0", (float)Config::rzero());

    computePacks[ComputePack::FILTER]->compute(textures[GPUTexture::DEPTH_FILTERED]->texture,
                                               inputs,
                                               &uniforms);

}

void SurfelMapping::removeMovings()
{
    // project current cars .. to last and check depth
    std::vector<Uniform> uniforms;
    std::vector<pangolin::GlTexture *> inputs;

    inputs.push_back(textures[GPUTexture::DEPTH_FILTERED]->texture);
    inputs.push_back(textures[GPUTexture::SEMANTIC]->texture);
    inputs.push_back(textures["LAST"]->texture);

    Eigen::Vector4f cam(Config::cx(), Config::cy(), Config::fx(), Config::fy());
    Eigen::Matrix4f T_c2l = lastPose.inverse() * currPose;

    uniforms.emplace_back("dSampler", 0);
    uniforms.emplace_back("sSampler", 1);
    uniforms.emplace_back("lSampler", 2);
    uniforms.emplace_back("cols", (float)Config::W() );
    uniforms.emplace_back("rows", (float)Config::H() );
    uniforms.emplace_back("cam", cam);
    uniforms.emplace_back("minD", nearClipDepth);
    uniforms.emplace_back("maxD", farClipDepth);
    uniforms.emplace_back("t_c2l", T_c2l);
    uniforms.emplace_back("stereoBorder", depthPad);
    uniforms.emplace_back("moveThresh", 0.5f);
    uniforms.emplace_back("r0", (float)Config::rzero());

    computePacks["MOVINGS"]->compute(textures[GPUTexture::DEPTH_METRIC]->texture,
                                     inputs,
                                     &uniforms);

}

void SurfelMapping::computeFeedbackBuffers()
{
    TICK("feedbackBuffers");
    feedbackBuffers[FeedbackBuffer::RAW]->compute(textures[GPUTexture::RGB]->texture,
                                                  textures[GPUTexture::DEPTH_METRIC]->texture,
                                                  textures[GPUTexture::SEMANTIC]->texture,
                                                  tick,
                                                  farClipDepth);
    TOCK("feedbackBuffers");
}

void SurfelMapping::acquireImages(std::string path, const std::vector<Eigen::Matrix4f> &views,
                                  int w, int h, float fx, float fy, float cx, float cy, int startId)
{
    auto texturePtr = new unsigned char [w * h * 3];
    auto semanticPtr = new unsigned char [w * h];

    if(path.back() != '/')  path += "/";
    std::string image_path = path;
    //std::string image_path = path + "image/";
    //std::string semantic_path = path + "semantic/";
    _mkdir(image_path.c_str());
    //_mkdir(semantic_path.c_str());

    globalModel.setImageSize(w, h, fx, fy, cx, cy);

    for(const auto &v : views)
    {
        //std::cout << v << std::endl;

        std::stringstream ss;
        ss << std::setfill('0') << std::setw(10) << startId;
        std::string file_name = ss.str() + ".png";

        globalModel.renderImage(v);

        //---------------------------------------------

        globalModel.getImageTex()->Download(texturePtr, GL_RGB_INTEGER, GL_UNSIGNED_BYTE);

        CheckGlDieOnError()

        cv::Mat image(h, w, CV_8UC3);
        memcpy(image.data, texturePtr, w * h * 3);

        bool res = cv::imwrite(image_path + file_name, image);

        //---------------------------------------------

        //globalModel.getSemanticTex()->Download(semanticPtr, GL_RED_INTEGER, GL_UNSIGNED_BYTE);

        //CheckGlDieOnError()

        //cv::Mat semantic(h, w, CV_8UC1);
        //memcpy(semantic.data, semanticPtr, w * h);


        if(!res)
            printf("%s is NOT saved!\n", file_name.c_str());
        //usleep(1000);

        startId++;
    }

    delete [] texturePtr;
    //delete [] semanticPtr;
}

void SurfelMapping::reset()
{
    globalModel.resetBuffer();
    tick = 0;
    historyPoses.clear();
}

void SurfelMapping::texcpy(pangolin::GlTexture *target, pangolin::GlTexture *source)
{
    glCopyImageSubData(source->tid, GL_TEXTURE_2D, 0, 0, 0, 0,
                       target->tid, GL_TEXTURE_2D, 0, 0, 0, 0,
                       source->width, source->height, 1);
}

pangolin::GlTexture * SurfelMapping::getTexture(const std::string &textureType)
{
    auto iter = textures.find(textureType);
    assert(iter != textures.end() && "there is no such texture type");

    return textures[textureType]->texture;
}

FeedbackBuffer * SurfelMapping::getFeedbackBuffer(const std::string &feedbackType)
{
    auto iter = feedbackBuffers.find(feedbackType);
    assert(iter != feedbackBuffers.end() && "there is no such FeedbackBuffer type");

    return feedbackBuffers[feedbackType];
}

const Eigen::Matrix4f & SurfelMapping::getCurrPose()
{
    return currPose;
}

const std::vector<Eigen::Matrix4f> & SurfelMapping::getHistoryPoses()
{
    return historyPoses;
}

GlobalModel & SurfelMapping::getGlobalModel()
{
    return globalModel;
}

IndexMap & SurfelMapping::getIndexMap()
{
    return indexMap;
}

void SurfelMapping::setBeginCleanPoints()
{
    beginCleanPoints = true;
}

bool SurfelMapping::getBeginCleanPoints()
{
    return beginCleanPoints;
}

void SurfelMapping::cleanPoints(const unsigned short *depth, const unsigned char * semantic, const Eigen::Matrix4f *gtPose)
{
    textures[GPUTexture::DEPTH_RAW]->texture->Upload(depth, GL_RED_INTEGER, GL_UNSIGNED_SHORT);
    if (semantic)
        textures[GPUTexture::SEMANTIC]->texture->Upload(semantic, GL_RED_INTEGER, GL_UNSIGNED_BYTE);

    currPose = *gtPose;

    // convert to metric unit
    metriciseDepth();

    // optimize the edges & filter the unwanted semantic classes
    //filterDepth();

    TICK("Clean Points");
    globalModel.processConflict(*gtPose,
                                tick,
                                textures[GPUTexture::DEPTH_METRIC],
                                textures[GPUTexture::SEMANTIC],
                                nearClipDepth,
                                farClipDepth - 15,
                                surfelFuseDistanceThreshFactor,
                                1);

    //std::cout << "Conflict Num: " << globalModel.getConflict().second << '\n';

    globalModel.updateConflict();

    globalModel.backMapping();

    //std::cout << "Model Num after conflict: " << globalModel.getModel().second  << '\n';

    globalModel.buildModelMap();
    TOCK("Clean Points");

    beginCleanPoints = false;

}
