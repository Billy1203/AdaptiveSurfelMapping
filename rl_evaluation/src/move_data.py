import os
import shutil
import sys
import cv2
import torch

from skimage.metrics import peak_signal_noise_ratio as compare_psnr
from skimage.metrics import structural_similarity as compare_ssim

sys.path.append("./PerceptualSimilarity-1.0/")
import models
from util import util


def move_dataset(param1, param2):
    """
        move data from 'aaa' to specific dataset

        - paired
            -0.1_0.5.png
            -0.1_0.8.png
    """
    image_dataset = "./load_map_output/"
    select_index = "0000000080.png"

    lpips_model = models.PerceptualLoss(use_gpu=False)
    print(">" * 17, "param1=%.1f param2=%.1f" % (param1, param2), "<" * 17)

    for _sub_dic_name in ["paired", "novel_left", "novel_right"]:

        gt_image_path = "./gt/" + _sub_dic_name + ".png"

        # if not exist
        if _sub_dic_name not in os.listdir("./"):
            os.mkdir(_sub_dic_name)

        new_image_name = str(param1) + "_" + str(param2) + ".png"

        ###################### Image movement ######################
        original_image_path = image_dataset + _sub_dic_name + "/" + select_index
        new_image_path = "./" + _sub_dic_name + "/" + new_image_name
        shutil.copy(original_image_path, new_image_path)
        # print(original_image_path, new_image_path)

        ###################### Image evaluation ######################
        original_img = cv2.imread(original_image_path)
        gt_img = cv2.imread(gt_image_path)
        # print(original_image_path, gt_image_path)

        psnr = compare_psnr(gt_img, original_img)
        ssim = compare_ssim(gt_img, original_img, channel_axis=2, data_range=255)

        gt_tensor = util.im2tensor(gt_img)
        original_tensor = util.im2tensor(original_img)
        lpips = torch.square(lpips_model.forward(gt_tensor, original_tensor))

        print("[%11s]\tpsnr=%.5f ssim=%.5f lpips=%.5f" % (_sub_dic_name, psnr, ssim, lpips))


if __name__ == '__main__':
    param1 = float(int(sys.argv[1]) / 100)
    param2 = float(int(sys.argv[2]) / 10)

    move_dataset(param1, param2)
