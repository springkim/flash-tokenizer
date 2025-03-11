```bash
# MacOS
CC=gcc CXX=g++ /usr/local/bin/python3 -m build

python -m build 
python -m twine upload --repository-url https://upload.pypi.org/legacy/ dist/*
```