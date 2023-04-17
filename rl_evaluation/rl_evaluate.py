import sys
import os
import subprocess

from rl_evaluation.src.move_data import move_dataset

dataset_path = "/DATA/carla_scene3_forest/"
model_path = "./tmp_carla03.bin"
# param1 = None  # d, 0.05-0.45
# param2 = None  # r, 0-5.0
depth_dir = "depth_2"
file_name_width = "10"

psnr_history = []
ssim_history = []
lpips_history = []


if __name__ == '__main__':
    param1 = 0.085355
    param2 = 0.73227

    cmd_build = ['python3', 'src/build_map.py', dataset_path, model_path, str(param1), str(param2), depth_dir, file_name_width]
    cmd_load = ['python3', 'src/load_map.py', dataset_path, model_path, str(param1), str(param2), depth_dir, file_name_width]
    subprocess.call(cmd_build, shell=False)
    subprocess.call(cmd_load, shell=False)

    # os.system(f"sh experiment.sh {dataset_path} {model_path} {param1} {param2} {depth_dir} {file_name_width}")
    _sub_dic_name, avg_psnr, avg_ssim, avg_lpips = move_dataset(dataset_path, param1, param2)
    psnr_history.append(avg_psnr)
    ssim_history.append(avg_ssim)
    lpips_history.append(avg_lpips)
