# File Audit and Cleanup Log

Date: 2026-04-14

## Audit scope

- Full review target: first-party files in repository root, `src/`, `gui/`, `rl_evaluation/`.
- Vendor scope (kept as-is except generated residue): `Pangolin/`, `pybind11/`, `rl_evaluation/PerceptualSimilarity-1.0/`.

## Decisions

### Kept (core source)

- `src/` and `gui/`: mapping engine, rendering, dataset reader, GUI.
- `build_map.cpp`, `load_map.cpp`: C++ entry points.
- `prepare_data.py`, `preparation.json`, `RotationTransform.py`: dataset preparation utilities.
- `rl_evaluation/src/*.py`: evaluation and image quality scripts.

### Reorganized (research assets)

- Moved demo media:
  - `loadmap.gif` -> `assets/demo/loadmap.gif`
- Moved notebooks:
  - `pre-processing.ipynb` -> `notebooks/pre_processing.ipynb`
  - `image_evaluation.ipynb` -> `notebooks/image_evaluation.ipynb`
  - `rl_evaluation/plot.ipynb` -> `notebooks/rl_evaluation_plot.ipynb`

### Removed (unnecessary/generated)

- `rl_evaluation/experiment_one_stepssssssssssssssss.sh` (duplicate/noise script).
- `build/` (compiled output directory).
- `Pangolin/examples/cmake-build-debug/` (generated CMake cache and IDE files).
- IDE metadata directories (`.idea/`, `rl_evaluation/.idea/`).
- Notebook checkpoint directories (`.ipynb_checkpoints/`, `rl_evaluation/.ipynb_checkpoints/`, `rl_evaluation/src/.ipynb_checkpoints/`).
- Python cache directories (`__pycache__/`, `rl_evaluation/src/__pycache__/`).
- Generated extension binaries in source folder (`rl_evaluation/src/*.so`).

## Configuration updates

- `.gitignore` updated to ignore compiled binaries, build outputs, IDE metadata, notebook checkpoints, and Python caches.
- `rl_evaluation/experiment_one_step.sh` and `rl_evaluation/experiment_batch.sh` updated with strict argument parsing and `python3` invocations.
- `README.md` rewritten in research-style format with consolidated download references, clearer parameter definition, and citation section.
- Added `docs/DOWNLOADS.md` and `docs/DATA_LAYOUT.md` to centralize external resource placement and dataset contract.

## Validation performed

- Python syntax check:
  - `prepare_data.py`, `move_data.py`, `RotationTransform.py`
  - `rl_evaluation/src/build_map.py`, `rl_evaluation/src/load_map.py`, `rl_evaluation/src/move_data.py`, `rl_evaluation/src/evaluate_images.py`, `rl_evaluation/src/img_sem_filter.py`
- Shell syntax check:
  - `rl_evaluation/experiment_one_step.sh`, `rl_evaluation/experiment_batch.sh`

No syntax errors were reported in these checks.
