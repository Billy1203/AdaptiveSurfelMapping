# External Downloads

This file is the single source of truth for external assets and where to place them in this repository.

## 1) Dataset and inputs

- CARLA / KITTI formatted dataset:
  - Place under a directory outside source control (recommended: `/data/...`).
  - Pass dataset root to `build_map` / `load_map` / experiment scripts.
- Demo dataset link from project notes:
  - https://drive.google.com/file/d/1uKM7Gbs_Hy99OwrfqNmNIAZQuydQ_Gdw/view?usp=sharing

## 2) Depth estimation references

- PSMNet:
  - https://github.com/JiaRenChang/PSMNet
  - Use it to generate depth directory (e.g. `psmnet`) under dataset root.

## 3) Semantic and post-processing references

- PointRend (Detectron2 project):
  - https://github.com/facebookresearch/detectron2/tree/main/projects/PointRend
- SPADE (optional post-processing):
  - https://github.com/NVlabs/SPADE

## 4) Evaluation dependencies

- LPIPS vendor code is already included in this repo:
  - `rl_evaluation/PerceptualSimilarity-1.0/`
- Additional Python deps:
  - Install via `requirements.txt` at repository root.

## 5) Placement checklist

- Do not commit raw datasets or generated outputs.
- Keep generated files under ignored folders:
  - `rl_evaluation/load_map_output/`
  - `rl_evaluation/paired/`
  - `rl_evaluation/novel_left/`
  - `rl_evaluation/novel_right/`
