//
// Created by zhijun on 2020/8/15.
//

#include <pangolin/pangolin.h>
#include <pangolin/scene/axis.h>
#include <pangolin/scene/scenehandler.h>
#include <pangolin/gl/gldraw.h>

int main( int /*argc*/, char** /*argv*/ )
{
    pangolin::CreateWindowAndBind("Main",640,480);
    glEnable(GL_DEPTH_TEST);

    // Define Projection and initial ModelView matrix
    pangolin::OpenGlRenderState s_cam(
            pangolin::ProjectionMatrix(640,480,420,420,320,240,0.2,100),
            pangolin::ModelViewLookAt(-2,2,-2, 0,0,0, pangolin::AxisY)
    );

    pangolin::Renderable tree;
    tree.Add( std::make_shared<pangolin::Axis>() );

    // Create Interactive View in window
    pangolin::SceneHandler handler(tree, s_cam);
    pangolin::View& d_cam = pangolin::CreateDisplay()
            .SetBounds(0.0, 1.0, 0.0, 1.0, -640.0f/480.0f)
            .SetHandler(&handler);

    d_cam.SetDrawFunction([&](pangolin::View& view){
        view.Activate(s_cam);
        tree.Render();
    });

    while( !pangolin::ShouldQuit() )
    {


        // Clear screen and activate view to render into
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glLineWidth(1.0);
        glColor4f(1,0,0,1);
        pangolin::glDrawLine(0,2,0, 1,2,0);


        glColor4f(0,1,0,1);
        pangolin::glDrawLine(0,2,0, 0,3,0);


        glColor4f(0,0,1,1);
        pangolin::glDrawLine(0,2,0, 0,2,1);


        glLineWidth(3.0);

        // Swap frames and Process Events
        pangolin::FinishFrame();
    }

    return 0;
}
