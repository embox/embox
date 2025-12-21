## Install required packages
#### Debian/Ubuntu:
```shell
sudo apt-get install \
  build-essential automake flex bison mercurial \
  libgtk-3-dev libgl1-mesa-dev libglu1-mesa-dev \
  libssl-dev cmake git mercurial wget
```

## Install python3.9
#### Install using package manager (Debian/Ubuntu):
```shell
sudo apt-get install python3.9 python3.9-venv python3.9-dev
```
#### Install from source:
```shell
wget https://www.python.org/ftp/python/3.9.20/Python-3.9.20.tgz
```
```shell
tar xvf Python-3.9.20.tgz
```
```shell
cd Python-3.9.20
```
```shell
./configure --enable-optimizations
```
```shell
sudo make altinstall
```

## Install Beremiz
```shell
git clone git@github.com:embox/beremiz.git
```
```shell
cd beremiz
```
```shell
python3.9 -m venv ./.venv
```
```shell
./.venv/bin/pip install -r ./requirements.txt
```

## Run Beremiz
```shell
./.venv/bin/python ./Beremiz.py
```
```
./scripts/beremiz.sh ~/Downloads/beremiz/
```

sudo ../src/openocd  -f board/vostok_vg015_dev.cfg

GPIOA выводы 12,13,14,15
pin GPIOA 12 set