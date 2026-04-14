import argparse
import shutil
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

NOVEL_TO_GT = {
    "paired": "image_2_filtered",
    "novel_left": "image_val_0_filtered",
    "novel_right": "image_val_1_filtered",
}


def parse_args():
    parser = argparse.ArgumentParser(description="Copy render outputs and evaluate PSNR/SSIM/LPIPS.")
    parser.add_argument("dataset_path", help="Formatted dataset root path")
    parser.add_argument("param1", type=float, help="diff_thresh")
    parser.add_argument("param2", type=float, help="r0")
    parser.add_argument(
        "--frames",
        type=int,
        nargs="+",
        default=[10, 20, 30, 40, 50, 60, 70, 80, 90],
        help="Frame indices to evaluate (default: 10..90 step 10)",
    )
    return parser.parse_args()


def _frame_name(frame_idx):
    return f"{frame_idx:010d}.png"


def _load_lpips_model():
    return models.PerceptualLoss(use_gpu=False)


def _compute_metrics(gt_image, pred_image, lpips_model):
    psnr = compare_psnr(gt_image, pred_image)
    ssim = compare_ssim(gt_image, pred_image, channel_axis=2, data_range=255)
    gt_tensor = util.im2tensor(gt_image)
    pred_tensor = util.im2tensor(pred_image)
    lpips = torch.square(lpips_model.forward(gt_tensor, pred_tensor))
    lpips_value = float(lpips.detach().numpy()[0][0][0][0])
    return psnr, ssim, lpips_value


def move_dataset(dataset_path, param1, param2, frames):
    dataset_root = Path(dataset_path).expanduser().resolve()
    if not dataset_root.exists():
        raise FileNotFoundError(f"Dataset path does not exist: {dataset_root}")

    image_dataset = PROJECT_DIR / "load_map_output"
    lpips_model = _load_lpips_model()

    print(">" * 16, f"param1={param1:.2f} param2={param2:.2f}", "<" * 16)

    for split_name in ("paired", "novel_left", "novel_right"):
        source_dir = image_dataset / split_name
        gt_dir = dataset_root / NOVEL_TO_GT[split_name]
        export_dir = PROJECT_DIR / split_name
        export_dir.mkdir(parents=True, exist_ok=True)

        if not source_dir.exists():
            raise FileNotFoundError(f"Missing render output directory: {source_dir}")
        if not gt_dir.exists():
            raise FileNotFoundError(
                f"Missing GT directory '{gt_dir}'. "
                f"Generate filtered images first (see docs/DATA_LAYOUT.md)."
            )

        psnr_values = []
        ssim_values = []
        lpips_values = []

        for frame_idx in frames:
            frame_name = _frame_name(frame_idx)
            pred_path = source_dir / frame_name
            gt_path = gt_dir / frame_name

            if not pred_path.exists() or not gt_path.exists():
                print(f"[WARN] skip frame {frame_name}: missing pred/gt file")
                continue

            new_name = f"{param1}_{param2}_{frame_name[-6:-4]}.png"
            shutil.copy2(pred_path, export_dir / new_name)

            pred_img = cv2.imread(str(pred_path))
            gt_img = cv2.imread(str(gt_path))
            if pred_img is None or gt_img is None:
                print(f"[WARN] skip frame {frame_name}: failed to decode image")
                continue

            psnr, ssim, lpips = _compute_metrics(gt_img, pred_img, lpips_model)
            psnr_values.append(psnr)
            ssim_values.append(ssim)
            lpips_values.append(lpips)

        if not psnr_values:
            print(f"[{split_name:>11s}]\tNo valid frames were evaluated.")
            continue

        print(
            f"[{split_name:>11s}]\t"
            f"psnr={np.mean(psnr_values):.5f} "
            f"ssim={np.mean(ssim_values):.5f} "
            f"lpips={np.mean(lpips_values):.5f}"
        )


if __name__ == "__main__":
    args = parse_args()
    move_dataset(args.dataset_path, args.param1, args.param2, args.frames)
