
```bash
# MacOS/Windows

conda create -n python38 python=3.8 -y &&\
conda create -n python39 python=3.9 -y &&\
conda create -n python310 python=3.10 -y &&\
conda create -n python311 python=3.11 -y &&\
conda create -n python312 python=3.12 -y &&\
conda create -n python313 python=3.13 -y

conda activate python38 && pip install build twine pybind11 numpy setuptools wheel --no-warn-script-location transformers &&\
conda activate python39 && pip install build twine pybind11 numpy setuptools wheel --no-warn-script-location transformers &&\
conda activate python310 && pip install build twine pybind11 numpy setuptools wheel --no-warn-script-location transformers &&\
conda activate python311 && pip install build twine pybind11 numpy setuptools wheel --no-warn-script-location transformers &&\
conda activate python312 && pip install build twine pybind11 numpy setuptools wheel --no-warn-script-location transformers &&\
conda activate python313 && pip install build twine pybind11 numpy setuptools wheel --no-warn-script-location transformers
 


conda activate python38 && python -m build
conda activate python39 && python -m build
conda activate python310 && python -m build
conda activate python311 && python -m build
conda activate python312 && python -m build
conda activate python313 && python -m build


conda activate python38 && CC=clang CXX=clang++ python -m build
conda activate python39 && CC=clang CXX=clang++ python -m build
conda activate python310 && CC=clang CXX=clang++ python -m build
conda activate python311 && CC=clang CXX=clang++ python -m build
conda activate python312 && CC=clang CXX=clang++ python -m build
conda activate python313 && CC=clang CXX=clang++ python -m build

python -m build
```


```bash




# MacOS

CC=gcc CXX=g++ CUSTOM_PYTHON=ON /usr/local/bin/python3 -m build
CC=gcc CXX=g++ CUSTOM_PYTHON=ON python -m build

python -m build 

cibuildwheel --platform linux

python -m twine upload --repository-url https://upload.pypi.org/legacy/ dist/*
```