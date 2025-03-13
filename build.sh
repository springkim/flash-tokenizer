#!/bin/bash

rm -rf dist

CC=gcc CXX=g++ CUSTOM_PYTHON=ON ./package_build/miniconda/envs/py39/bin/python -m build
CC=gcc CXX=g++ CUSTOM_PYTHON=ON ./package_build/miniconda/envs/py310/bin/python -m build
CC=gcc CXX=g++ CUSTOM_PYTHON=ON ./package_build/miniconda/envs/py311/bin/python -m build
CC=gcc CXX=g++ CUSTOM_PYTHON=ON ./package_build/miniconda/envs/py312/bin/python -m build
