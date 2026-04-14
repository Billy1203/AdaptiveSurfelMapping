import argparse
from pathlib import Path

import build_map


def parse_args():
    parser = argparse.ArgumentParser(description="Build global surfel map.")
    parser.add_argument("dataset_path", help="Dataset root path")
    parser.add_argument("model_path", help="Output .bin model path")
    parser.add_argument("param1", type=float, help="Depth fusion threshold (diff_thresh)")
    parser.add_argument("param2", type=float, help="Surfel radius scale parameter (r0)")
    parser.add_argument("depth_dir", help="Depth directory name under dataset root")
    parser.add_argument("file_name_width", type=int, help="Frame filename width, e.g. 6 or 10")
    return parser.parse_args()


if __name__ == "__main__":
    args = parse_args()
    dataset_path = Path(args.dataset_path).expanduser().resolve()
    if not dataset_path.exists():
        raise FileNotFoundError(f"Dataset path does not exist: {dataset_path}")

    print(f"++++diff_thresh={args.param1:.2f}++++r0={args.param2:.2f}")
    build_map.building_map(
        str(dataset_path),
        args.model_path,
        args.param1,
        args.param2,
        args.depth_dir,
        args.file_name_width,
    )
