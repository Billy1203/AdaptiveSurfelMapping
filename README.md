# Adaptive Surfel Mapping

Real time traffic scene mapping using dense surfel representation.

This repo is part of the code of Adaptive Large-Scale Novel View Image Synthesis for Autonomous Driving Datasets. The SurfelMapping rebuilds the 3D traffic scene from images collected by stereo camera of a driving vihecle. The 3D model is represented by surfels to make the rendered images more realistic.

# 1. Dependencies

1. Tested on Ubuntu 20.04 & Ubuntu 18.04
2. CMake 3.16.3
3. OpenGL
4. Eigen
5. https://github.com/stevenlovegrove/Pangolin build from source
6. https://github.com/pybind/pybind11 optional, calling opengl methods in python
7. https://github.com/NVlabs/SPADE optional, for pose-processing
8. Not too small GPU memory, we tested on NVIDIA GTX 1080ti, GPU memory=12GB

# 2. Dataset Preparation

## 2.1 carlaDatasetTools

## 2.2 psmnet depth generate

# 3. Build

## 3.1. Build in C++ for testing

```bash
git clone https://github.com/Billy1203/AdaptiveSurfelMapping.git

cd AdaptiveSurfelMapping & mkdir build & cd build
cmake ..
make -j12

# Step1: to build the surfel map, run
./build_map [path to the data super dir] [path for model saving] [param1] [param2]
# In the GUI window, uncheck "pause" button and run the mapping. Click "save" for saving the built map.

# Step2: to load saved map and generate new data, run
./load_map [path to the data super dir] [saved model path] [param1] [param2]
# Click "path mode" and generate novel views as shown and instructed in _loadmap.gif_. Then click "Acquire Novel Images" to get new images of those views.
```

![image](./loadmap.gif)

## 3.2. Build in Python for response surface methodology

```bash
# Modify CMakeList.txt, build_map.cpp and load_map.cpp
# Generate build_map.so and load_map.so can be called in python

cd rl_evaluation
# Step1: evaluation for one pair of parameters
./experiment_one_step.sh [param1] [param2] # recommand param1 in (0.5, 4.5), param2 in (0.1, 5.0)
# Show the evaluation values in terminal, using them in minitab for RSM

# Step2: evaluation for 2,050 pairs of parameters
./experiment_batch.sh
# Generate 2,050 images for each experiment in the directories, compare them later.
```

# 4. Usage

The program requires RGB images and corresponding DEPTH and SEMANTIC maps as input. We use some third part learning methods prediction to provide dense depth map and semantic labels. You can download our demo data [here](https://drive.google.com/file/d/1uKM7Gbs_Hy99OwrfqNmNIAZQuydQ_Gdw/view?usp=sharing) (using [PSMNet](https://github.com/JiaRenChang/PSMNet) for depth and [PointRend](https://github.com/facebookresearch/detectron2/tree/main/projects/PointRend) for semantic). You can use your own data including RGB, depth and semantic. The RGB, depth and semantic subdirectories should be in a same super directory and set the subdir name in the KittiReader. See `KittiReader.cpp` for details of input path. If you use the demo data, you do not need to change it.

## 4.1 Ground truth depth

1. Modify in `KittiReader.cpp`, change the path of depth directory.
2. Use

## 4.2 PSMNet depth

1. Modify the depth directory path in `KittiReader.cpp`
2. evaluation directly


# SPADE
The work in _SPADE_ dir is forked from https://github.com/NVlabs/SPADE. We modified the input and some other parts. a _postprocess.py_ is also added for synthesizing final images from the GAN generated image and rendered image. Please go to original [SPADE](https://github.com/NVlabs/SPADE) to see the training and testing of the code.


# Acknowledgements
Our code is inspired by [ElasticFusion](https://www.imperial.ac.uk/dyson-robotics-lab/downloads/elastic-fusion/).
