- https://github.com/LadybirdBrowser/ladybird/blob/master/Documentation/BuildInstructionsLadybird.md

## 依赖

```shell
sudo apt update && sudo apt install -y \
  autoconf autoconf-archive automake build-essential ccache \
  curl git libdrm-dev libgl1-mesa-dev libtool nasm ninja-build \
  pkg-config python3-venv qt6-base-dev qt6-tools-dev-tools \
  qt6-wayland tar unzip zip fonts-liberation2
```

```shell
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
```

## 下载源码

```shell
git clone https://github.com/LadybirdBrowser/ladybird.git
cd ladybird
```

## 编译运行

```shell
./Meta/ladybird.py run
```
