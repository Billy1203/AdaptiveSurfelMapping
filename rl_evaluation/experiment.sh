#!/usr/bin/zsh

python_executable="/opt/miniconda3/envs/surfel/bin/python3"
build_map_executable="../build/build_map"
dataset_path="/DATA/carla_scene3_forest"
depth_dir="/depth_2"
data_file="./tmp_carla03.bin"

cd /src/AdaptiveSurfelMapping/rl_evaluation || (echo "project path wrong" && exit)

$python_executable src/build_map.py "$1" "$2" "$3" "$4" "$5" "$6"
$python_executable src/load_map.py "$1" "$2" "$3" "$4" "$5" "$6"