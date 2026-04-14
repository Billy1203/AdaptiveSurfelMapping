#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

if [ "$#" -lt 5 ] || [ "$#" -gt 6 ]; then
  echo "Usage: $0 <dataset_path> <param1> <param2> <depth_dir> <file_name_width> [model_path]"
  echo "Example: $0 /data/carla_scene1 1.5 2.0 depth_2 10 ./tmp_model.bin"
  exit 1
fi

dataset_path="$1"
param1="$2"
param2="$3"
depth_dir="$4"
file_name_width="$5"
model_path="${6:-./tmp_model.bin}"

if [ ! -d "$dataset_path" ]; then
  echo "Error: dataset path does not exist: $dataset_path"
  exit 1
fi

python3 "$SCRIPT_DIR/src/build_map.py" "$dataset_path" "$model_path" "$param1" "$param2" "$depth_dir" "$file_name_width"
python3 "$SCRIPT_DIR/src/load_map.py" "$dataset_path" "$model_path" "$param1" "$param2" "$depth_dir" "$file_name_width"
python3 "$SCRIPT_DIR/src/move_data.py" "$dataset_path" "$param1" "$param2"
