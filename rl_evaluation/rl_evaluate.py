import os
import shutil
import sys
import cv2
import torch
import numpy as np

from skimage.metrics import peak_signal_noise_ratio as compare_psnr
from skimage.metrics import structural_similarity as compare_ssim

# LPIPS environment
sys.path.append("./PerceptualSimilarity-1.0/")
import models
from util import util


def move_dataset(dataset_path, param1, param2):
    """
        move data from 'aaa' to specific dataset

        - paired
            -0.1_0.5.png
            -0.1_0.8.png
    """

    image_dataset = "../build/load_map_output/"
    select_index = [10, 20, 30, 40, 50, 60, 70, 80, 90]  # two numbers
    select_images = ["00000000%2s.png" % i for i in select_index]
    novel2gt = {"paired": "image_2_filtered", "novel_left": "image_val_0_filtered", "novel_right": "image_val_1_filtered"}

    lpips_model = models.PerceptualLoss(use_gpu=False)
    print(">" * 16, "param1=%.2f param2=%.2f" % (param1, param2), "<" * 16)

    for _sub_dic_name in ["paired", "novel_left", "novel_right"]:
        # for _sub_dic_name in ["paired"]:
        psnr_value_list = []
        lpips_value_list = []
        ssim_value_list = []
        for _select_image in select_images:
            gt_image_path = dataset_path + novel2gt[_sub_dic_name] + "/" + _select_image
            original_image_path = image_dataset + _sub_dic_name + "/" + _select_image
            # gt: carla scene1
            # gt2: kitti01
            # gt3: kitti02
            # gt4: kitti06

            # if not exist
            if _sub_dic_name not in os.listdir("./"):
                os.mkdir(_sub_dic_name)

            new_image_name = str(param1) + "_" + str(param2) + "_" + _select_image[-6:-4] + ".png"

            ###################### Image movement ######################
            # original_image_path = image_dataset + _sub_dic_name + "/" + select_index
            new_image_path = "./" + _sub_dic_name + "/" + new_image_name
            shutil.copy(original_image_path, new_image_path)
            # print(gt_image_path, original_image_path, new_image_path)

            ###################### Image evaluation ######################
            original_img = cv2.imread(original_image_path)
            gt_img = cv2.imread(gt_image_path)
            # print(original_image_path, gt_image_path)
            # print(original_image_path, gt_image_path)

            psnr = compare_psnr(gt_img, original_img)
            ssim = compare_ssim(gt_img, original_img, channel_axis=2, data_range=255)

            gt_tensor = util.im2tensor(gt_img)
            original_tensor = util.im2tensor(original_img)
            lpips = torch.square(lpips_model.forward(gt_tensor, original_tensor))
            lpips_tmp = lpips.detach().numpy()[0][0][0][0]
            # print(lpips.detach().numpy()[0][0][0][0])

            psnr_value_list.append(psnr)
            ssim_value_list.append(ssim)
            lpips_value_list.append(lpips_tmp)
        # print(psnr_value_list)
        avg_psnr = np.array(psnr_value_list).mean()
        avg_ssim = np.array(ssim_value_list).mean()
        avg_lpips = np.array(lpips_value_list).mean()

        print("[%11s]\tpsnr=%.5f ssim=%.5f lpips=%.5f" % (_sub_dic_name, avg_psnr, avg_ssim, avg_lpips))
        # print("%.5f\t%.5f\t%.5f" % (psnr, ssim, lpips))


if __name__ == '__main__':
    dataset_path = sys.argv[1]
    param1 = float(sys.argv[2])
    param2 = float(sys.argv[3])

    move_dataset(dataset_path, param1, param2)
