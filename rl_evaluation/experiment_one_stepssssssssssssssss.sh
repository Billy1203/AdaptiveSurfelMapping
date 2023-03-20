#/home/yiheng/dataset/carla/carla_scene3_forest/


python src/build_map.py $1 ./tmp_carla03.bin 0.085355 0.73227 depth_2 10
python src/load_map.py $1 ./tmp_carla03.bin 0.085355 0.73227 depth_2 10
python src/move_data.py $1 0.085355 0.73227

python src/build_map.py $1 ./tmp_carla03.bin 0.05 2.5 depth_2 10
python src/load_map.py $1 ./tmp_carla03.bin 0.05 2.5 depth_2 10
python src/move_data.py $1 0.05 2.5

python src/build_map.py $1 ./tmp_carla03.bin 0.05 0 depth_2 10
python src/load_map.py $1 ./tmp_carla03.bin 0.05 0 depth_2 10
python src/move_data.py $1 0.05 0


python src/build_map.py $1 ./tmp_carla03.bin 0.014645 2.5 depth_2 10
python src/load_map.py $1 ./tmp_carla03.bin 0.014645 2.5 depth_2 10
python src/move_data.py $1 0.014645 2.5

python src/build_map.py $1 ./tmp_carla03.bin 0.085355 4.26777 depth_2 10
python src/load_map.py $1 ./tmp_carla03.bin 0.085355 4.26777 depth_2 10
python src/move_data.py $1 0.085355 4.26777


python src/build_map.py $1 ./tmp_carla03.bin 0.014645 4.26777 depth_2 10
python src/load_map.py $1 ./tmp_carla03.bin 0.014645 4.26777 depth_2 10
python src/move_data.py $1 0.014645 4.26777


python src/build_map.py $1 ./tmp_carla03.bin 0.014645 0.73223 depth_2 10
python src/load_map.py $1 ./tmp_carla03.bin 0.014645 0.73223 depth_2 10
python src/move_data.py $1 0.014645 0.73223

python src/build_map.py $1 ./tmp_carla03.bin 0.05 2.1 depth_2 10
python src/load_map.py $1 ./tmp_carla03.bin 0.05 2.1 depth_2 10
python src/move_data.py $1 0.05 2.1


python src/build_map.py $1 ./tmp_carla03.bin 0 2.5 depth_2 10
python src/load_map.py $1 ./tmp_carla03.bin 0 2.5 depth_2 10
python src/move_data.py $1 0 2.5


python src/build_map.py $1 ./tmp_carla03.bin 0.1 2.5 depth_2 10
python src/load_map.py $1 ./tmp_carla03.bin 0.1 2.5 depth_2 10
python src/move_data.py $1 0.1 2.5


python src/build_map.py $1 ./tmp_carla03.bin 0.03 2.5 depth_2 10
python src/load_map.py $1 ./tmp_carla03.bin 0.03 2.5 depth_2 10
python src/move_data.py $1 0.03 2.5


python src/build_map.py $1 ./tmp_carla03.bin 0.07 2.5 depth_2 10
python src/load_map.py $1 ./tmp_carla03.bin 0.07 2.5 depth_2 10
python src/move_data.py $1 0.07 2.5


python src/build_map.py $1 ./tmp_carla03.bin 0.05 3.5 depth_2 10
python src/load_map.py $1 ./tmp_carla03.bin 0.05 3.5 depth_2 10
python src/move_data.py $1 0.05 3.5


python src/build_map.py $1 ./tmp_carla03.bin 0.05 5 depth_2 10
python src/load_map.py $1 ./tmp_carla03.bin 0.05 5 depth_2 10
python src/move_data.py $1 0.05 5