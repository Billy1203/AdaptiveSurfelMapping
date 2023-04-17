#!/usr/bin/zsh

python_executable="/opt/miniconda3/envs/surfel/bin/python3"
build_map_executable="build/build_map"
dataset_path="/DATA/carla_scene3_forest"

$build_map_executable $dataset_path ./tmp_carla03.bin 0.085355 0.73227 depth_2 10