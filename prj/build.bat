@echo off
setlocal enabledelayedexpansion

for %%v in (38 39 310 311 312 313) do (
    conda env list | findstr "python%%v" > nul
    if errorlevel 1 (
        echo Creating python%%v environment...
        conda create -n python%%v python=3.%%v -y
    )
)

set versions=cp38 cp39 cp310 cp311 cp312 cp313

for %%v in (%versions%) do (
    conda activate python%%v && pip install build twine pybind11 numpy setuptools wheel
)

for %%v in (%versions%) do (
    conda activate python%%v && python -m build --wheel
)

echo Build process complete.
endlocal