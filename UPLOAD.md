```bash
python -m pip install build twine pybind11 numpy
python -m pip install --upgrade setuptools wheel build


python setup.py bdist_wheel

python -m build 
python -m twine upload --repository-url https://upload.pypi.org/legacy/ dist/*
```