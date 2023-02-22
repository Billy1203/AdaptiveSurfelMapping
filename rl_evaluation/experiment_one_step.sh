
python build_map.py /home/yiheng/dataset/carla/map1_scene1_output ./tmp_model23.bin $1 $2

python load_map.py /home/yiheng/dataset/carla/map1_scene1_output ./tmp_model23.bin $1 $2

python move_data.py $1 $2
