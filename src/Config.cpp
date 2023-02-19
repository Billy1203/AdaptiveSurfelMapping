//
// Created by zhijun on 2021/5/22.
// Edited by yiheng on 2023/2/18.
//

#include "Config.h"

Config::Config(float fx, float fy, float cx, float cy, int rows, int cols, float diff, float r)
: fx_(fx),
  fy_(fy),
  cx_(cx),
  cy_(cy),
  rows_(rows),
  cols_(cols),
  num_pixels(rows * cols),
  diff0(diff),
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
     *
     * config里面的常数可以传入到shader，如何引入build_map中的自变量，r和diff_thresh到config然后导入到shader？
     */
    vertex_size = sizeof(Eigen::Vector4f) * 3;
    near_clip = 1.0f;                                         // the min depth processed
    far_clip = 50.0f;                                         // the max depth processed
    //diff = 0.12f;             // modify threshold of two sufels

    max_sqrt_vertices = 5000;
    depth_padding = 0.0f;                                    // depth padding width

    //r0 = 1.0;

    //r = 10.0f;

    // Parameters in building map
    //diff_thresh
    //h_0



    // Parameters in loading map
    // r_0


}

Config & Config::getInstance(float fx, float fy, float cx, float cy, int rows, int cols, float diff, float r)
{
    static Config instance(fx, fy, cx, cy, rows, cols, diff, r);
    return instance;
}
