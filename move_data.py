"""Move generated novel outputs into dataset folders with index offset."""

import argparse
import shutil
from pathlib import Path


def parse_args():
    parser = argparse.ArgumentParser(description="Move generated novel image/semantic files with index offset.")
    parser.add_argument("--offset", type=int, required=True, help="Frame number offset to add")
    parser.add_argument("-t", "--destination", type=str, required=True, help="Destination dataset directory")
    parser.add_argument("-f", "--fake", action="store_true", help="Only print operations")
    parser.add_argument("--source", type=str, default="./output/novel", help="Source novel output directory")
    return parser.parse_args()


def main():
    args = parse_args()
    source_root = Path(args.source).expanduser().resolve()
    dest_root = Path(args.destination).expanduser().resolve()

    for data_dir in ("image", "semantic"):
        src_dir = source_root / data_dir
        dst_dir = dest_root / data_dir

        if not src_dir.exists():
            print(f"[WARN] source directory not found: {src_dir}")
            continue

        dst_dir.mkdir(parents=True, exist_ok=True)
        src_files = sorted([p for p in src_dir.iterdir() if p.is_file() and p.suffix.lower() == ".png"])

        for src_file in src_files:
            base_id = int(src_file.stem)
            dest_name = f"{base_id + args.offset:06d}.png"
            dst_file = dst_dir / dest_name

            print(f"mv {src_file} {dst_file}")
            if not args.fake:
                shutil.move(str(src_file), str(dst_file))


if __name__ == "__main__":
    main()
