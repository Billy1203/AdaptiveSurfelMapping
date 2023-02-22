param1=6;
for param2 in $(seq 20 50);
do
	python load_map.py /home/yiheng/dataset/carla/map1_scene1_output tmp_model.bin $param2

	python move_data.py $param1 $param2
done
