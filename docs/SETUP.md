# Setup

### This document is a summary of the information required to build C++ instead of Python.


# 01. 7z

### Windows

Download the `7-Zip installer for Windows` from [https://www.7-zip.org/download.html](https://www.7-zip.org/download.html) and run it to copy the following files to `C:\Windows\System32\`.

* Lang
* 7z.dll
* 7z.exe
* 7-zip.dll
* 7-zip32.dll

### Ubuntu
sudo apt install p7zip-full -y
### CentOS
sudo yum install p7zip p7zip-plugins -y
### MacOS
brew install p7zip

# 02. Compiler

### MacOS(clang)

```bash
brew install llvm libomp

# Add the following to ~/.zshrc
export PATH="$(brew --prefix llvm)/bin:$PATH"
export LDFLAGS="-L$(brew --prefix llvm)/lib"
export CPPFLAGS="-I$(brew --prefix llvm)/include"
export CC="$(brew --prefix llvm)/bin/clang"
export CXX="$(brew --prefix llvm)/bin/clang++"
```

### Windows(MSVC 2022)

Install Visual Studio 2022 from https://visualstudio.microsoft.com/ko/vs/community/.

### Ubuntu(g++)

```bash
sudo apt install gcc g++ make cmake -y
```

# 03. IDE

[CLion](https://www.jetbrains.com/clion/)

# 04. C++

* Compatible with C++17.
* Supports building on Linux, Windows, and macOS.
* Buildable with G++, Clang++, and MSVC compilers.