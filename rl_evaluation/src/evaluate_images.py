import argparse
import sys
from pathlib import Path

import cv2
import numpy as np
import torch
from skimage.metrics import peak_signal_noise_ratio as compare_psnr
from skimage.metrics import structural_similarity as compare_ssim

PROJECT_DIR = Path(__file__).resolve().parents[1]
LPIPS_DIR = PROJECT_DIR / "PerceptualSimilarity-1.0"
sys.path.insert(0, str(LPIPS_DIR))

import models  # noqa: E402
from util import util  # noqa: E402


def parse_args():
    parser = argparse.ArgumentParser(description="Evaluate paired images against GT using PSNR/SSIM/LPIPS.")
    parser.add_argument("kitti_gt_path", help="Ground-truth image directory")
    parser.add_argument("paired_path", help="Predicted image directory")
    parser.add_argument(
        "--frames",
        type=int,
        nargs="+",
        default=[140, 150, 160, 170, 180, 190, 200, 210, 220, 230, 240, 250, 260, 270, 280],
        help="Frame indices to evaluate",
    )
    return parser.parse_args()


def _frame_name(frame_idx):
    return f"{frame_idx:010d}.png"


def evaluate_images(kitti_gt_path, paired_path, frames):
    gt_root = Path(kitti_gt_path).expanduser().resolve()
    pred_root = Path(paired_path).expanduser().resolve()
    if not gt_root.exists():
        raise FileNotFoundError(f"GT path does not exist: {gt_root}")
    if not pred_root.exists():
        raise FileNotFoundError(f"Prediction path does not exist: {pred_root}")

    lpips_model = models.PerceptualLoss(use_gpu=False)
    psnr_values = []
    ssim_values = []
    lpips_values = []

    for frame_idx in frames:
        frame_name = _frame_name(frame_idx)
        gt_image_path = gt_root / frame_name
        pred_image_path = pred_root / frame_name

        if not gt_image_path.exists() or not pred_image_path.exists():
            print(f"[WARN] skip frame {frame_name}: missing pred/gt file")
            continue

        pred_img = cv2.imread(str(pred_image_path))
        gt_img = cv2.imread(str(gt_image_path))
        if pred_img is None or gt_img is None:
            print(f"[WARN] skip frame {frame_name}: failed to decode image")
            continue

        psnr = compare_psnr(gt_img, pred_img)
        ssim = compare_ssim(gt_img, pred_img, channel_axis=2, data_range=255)

        gt_tensor = util.im2tensor(gt_img)
        pred_tensor = util.im2tensor(pred_img)
        lpips = torch.square(lpips_model.forward(gt_tensor, pred_tensor))
        lpips_value = float(lpips.detach().numpy()[0][0][0][0])

        psnr_values.append(psnr)
        ssim_values.append(ssim)
        lpips_values.append(lpips_value)

    if not psnr_values:
        raise RuntimeError("No valid frame pairs were found for evaluation.")

    print(f"psnr={np.mean(psnr_values):.5f} ssim={np.mean(ssim_values):.5f} lpips={np.mean(lpips_values):.5f}")
    print(f"max_psnr={np.max(psnr_values):.5f} max_ssim={np.max(ssim_values):.5f} min_lpips={np.min(lpips_values):.5f}")


if __name__ == "__main__":
    args = parse_args()
    evaluate_images(args.kitti_gt_path, args.paired_path, args.frames)
