@echo off
setlocal enabledelayedexpansion

set "INSTALL_DIR=%cd%\miniconda"
curl https://repo.anaconda.com/miniconda/Miniconda3-latest-Windows-x86_64.exe -o miniconda.exe
start /wait "" miniconda.exe /S /D=%INSTALL_DIR%
del miniconda.exe
for %%v in (3.9 3.10 3.11 3.12 3.13) do (
    call "%INSTALL_DIR%\Scripts\conda.exe" create -n py%%v python=%%v -y
    call "%INSTALL_DIR%\envs\py%%v\python.exe" -m pip install build twine pybind11 numpy==1.26.4 setuptools wheel --no-warn-script-location
)
echo "Complete!"
pause