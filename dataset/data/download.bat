@echo off

echo Deleting existing data files...
del /Q data.*

echo Starting dataset download...

start /b curl -sL https://github.com/NLPOptimize/flash-tokenizer/releases/download/Dataset/data.z01 -o data.z01
start /b curl -sL https://github.com/NLPOptimize/flash-tokenizer/releases/download/Dataset/data.z02 -o data.z02
start /b curl -sL https://github.com/NLPOptimize/flash-tokenizer/releases/download/Dataset/data.z03 -o data.z03
start /b curl -sL https://github.com/NLPOptimize/flash-tokenizer/releases/download/Dataset/data.z04 -o data.z04
start /b curl -sL https://github.com/NLPOptimize/flash-tokenizer/releases/download/Dataset/data.zip -o data.zip

:waitloop
timeout /t 2 >nul
tasklist /FI "IMAGENAME eq curl.exe" 2>NUL | find /I "curl.exe">NUL
if "%ERRORLEVEL%"=="0" goto waitloop

echo Extracting the compressed file...

del /Q texts_*.parquet

7z x data.zip

del /Q data.*

echo Done.
