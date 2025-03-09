```bash
pip install build twine
python -m build
python -m twine upload --repository-url https://upload.pypi.org/legacy/ dist/*
```