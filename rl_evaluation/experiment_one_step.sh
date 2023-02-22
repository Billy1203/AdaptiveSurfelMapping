python src/build_map.py /home/yiheng/dataset/carla/map1_scene1_output ./tmp_model.bin $1 $2

python src/load_map.py /home/yiheng/dataset/carla/map1_scene1_output ./tmp_model.bin $1 $2

python src/move_data.py $1 $2
