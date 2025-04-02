# 环境配置

### 本文档总结了构建 C++（而非 Python）程序所需的相关信息。

---

# 01. 7z 安装

### Windows

从[7-Zip官网](https://www.7-zip.org/download.html)下载`7-Zip installer for Windows`，运行安装程序，并将以下文件复制到目录 `C:\Windows\System32\`：

* Lang
* 7z.dll
* 7z.exe
* 7-zip.dll
* 7-zip32.dll

### Ubuntu

```bash
sudo apt install p7zip-full -y
```

### CentOS

```bash
sudo yum install p7zip p7zip-plugins -y
```

### MacOS

```bash
brew install p7zip
```

---

# 02. 编译器安装

### MacOS (Clang)

```bash
brew install llvm libomp

# 在 ~/.zshrc 中添加以下内容：
export PATH="$(brew --prefix llvm)/bin:$PATH"
export LDFLAGS="-L$(brew --prefix llvm)/lib"
export CPPFLAGS="-I$(brew --prefix llvm)/include"
export CC="$(brew --prefix llvm)/bin/clang"
export CXX="$(brew --prefix llvm)/bin/clang++"
```

### Windows (MSVC 2022)

从以下链接安装 Visual Studio 2022：
[https://visualstudio.microsoft.com/ko/vs/community/](https://visualstudio.microsoft.com/ko/vs/community/)

### Ubuntu (G++)

```bash
sudo apt install gcc g++ make cmake -y
```

---

# 03. 推荐 IDE

推荐使用 [CLion](https://www.jetbrains.com/clion/)。

---

# 04. C++ 支持

* 兼容 C++17 标准。
* 支持 Linux、Windows 和 macOS 平台构建。
* 支持使用 G++、Clang++ 和 MSVC 编译器构建。