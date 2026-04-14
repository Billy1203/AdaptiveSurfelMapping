import argparse
from pathlib import Path

import cv2
import numpy as np


def image_filter(img, sem, target_rgb):
    if img is None or sem is None:
        raise ValueError("image or semantic image is None.")
    if img.shape != sem.shape:
        raise ValueError("image and semantic map have different shapes.")

    # Remove pixels whose semantic color exactly matches target_rgb.
    target = np.array(target_rgb, dtype=np.uint8).reshape(1, 1, 3)
    keep_mask = np.any(sem != target, axis=2).astype(np.uint8) * 255
    keep_mask_bgr = cv2.cvtColor(keep_mask, cv2.COLOR_GRAY2BGR)
    return cv2.bitwise_and(img, keep_mask_bgr)


def parse_args():
    parser = argparse.ArgumentParser(description="Filter semantic class pixels from CARLA RGB images.")
    parser.add_argument("dataset_path", help="Formatted dataset root path")
    parser.add_argument(
        "--target-rgb",
        nargs=3,
        type=int,
        default=[180, 130, 70],
        help="Semantic RGB value to remove, default is sky class in CARLA",
    )
    return parser.parse_args()


def main():
    args = parse_args()
    dataset_root = Path(args.dataset_path).expanduser().resolve()
    if not dataset_root.exists():
        raise FileNotFoundError(f"Dataset path does not exist: {dataset_root}")

    for image_dir in ["image_2", "image_3", "image_val_0", "image_val_1"]:
        sem_dir = f"{image_dir}_semantic"
        filtered_dir = f"{image_dir}_filtered"

        image_root = dataset_root / image_dir
        sem_root = dataset_root / sem_dir
        out_root = dataset_root / filtered_dir
        out_root.mkdir(parents=True, exist_ok=True)

        if not image_root.exists() or not sem_root.exists():
            print(f"[WARN] skip {image_dir}: missing {image_root} or {sem_root}")
            continue

        image_names = sorted([p.name for p in image_root.iterdir() if p.suffix.lower() == ".png"])
        for image_name in image_names:
            image_path = image_root / image_name
            sem_path = sem_root / image_name
            out_path = out_root / image_name
            if not sem_path.exists():
                print(f"[WARN] missing semantic file: {sem_path}")
                continue

            img = cv2.imread(str(image_path))
            sem = cv2.imread(str(sem_path))
            filtered_img = image_filter(img, sem, args.target_rgb)
            cv2.imwrite(str(out_path), filtered_img)

        print(f"{image_dir} finished.")


if __name__ == "__main__":
    main()





