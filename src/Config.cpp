//
// Created by zhijun on 2021/5/22.
// Edited by yiheng on 2023/2/18.
//

#include "Config.h"

Config::Config(float fx, float fy, float cx, float cy, int rows, int cols, float r)
: fx_(fx),
  fy_(fy),
  cx_(cx),
  cy_(cy),
  rows_(rows),
  cols_(cols),
  num_pixels(rows * cols),

  r0(r)
{
    //============ Shader =====================//
    /* Single surfel model:
     *--------------------
     * vec3 position
     * float confidence
     *
     * float color + class (24-bit rgb + 8-bit class)
     * float <standby>  for feasible usage
     * float initTime
     * float timestamp
     *
     * vec3 normal
     * float radius
     *--------------------
     * Which is three vec4s
     */
    vertex_size = sizeof(Eigen::Vector4f) * 3;
    near_clip = 1.0f;                                         // the min depth processed
    far_clip = 50.0f;                                         // the max depth processed
    surfel_fuse_distance_threshold_factor = 0.0f;             // modify threshold of two sufels

    max_sqrt_vertices = 5000;
    depth_padding = 10.0f;                                    // depth padding width

    //r0 = 1.0;

    //r = 1.5;

    // Parameters in building map
    //diff_thresh
    //h_0



    // Parameters in loading map
    // r_0


}

Config & Config::getInstance(float fx, float fy, float cx, float cy, int rows, int cols, float r)
{
    static Config instance(fx, fy, cx, cy, rows, cols, r);
    return instance;
}
