@echo off


.\package_build\miniconda_win\envs\py3.9\python.exe -m build
.\package_build\miniconda_win\envs\py3.10\python.exe -m build
.\package_build\miniconda_win\envs\py3.11\python.exe -m build
.\package_build\miniconda_win\envs\py3.12\python.exe -m build

echo "Complete!!"
pause
