python_executable="/opt/miniconda3/envs/surfel/bin/python3"
build_map_executable="build/build_map"
dataset_path="/DATA/carla_scene3_forest"

$python_executable src/build_map.py $dataset_path ./tmp_carla03.bin 0.085355 0.73227 depth_2 10
$python_executable src/load_map.py $dataset_path ./tmp_carla03.bin 0.085355 0.73227 depth_2 10
$python_executable src/move_data.py $dataset_path 0.085355 0.73227