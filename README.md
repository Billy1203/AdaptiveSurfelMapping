# Adaptive Surfel Mapping

<p align="left">
  <img src="https://img.shields.io/badge/C%2B%2B-11-blue" alt="C++11"/>
  <img src="https://img.shields.io/badge/CMake-%3E%3D3.16-0f4c81" alt="CMake"/>
  <img src="https://img.shields.io/badge/Python-3.10-3776ab" alt="Python"/>
  <img src="https://img.shields.io/badge/Status-Research%20Code-8a2be2" alt="Status"/>
</p>

Official code release for the IROS 2025 paper  
**Adaptive Large-Scale Novel View Image Synthesis for Autonomous Driving Datasets**.

Adaptive surfel-based mapping and novel-view rendering for autonomous driving scenes (CARLA / KITTI). The repository contains C++ mapping core, Python experiment wrappers, and evaluation scripts.

![Load map demo](assets/demo/loadmap.gif)

## 📌 Project Scope

- Build a global surfel map from RGB-D (or pseudo-depth) sequences.
- Render paired and novel views from generated maps.
- Evaluate image quality with PSNR / SSIM / LPIPS for parameter search.
- Keep reproducible experiment scripts under `rl_evaluation/`.

## 🗂 Repository Layout

```text
AdaptiveSurfelMapping/
  assets/
    demo/                         # README demo media
  docs/
    DATA_LAYOUT.md                # dataset contract
    DOWNLOADS.md                  # all external links and placement
    FILE_AUDIT.md                 # cleanup log
  notebooks/
    pre_processing.ipynb
    image_evaluation.ipynb
    rl_evaluation_plot.ipynb
  src/                            # surfel mapping core + shaders
  gui/                            # dataset reader + GUI
  rl_evaluation/
    experiment_one_step.sh
    experiment_batch.sh
    src/                          # python wrappers + evaluation
    PerceptualSimilarity-1.0/     # LPIPS vendor code
  build_map.cpp                   # C++ map build entry
  load_map.cpp                    # C++ map load/render entry
  prepare_data.py                 # dataset conversion utility
  preparation.json                # conversion config
  move_data.py                    # generated image organizer
  CMakeLists.txt
  requirements.txt
```

## ⚙️ Environment

### C++ dependencies

- Ubuntu 18.04/20.04 (historically tested)
- CMake >= 3.16
- OpenGL
- Eigen
- OpenCV
- Pangolin (vendored in this repo)

### Python dependencies

```bash
python3 -m pip install -r requirements.txt
```

- Python 3.10

Security note:
- `pytest` has been constrained to `>=9.0.3` for CVE-2025-71176 mitigation.

## 🚀 Build and Run

```bash
mkdir -p build
cd build
cmake ..
make -j"$(nproc)"
```

Run `build_map`:

```bash
./build_map <dataset_root> <model_path.bin> <diff_thresh> <r0> <depth_dir> <file_name_width>
```

Run `load_map`:

```bash
./load_map <dataset_root> <model_path.bin> <diff_thresh> <r0> <depth_dir> <file_name_width>
```

## 🧪 Experiment Pipeline

1. Prepare dataset (format conversion and metadata export):
   - edit `preparation.json`
   - run `python3 prepare_data.py`
2. Build map:
   - `rl_evaluation/src/build_map.py` (or C++ binary directly)
3. Load map and render paired/novel views:
   - `rl_evaluation/src/load_map.py`
4. Collect outputs and evaluate:
   - `rl_evaluation/src/move_data.py`
   - `rl_evaluation/src/evaluate_images.py`

Single run:

```bash
cd rl_evaluation
./experiment_one_step.sh <dataset_root> <diff_thresh> <r0> <depth_dir> <file_name_width> [model_path]
```

Batch sweep:

```bash
cd rl_evaluation
./experiment_batch.sh <dataset_root> <depth_dir> <file_name_width> [model_path]
```

## 🧭 Parameter Definition

- `diff_thresh`:
  depth fusion threshold used during map construction (`build_map` stage).
- `r0`:
  surfel rendering scale control used during map rendering (`load_map` stage).
- `depth_dir`:
  depth source directory name under dataset root (e.g. `depth_2`, `psmnet`).
- `file_name_width`:
  zero-padding width of frame names (typically `6` or `10`).

## 📁 Downloads and Data Placement

All external links and recommended placement are maintained in [docs/DOWNLOADS.md](docs/DOWNLOADS.md).  
Dataset directory contract is documented in [docs/DATA_LAYOUT.md](docs/DATA_LAYOUT.md).

## ❗ Troubleshooting

- `file_name_width` mismatch (e.g. `6` vs `10`) leads to immediate file-not-found errors.
- `depth_dir` mismatch (e.g. `depth_2` vs `psmnet`) causes missing depth frames.
- If Python wrappers cannot import `build_map` or `load_map`, rebuild the C++ modules and verify working directory / `PYTHONPATH`.
- For CARLA evaluation, ensure filtered GT folders exist (`image_2_filtered`, `image_val_0_filtered`, `image_val_1_filtered`) before metric scripts.

## 📚 Citation

Paper: [IEEE Xplore](https://ieeexplore.ieee.org/abstract/document/11246703)

```bibtex
@INPROCEEDINGS{11246703,
  author={Xue, Yiheng and Lyu, Zhijun and Ma, Rui and Xie, Yuezhen and Hao, Qi},
  booktitle={2025 IEEE/RSJ International Conference on Intelligent Robots and Systems (IROS)},
  title={Adaptive Large-Scale Novel View Image Synthesis for Autonomous Driving Datasets},
  year={2025},
  pages={13705-13712},
  keywords={Image quality;Geometry;Adaptation models;Solid modeling;Three-dimensional displays;Translation;Computational modeling;Source coding;Rendering (computer graphics);Tuning;mapping;RGB-D perception;data sets for robotic vision},
  doi={10.1109/IROS60139.2025.11246703}
}
```

## 🙏 Acknowledgement

This project is inspired by ElasticFusion:
https://www.imperial.ac.uk/dyson-robotics-lab/downloads/elastic-fusion/
