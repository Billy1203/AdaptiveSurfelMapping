#/home/yiheng/dataset/carla/carla_scene3_forest/ 0.2 3.1 depth_2 10

python src/build_map.py $1 ./tmp_carla01.bin $2 $3 $4 $5
python src/load_map.py $1 ./tmp_carla01.bin $2 $3 $4 $5
python src/move_data.py $1 $2 $3
