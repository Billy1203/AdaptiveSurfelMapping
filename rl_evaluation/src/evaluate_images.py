import os
import shutil
import sys
import cv2
import torch
import numpy as np

from skimage.metrics import peak_signal_noise_ratio as compare_psnr
from skimage.metrics import structural_similarity as compare_ssim

# LPIPS environment
sys.path.append("../PerceptualSimilarity-1.0/")
import models
from util import util


def evaluate_images(kitti_gt_path, paired_path):


    image_dataset = "./load_map_output/"
    # select_index = "0000000080.png"
    # select_index = "0000000033.png"
    select_index = [140, 150, 160, 170, 180, 190, 200, 210, 220, 230, 240, 250, 260, 270, 280]  # two numbers
    select_images = ["0000000%3s.png" % i for i in select_index]



    lpips_model = models.PerceptualLoss(use_gpu=False)

    # for _sub_dic_name in ["paired"]:
    psnr_value_list = []
    lpips_value_list = []
    ssim_value_list = []
    for _select_image in select_images:
        gt_image_path = kitti_gt_path + "/" + _select_image[-10:]
        original_image_path = paired_path + "/" + _select_image
        ###################### Image evaluation ######################
        original_img = cv2.imread(original_image_path)
        gt_img = cv2.imread(gt_image_path)
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

    print("psnr=%.5f ssim=%.5f lpips=%.5f" % (avg_psnr, avg_ssim, avg_lpips))
    print(np.array(psnr_value_list).max(), np.array(ssim_value_list).max(), np.array(lpips_value_list).min())
    print(psnr_value_list)
    print(ssim_value_list)
    print(lpips_value_list)
    # print("%.5f\t%.5f\t%.5f" % (psnr, ssim, lpips))


if __name__ == '__main__':
    kitti_gt_path = sys.argv[1]
    paired_path = sys.argv[2]


    evaluate_images(kitti_gt_path, paired_path)
