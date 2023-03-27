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
  r0(r),
  diff0(diff)
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

    //depth_dic_length = 6;
    vertex_size = sizeof(Eigen::Vector4f) * 3;
    near_clip = 1.0f;                                         // the min depth processed 1.0f
    far_clip = 80.0f;                                         // the max depth processed 50.0f

    max_sqrt_vertices = 5000;
    depth_padding = 0.0f;                                    // depth padding width 80.0f
    depth_diff_thresh = 5.0f;                               // threshold of 7-support pixels 0.15f

    //r0 = 1.0;
    //r = 10.0f;

    // Parameters in building map
    //diff_thresh: argv[1] param1, two surfels fusion thresh


    // Parameters in loading map
    // r_0: argv[2] param2, surfel resize times


}

Config & Config::getInstance(float fx, float fy, float cx, float cy, int rows, int cols, float diff, float r)
{
    static Config instance(fx, fy, cx, cy, rows, cols, diff, r);
    return instance;
}
