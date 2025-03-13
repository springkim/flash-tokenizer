#!/usr/bin/env zsh
export HOME=$(pwd)
mkdir -p miniconda
wget https://repo.anaconda.com/miniconda/Miniconda3-latest-MacOSX-arm64.sh -O /tmp/miniconda.sh
bash /tmp/miniconda.sh -b -u -p ./miniconda

python_versions=("3.9" "3.10" "3.11" "3.12")

for version in $python_versions; do
    short_version="py${version//./}"
    ./miniconda/bin/conda create -n $short_version python=$version -y
done

for version in $python_versions; do
    short_version="py${version//./}"
    ./miniconda/envs/$short_version/bin/python -m pip install build twine pybind11 numpy==1.26.4 setuptools wheel
done