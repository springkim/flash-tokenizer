```bash
# MacOS

CC=gcc CXX=g++ CUSTOM_PYTHON=ON /usr/local/bin/python3 -m build
CC=gcc CXX=g++ CUSTOM_PYTHON=ON python -m build

python -m build 
python -m twine upload --repository-url https://upload.pypi.org/legacy/ dist/*

```