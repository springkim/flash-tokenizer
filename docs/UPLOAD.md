
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
 

 

conda activate python38 && python -m build --wheel
conda activate python39 && python -m build --wheel
conda activate python310 && python -m build --wheel
conda activate python311 && python -m build --wheel
conda activate python312 && python -m build  --wheel
conda activate python313 && python -m build

```

```bash
# Windows docker
docker pull quay.io/pypa/manylinux_2_28
docker run -it -v %cd%:/io quay.io/pypa/manylinux_2_28 /bin/bash

/opt/python/cp313-cp313/bin/pip install build scikit-build-core pybind11 && \
/opt/python/cp312-cp312/bin/pip install build scikit-build-core pybind11 && \
/opt/python/cp311-cp311/bin/pip install build scikit-build-core pybind11 && \
/opt/python/cp310-cp310/bin/pip install build scikit-build-core pybind11 && \
/opt/python/cp39-cp39/bin/pip install build scikit-build-core pybind11 && \
/opt/python/cp38-cp38/bin/pip install build scikit-build-core pybind11

cd /io
CC=gcc CXX=g++ /opt/python/cp313-cp313/bin/python -m build --wheel
CC=gcc CXX=g++ /opt/python/cp312-cp312/bin/python -m build --wheel
CC=gcc CXX=g++ /opt/python/cp311-cp311/bin/python -m build --wheel
CC=gcc CXX=g++ /opt/python/cp310-cp310/bin/python -m build --wheel
CC=gcc CXX=g++ /opt/python/cp39-cp39/bin/python -m build --wheel
CC=gcc CXX=g++ /opt/python/cp38-cp38/bin/python -m build --wheel

mkdir wheelhouse && \
auditwheel repair dist/*.whl --wheel-dir /io/wheelhouse && \
rm -f dist/*linux*.whl && \
mv wheelhouse/*.whl dist/ && \
rm -rf wheelhouse
```

```bash
CC=gcc CXX=g++ CUSTOM_PYTHON=ON python -m pip install .
conda install -c conda-forge libstdcxx-ng
```


## Upload
```bash
python -m twine upload --repository-url https://upload.pypi.org/legacy/ dist/*
```