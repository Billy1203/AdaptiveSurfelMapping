#/home/yiheng/dataset/carla/carla_scene3_forest/


python src/build_map.py $1 ./tmp_kitti01.bin 3.91421 4.28241 psmnet 6
python src/load_map.py $1 ./tmp_kitti01.bin 3.91421 4.28241 psmnet 6
python src/move_data.py 3.91421 4.28241


python src/build_map.py $1 ./tmp_kitti01.bin 2.5 2.55 psmnet 6
python src/load_map.py $1 ./tmp_kitti01.bin 2.5 2.55 psmnet 6
python src/move_data.py 2.5 2.55


python src/build_map.py $1 ./tmp_kitti01.bin 2.5 5 psmnet 6
python src/load_map.py $1 ./tmp_kitti01.bin 2.5 5 psmnet 6
python src/move_data.py 2.5 5


python src/build_map.py $1 ./tmp_kitti01.bin 2.5 0.1 psmnet 6
python src/load_map.py $1 ./tmp_kitti01.bin 2.5 0.1 psmnet 6
python src/move_data.py 2.5 0.1


python src/build_map.py $1 ./tmp_kitti01.bin 1.08579 0.81759 psmnet 6
python src/load_map.py $1 ./tmp_kitti01.bin 1.08579 0.81759 psmnet 6
python src/move_data.py 1.08579 0.81759


python src/build_map.py $1 ./tmp_kitti01.bin 0.5 2.55 psmnet 6
python src/load_map.py $1 ./tmp_kitti01.bin 0.5 2.55 psmnet 6
python src/move_data.py 0.5 2.55


python src/build_map.py $1 ./tmp_kitti01.bin 4.5 2.55 psmnet 6
python src/load_map.py $1 ./tmp_kitti01.bin 4.5 2.55 psmnet 6
python src/move_data.py 4.5 2.55


python src/build_map.py $1 ./tmp_kitti01.bin 3.91421 0.81579 psmnet 6
python src/load_map.py $1 ./tmp_kitti01.bin 3.91421 0.81579 psmnet 6
python src/move_data.py 3.91421 0.81579

python src/build_map.py $1 ./tmp_kitti01.bin 1.08579 4.28241 psmnet 6
python src/load_map.py $1 ./tmp_kitti01.bin 1.08579 4.28241 psmnet 6
python src/move_data.py 1.08579 4.28241