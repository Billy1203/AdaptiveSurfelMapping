#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

if [ "$#" -lt 3 ] || [ "$#" -gt 4 ]; then
  echo "Usage: $0 <dataset_path> <depth_dir> <file_name_width> [model_path]"
  echo "Example: $0 /data/carla_scene1 depth_2 10 ./tmp_model.bin"
  exit 1
fi

dataset_path="$1"
depth_dir="$2"
file_name_width="$3"
model_path="${4:-./tmp_model.bin}"

if [ ! -d "$dataset_path" ]; then
  echo "Error: dataset path does not exist: $dataset_path"
  exit 1
fi

for p1 in $(seq 5 45); do
  param1=$(awk -v v="$p1" 'BEGIN { printf "%.1f", v / 10.0 }')

  for p2 in $(seq 1 50); do
    param2=$(awk -v v="$p2" 'BEGIN { printf "%.1f", v / 10.0 }')
    python3 "$SCRIPT_DIR/src/build_map.py" "$dataset_path" "$model_path" "$param1" "$param2" "$depth_dir" "$file_name_width"
    python3 "$SCRIPT_DIR/src/load_map.py" "$dataset_path" "$model_path" "$param1" "$param2" "$depth_dir" "$file_name_width"
    python3 "$SCRIPT_DIR/src/move_data.py" "$dataset_path" "$param1" "$param2"
  done
done
