# Dataset Layout Contract

This is the expected dataset layout consumed by `gui/KittiReader.cpp` and experiment scripts.

```text
<dataset_root>/
  calibration.txt
  pose.txt
  image_2/                    # main RGB sequence
  image_3/                    # stereo partner (optional for some workflows)
  depth_2/ or psmnet/         # depth directory selected by runtime argument
  image_val_0/                # novel-view GT (left)
  image_val_1/                # novel-view GT (right)
  image_2_semantic/           # optional semantic map
  image_3_semantic/           # optional semantic map
  image_val_0_semantic/       # optional semantic map
  image_val_1_semantic/       # optional semantic map
```

## Required runtime arguments

- `dataset_root`: absolute or relative path to dataset root.
- `depth_dir`: folder name under dataset root (example: `depth_2`, `psmnet`).
- `file_name_width`: zero-padding width of frame names (`6` or `10`).

## Naming rule

- Frame files must use zero-padded numeric names:
  - width `6`: `000001.png`
  - width `10`: `0000000001.png`

## Filtered GT folders (for CARLA evaluation)

`rl_evaluation/src/move_data.py` expects:

- `image_2_filtered/`
- `image_val_0_filtered/`
- `image_val_1_filtered/`

You can generate these with:

```bash
python3 rl_evaluation/src/img_sem_filter.py <dataset_root>
```
