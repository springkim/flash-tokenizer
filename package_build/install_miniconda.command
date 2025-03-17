#!/usr/bin/env zsh
export HOME=$(pwd)
mkdir -p miniconda_mac
wget https://repo.anaconda.com/miniconda/Miniconda3-py312_24.1.2-0-MacOSX-arm64.sh -O /tmp/miniconda.sh
bash /tmp/miniconda.sh -b -u -p ./miniconda_mac

python_versions=("3.9" "3.10" "3.11" "3.12")

for version in ${python_versions}; do
    short_version="py${version//./}"
    ./miniconda_mac/bin/conda create -n $short_version python=$version -y
done

for version in ${python_versions}; do
    short_version="py${version//./}"
    ./miniconda_mac/envs/$short_version/bin/python -m pip install build twine pybind11 numpy==1.26.4 setuptools wheel
done