for param1 in $(seq 5 45); # param1 for build_map, from 0.5 to 4.5

do
  python build_map.py /home/yiheng/dataset/carla/map1_scene1_output tmp_model.bin $param1 15
  for param2 in $(seq 50); # param2 for load_map, from 0.1 to 5.0
  do
    python load_map.py /home/yiheng/dataset/carla/map1_scene1_output tmp_model.bin $param1 $param2
    python move_data.py $param1 $param2
  done
done
