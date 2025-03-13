#!/usr/bin/bash
export HOME=$(pwd)
mkdir miniconda
wget https://repo.anaconda.com/miniconda/Miniconda3-latest-MacOSX-arm64.sh -O /tmp/miniconda.sh
bash /tmp/miniconda.sh -b -u -p ./miniconda

./miniconda/bin/conda create -n py39 python=3.9 -y
./miniconda/bin/conda create -n py310 python=3.10 -y
./miniconda/bin/conda create -n py311 python=3.11 -y
./miniconda/bin/conda create -n py312 python=3.12 -y
./miniconda/bin/conda create -n py313 python=3.13 -y

./miniconda/envs/py39/bin/python -m pip install build twine pybind11 numpy==1.26.4 setuptools wheel
./miniconda/envs/py310/bin/python -m pip install build twine pybind11 numpy==1.26.4 setuptools wheel
./miniconda/envs/py311/bin/python -m pip install build twine pybind11 numpy==1.26.4 setuptools wheel
./miniconda/envs/py312/bin/python -m pip install build twine pybind11 numpy==1.26.4 setuptools wheel
./miniconda/envs/py313/bin/python -m pip install build twine pybind11 numpy==1.26.4 setuptools wheel


