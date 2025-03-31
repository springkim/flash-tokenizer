#!/bin/bash

if ! conda env list | grep -q "python38" || ! conda env list | grep -q "python39" || ! conda env list | grep -q "python310" || ! conda env list | grep -q "python311" || ! conda env list | grep -q "python312" || ! conda env list | grep -q "python313"; then
  conda create -n python38 python=3.8 -y
  conda create -n python39 python=3.9 -y
  conda create -n python310 python=3.10 -y
  conda create -n python311 python=3.11 -y
  conda create -n python312 python=3.12 -y
  conda create -n python313 python=3.13 -y

  versions=(cp313 cp312 cp311 cp310 cp39 cp38)
  for version in "${versions[@]}"; do
    /opt/python/"${version}"-"${version}"/bin/pip install build scikit-build-core pybind11
  done
fi

cd /io
versions=(cp313 cp312 cp311 cp310 cp39 cp38)
for version in "${versions[@]}"; do
  CC=gcc CXX=g++ /opt/python/"${version}"-"${version}"/bin/python -m build --wheel
done

mkdir wheelhouse
auditwheel repair dist/*.whl --wheel-dir /io/wheelhouse
rm -f dist/*linux*.whl
mv wheelhouse/*.whl dist/
rm -rf wheelhouse