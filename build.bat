@echo off

conda create -n python39 python=3.9 -y
conda create -n python310 python=3.10 -y
conda create -n python311 python=3.11 -y
conda create -n python312 python=3.12 -y


conda activate python39
conda activate python310
conda activate python311
conda activate python312
pip install build twine pybind11 numpy==1.26.4 setuptools wheel --no-warn-script-location
python -m build




echo "Complete!!"
pause
