//
// Created by zhijun on 2021/8/23.
//

#include "KittiReader.h"
#include "SurfelMapping.h"
#include "GUI.h"
#include "Checker.h"

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <vector>
#include <random>
#include <ctime>

using namespace std;


int globalId = 0;
int lastRestartId = 0;

std::vector<Eigen::Matrix4f> modelPoses;
std::vector<Eigen::Matrix4f> novelViewS;
std::vector<Eigen::Matrix4f> novelViewLeft;
std::vector<Eigen::Matrix4f> novelViewRight;

bool S_shaped_novel = false;

// overview
int overviewId = 0;

bool autoGenerateImage = true;


void rungui(SurfelMapping & core, GUI & gui)
{
    if(gui.getMode() == GUI::ShowMode::minimum)
    {

        bool run_gui = true;
        while(run_gui)
        {
            Eigen::Matrix4f pose = modelPoses[overviewId];

            bool follow_pose = gui.followPose->Get();

            if(follow_pose)
            {
                pangolin::OpenGlMatrix mv;

                Eigen::Matrix3f currRot = pose.topLeftCorner(3, 3);

                Eigen::Vector3f forwardVector(0, 0, 1);
                Eigen::Vector3f upVector(0, -1, 0);

                Eigen::Vector3f forward = (currRot * forwardVector).normalized();
                Eigen::Vector3f up = (currRot * upVector).normalized();

                Eigen::Vector3f viewAt(pose(0, 3), pose(1, 3), pose(2, 3));

                Eigen::Vector3f eye = viewAt - forward;

                Eigen::Vector3f z = (eye - viewAt).normalized();  // Forward, OpenGL camera z direction
                Eigen::Vector3f x = up.cross(z).normalized();     // Right
                Eigen::Vector3f y = z.cross(x);                   // Up

                Eigen::Matrix4d m;                                // [R; U; F]_4x4 * [E; -eye]_4x4
                m << x(0),  x(1),  x(2),  -(x.dot(eye)),
                        y(0),  y(1),  y(2),  -(y.dot(eye)),
                        z(0),  z(1),  z(2),  -(z.dot(eye)),
                        0,     0,     0,      1;

                memcpy(&mv.m[0], m.data(), sizeof(Eigen::Matrix4d));

                gui.s_cam.SetModelViewMatrix(mv);
            }


            //float backColor[4] = {0.05, 0.05, 0.3, 0.0f};
            float backColor[4] = {0, 0, 0, 0};
            gui.preCall(backColor);

            //====== Enter Path Mode until Complete
            bool initView = true;
            int totalNovelViewNum = 0;
            if(gui.pathMode->Get() || autoGenerateImage)
            {

                //=== draw all history frame
                std::vector<Eigen::Vector3f> positionVerts, positionVertNovel;
                for(auto & p : modelPoses)
                {
                    gui.drawFrustum(p);
                    positionVerts.emplace_back(p.topRightCorner<3, 1>());
                }
                glColor3f(1.0f,1.0f,0.0f);
                pangolin::glDrawVertices(positionVerts, GL_LINE_STRIP);
                glColor3f(1.0f,1.0f,1.0f);

                //=== draw all novel frame (if exist)
                float frameColor[3] = {1.f, 0.f, 0.0f};
                for(auto & p : novelViewLeft)
                {
                    gui.drawFrustum(p, frameColor);
                    positionVertNovel.emplace_back(p.topRightCorner<3, 1>());
                }
                for(auto & p : novelViewRight)
                {
                    gui.drawFrustum(p, frameColor);
                    positionVertNovel.emplace_back(p.topRightCorner<3, 1>());
                }

                if(S_shaped_novel)
                {

                    glColor3f(1.0f,0.0f,0.0f);
                    pangolin::glDrawVertices(positionVertNovel, GL_LINE_STRIP);
                    glColor3f(1.0f,1.0f,1.0f);
                }


                //=== If acquire images
                if(pangolin::Pushed(*gui.acquirePairedImage))
                {
                    std::string data_path = "./load_map_output/paired";  // todo

                    std::vector<Eigen::Matrix4f> views;
                    gui.getViews(views, modelPoses);  // todo

                    core.acquireImages(data_path, views,
                                       Config::W(), Config::H(),
                                       Config::fx(), Config::fy(),
                                       Config::cx(), Config::cy(),
                                       lastRestartId);

                    printf("|==== Paired images from frame %d to %d are saved. ====|\n", lastRestartId, globalId);
                    usleep(10000);
                }

                //=== generate novel views
                if(pangolin::Pushed(*gui.generateNovelViews))
                /*
                {
                    // get novel view number
                    int novelViewNum = gui.novelViewNum->Get();

                    novelViews.clear();

                    std::vector<Eigen::Matrix4f> views;
                    gui.getViews(views, modelPoses);  // todo

                    // random frame generator
                    std::default_random_engine g;
                    g.seed(time(0));
                    std::uniform_int_distribution<int> uniFrame(0, views.size());
                    // random translation generator
                    std::uniform_real_distribution<float> uniTransX(-2, 2);
                    std::uniform_real_distribution<float> uniTransZ(-1, 1);
                    // random angle generator
                    std::uniform_real_distribution<float> uniAngle(-15, 15);
                    for(int i = 0; i < 100 * novelViewNum; ++i)
                    {
                        // choose a random view
                        int frame_num = uniFrame(g);
                        auto v = views[frame_num];

                        float x_off = uniTransX(g);
                        float z_off = uniTransZ(g);
                        float theta_off = uniAngle(g) * M_PI / 180;

                        auto rotation = Eigen::AngleAxis<float>(theta_off, Eigen::Vector3f(0, -1, 0));
                        auto translation = Eigen::Translation3f(x_off, 0, z_off);

                        Eigen::Transform<float, 3, Eigen::Affine> T;
                        T = translation * rotation;

                        v = v * T.matrix();

                        novelViews.push_back(v);
                    }

                    S_shaped_novel = false;
                }
                */ // generate random novel views

                {
                    novelViewLeft.clear();

                    std::vector<Eigen::Matrix4f> views;
                    gui.getViews(views, modelPoses);  // todo

                    // novel left view
                    for(int i = 0; i < views.size(); ++i)
                    {
                        auto v = views[i];

                        float x_off = -3;
                        float z_off = 0;
                        float theta_off = -10 * M_PI / 180;

                        auto rotation = Eigen::AngleAxis<float>(theta_off, Eigen::Vector3f(0, -1, 0));
                        auto translation = Eigen::Translation3f(x_off, 0, z_off);

                        Eigen::Transform<float, 3, Eigen::Affine> T;
                        T = translation * rotation;

                        v = v * T.matrix();

                        novelViewLeft.push_back(v);
                    }

                    novelViewRight.clear();
                    // novel right view
                    for(int i = 0; i < views.size(); ++i)
                    {
                        auto v = views[i];

                        float x_off = 2;
                        float z_off = 0;
                        float theta_off = 10 * M_PI / 180;

                        auto rotation = Eigen::AngleAxis<float>(theta_off, Eigen::Vector3f(0, -1, 0));
                        auto translation = Eigen::Translation3f(x_off, 0, z_off);

                        Eigen::Transform<float, 3, Eigen::Affine> T;
                        T = translation * rotation;

                        v = v * T.matrix();

                        novelViewRight.push_back(v);
                    }

                    S_shaped_novel = false;
                }


                //=== generate "S"-shaped novel views
                if(pangolin::Pushed(*gui.generate_S_views))
                {
                    novelViewS.clear();

                    // get novel view sinusoidal period
                    int novelViewNum = gui.novelViewNum->Get() * 3;

                    std::vector<Eigen::Matrix4f> views;
                    gui.getViews(views, modelPoses);  // todo
                    novelViewS.reserve(views.size());

                    auto curr_v = views[0];
                    auto last_v = curr_v;
                    double total_dist = 0;
                    float max_trans_offset = 2;
                    float max_theta_offset = 15 * M_PI / 180;
                    for(int i = 0; i < views.size(); ++i)
                    {
                        curr_v = views[i];
                        auto step_trans = curr_v.topRightCorner<3, 1>() - last_v.topRightCorner<3, 1>();
                        double step_dist = step_trans.norm();
                        total_dist += step_dist;
                        //printf("step: %f, total: %f\n", step_dist, total_dist);

                        float x_off = sin(total_dist / novelViewNum) * max_trans_offset;
                        float z_off = 0;
                        float theta_off = -cos(total_dist / novelViewNum) * max_theta_offset;

                        auto rotation = Eigen::AngleAxis<float>(theta_off, Eigen::Vector3f(0, -1, 0));
                        auto translation = Eigen::Translation3f(x_off, 0, z_off);

                        Eigen::Transform<float, 3, Eigen::Affine> T;
                        T = translation * rotation;
                        novelViewS.emplace_back(curr_v * T.matrix());

                        last_v = curr_v;
                    }

                    S_shaped_novel = true;
                }

                //=== If acquire novel images
                if(pangolin::Pushed(*gui.acquireNovelImage))
                {
                    std::string data_path = "./load_map_output/novel_S";  // todo

                    if(S_shaped_novel)
                    {
                        // remove the start 4 frames
                        std::vector<Eigen::Matrix4f> render_views(novelViewS.begin() + 4, novelViewS.end());

                        core.acquireImages(data_path, render_views,
                                           Config::W(), Config::H(),
                                           Config::fx(), Config::fy(),
                                           Config::cx(), Config::cy(),
                                           lastRestartId + 4);
                        printf("|==== Novel images from frame %d to %d are saved. ====|\n", lastRestartId + 4, globalId);
                    }
                    else
                    {

                        // save novel left image
                        std::string data_left_path = "./load_map_output/novel_left";  // todo
                        core.acquireImages(data_left_path, novelViewLeft,
                                           Config::W(), Config::H(),
                                           Config::fx(), Config::fy(),
                                           Config::cx(), Config::cy(),
                                           0); // totalNovelViewNum
                        printf("|==== Novel left images from frame %d to %d are saved. ====|\n", 0, novelViewLeft.size() - 1);
                        //totalNovelViewNum += novelViews.size();


                        // save novel right image
                        std::string data_right_path = "./load_map_output/novel_right";  // todo
                        core.acquireImages(data_right_path, novelViewRight,
                                           Config::W(), Config::H(),
                                           Config::fx(), Config::fy(),
                                           Config::cx(), Config::cy(),
                                           0);
                        printf("|==== Novel right images from frame %d to %d are saved. ====|\n", 0,  + novelViewRight.size() - 1);
                        break;

                    }

                    usleep(10000);
                }

                if (autoGenerateImage) {
                    // Acquire Images
                    std::string data_path = "./load_map_output/paired";  // todo

                    std::vector<Eigen::Matrix4f> views;
                    gui.getViews(views, modelPoses);  // todo

                    core.acquireImages(data_path, views,
                                       Config::W(), Config::H(),
                                       Config::fx(), Config::fy(),
                                       Config::cx(), Config::cy(),
                                       lastRestartId);

                    printf("|==== Paired images from frame %d to %d are saved. ====|\n", lastRestartId, globalId);
                    usleep(10000);

                    // save novel left image
                    std::string data_left_path = "./load_map_output/novel_left";  // todo
                    core.acquireImages(data_left_path, novelViewLeft,
                                       Config::W(), Config::H(),
                                       Config::fx(), Config::fy(),
                                       Config::cx(), Config::cy(),
                                       0); // totalNovelViewNum
                    printf("|==== Novel left images from frame %d to %d are saved. ====|\n", 0,
                           novelViewLeft.size() - 1);
                    //totalNovelViewNum += novelViews.size();


                    // save novel right image
                    std::string data_right_path = "./load_map_output/novel_right";  // todo
                    core.acquireImages(data_right_path, novelViewRight,
                                       Config::W(), Config::H(),
                                       Config::fx(), Config::fy(),
                                       Config::cx(), Config::cy(),
                                       0);
                    printf("|==== Novel right images from frame %d to %d are saved. ====|\n", 0,
                           +novelViewRight.size() - 1);

                    usleep(10000);

                    return;
                }

            }


            //============ overview
            if(!follow_pose && gui.overview->Get())
            {
                pangolin::OpenGlMatrix mv;

                Eigen::Matrix3f currRot = pose.topLeftCorner(3, 3);

                Eigen::Vector3f forwardVector(0, 0, 1);
                Eigen::Vector3f upVector(0, -1, 0);

                Eigen::Vector3f forward = (currRot * forwardVector).normalized();
                Eigen::Vector3f up = (currRot * upVector).normalized();

                Eigen::Vector3f viewAt(pose(0, 3), pose(1, 3) - 5, pose(2, 3));

                Eigen::Vector3f eye = viewAt - forward;

                Eigen::Vector3f z = (eye - viewAt).normalized();  // Forward, OpenGL camera z direction
                Eigen::Vector3f x = up.cross(z).normalized();     // Right
                Eigen::Vector3f y = z.cross(x);                   // Up

                Eigen::Matrix4d m;                                // [R; U; F]_4x4 * [E; -eye]_4x4
                m << x(0),  x(1),  x(2),  -(x.dot(eye)),
                        y(0),  y(1),  y(2),  -(y.dot(eye)),
                        z(0),  z(1),  z(2),  -(z.dot(eye)),
                        0,     0,     0,      1;

                memcpy(&mv.m[0], m.data(), sizeof(Eigen::Matrix4d));

                gui.s_cam.SetModelViewMatrix(mv);

                overviewId = ++overviewId % modelPoses.size();

                usleep(100000);
            }



            //============ draw global model
            int surfel_mode = gui.drawGlobalModel->Get();

            if(surfel_mode)
                core.getGlobalModel().renderModel(gui.s_cam.GetProjectionModelViewMatrix(),
                                                  gui.s_cam.GetModelViewMatrix(),
                                                  0.0,
                                                  true,
                                                  surfel_mode == 3,
                                                  surfel_mode == 4,
                                                  surfel_mode == 1,
                                                  false,
                                                  surfel_mode == 5,
                                                  3,
                                                  3);


            gui.drawFrustum(core.getCurrPose());


            gui.postCall();



        }


    }
}



int main(int argc, char ** argv)
//int loading_map(string kittiDir, string model_path, float diff, float r, string depth_dir, int file_name_width)
{
    //##################### Parameters #####################
    std::string kittiDir(argv[1]);
    std::string model_path(argv[2]);
    std::string diff_tmp(argv[3]);
    float diff = std::stod(diff_tmp);
    std::string r_tmp(argv[4]);
    float r = std::stod(r_tmp);
    std::string depth_dir(argv[5]);

    std::string name_length(argv[6]);
    int file_name_width = std::stod(name_length);
    //######################################################

    KittiReader reader(kittiDir, false, false, 0, true, depth_dir);

    Config::getInstance(reader.fx(), reader.fy(), reader.cx(), reader.cy(), reader.H(), reader.W(), diff, r);

    GUI gui(reader.W(), reader.H(), GUI::ShowMode::minimum);

    SurfelMapping core;

    vector<int> startEndIds;

    core.getGlobalModel().uploadMap(model_path, startEndIds);

    if(!startEndIds.empty())
    {
        lastRestartId = startEndIds[0];
        globalId = startEndIds[1];
        printf("Model from frame %d to %d.\n", lastRestartId, globalId);
    }

    int frame_id = lastRestartId - 1;
    reader.setState(frame_id);
    modelPoses.clear();
    while (reader.getNext(file_name_width))
    {
        cout << reader.currentFrameId << "<-" << '\n';

        if(reader.currentFrameId > globalId)
            break;

        modelPoses.push_back(reader.gtPose);
    }

    //  generate view




    // save image


    rungui(core, gui);
    // show after loop
//    while (true)
//    {
//        rungui(core, gui);
//        if(!core.getBeginCleanPoints())
//            break;
//    }

}

//#include <pybind11/pybind11.h>
//namespace  py=pybind11;
//
//PYBIND11_MODULE(load_map, m){
//    m.doc()="Adaptive surfel mapping step1-build map";
//    m.def("loading_map", &loading_map, "Building GSM and save .bin file in the local dictionary.");
//}

