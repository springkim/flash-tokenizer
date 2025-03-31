#!/bin/bash

# yum install -y clang llvm


versions=(cp313 cp312 cp311 cp310 cp39 cp38)
for version in "${versions[@]}"; do
  /opt/python/"${version}"-"${version}"/bin/pip install build scikit-build-core pybind11
done

cd /io

for version in "${versions[@]}"; do
  CC=gcc CXX=g++ /opt/python/"${version}"-"${version}"/bin/python -m build --wheel
done

mkdir wheelhouse
auditwheel repair dist/*.whl --wheel-dir /io/wheelhouse
rm -f dist/*linux*.whl
mv wheelhouse/*.whl dist/
rm -rf wheelhouse