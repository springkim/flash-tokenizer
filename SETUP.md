x## Python

Windows, MacOS는 아래 python 공식 홈페이지에서 설치한다.

Python 3.7 부터 3.12까지 지원한다.

* https://www.python.org/downloads/release/python-3129/
* https://www.python.org/downloads/release/python-3116/
* https://www.python.org/downloads/release/python-31011/

Windows는 `C:/Users/%USERNAME%/AppData/Local/Programs/Python/Python312/python.exe` 과 같은 경로에 설치한다.

MacOS는 `/usr/local/bin/python3` 과 같은 경로에 설치된다.

Linux의 경우는 아래처럼 설치하거나 직접 빌드해서 사용해도 괜찮다.

```
# Ubuntu
sudo apt install python3 python3-pip -y
# CentOS
sudo apt install python3 python3-pip -y
```

어떻게 설치를 하던 `/usr/bin/python3` 에 python이 있으면 상관없다.


```bash
brew install dosbox
```

##Windows
docker run -it -v "C:/Users/spring/Documents/git_NLPOptimize/flash-tokenize:/io" quay.io/pypa/manylinux2014_x86_64 bash
```
curl -L "https://www.python.org/ftp/python/3.12.1/Python-3.12.1.tgz" -o Python-3.12.1.tgz
tar -xzvf Python-3.12.1.tgz
cd Python-3.12.1
./configure
./configure --enable-optimizations
make
```

## manylinux

docker pull quay.io/pypa/manylinux2014_x86_64

docker run -it --name manylinux -v $(pwd):/io quay.io/pypa/manylinux2014_x86_64 /bin/bash
yum install python3 python3-pip wget git vim curl -y



docker run --rm -v $(pwd):/io quay.io/pypa/manylinux2014_x86_64 bash -c "\
cd /io && \
curl https://bootstrap.pypa.io/get-pip.py -o get-pip.py && \
python get-pip.py && \
python -m pip install setuptools wheel build pybind11 && \
python setup.py build && \
auditwheel repair dist/*.whl -w wheelhouse/"
