https://www.browserbase.com/blog/recommending-playwright

## 配置环境

```shell
sudo apt install git
git config core.fileMode false

sudo apt update
sudo apt install -y build-essential mercurial zip python3 python3-pip cmake
sudo apt install -y clang llvm
sudo apt install -y libcurl4-openssl-dev libxt-dev libasound2-dev libgtk2.0-dev
sudo apt install -y libdbus-1-dev libdbus-glib-1-dev libglib2.0-dev
sudo apt install -y libxt6 libxss1 libx11-xcb-dev libxcomposite-dev
sudo apt install -y libasound2-dev libpulse-dev libxt-dev libxtst-dev
sudo apt install -y libpci-dev libxrandr-dev libdbus-1-3
```

```shell
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
# 如果上面安装出错错误，请检查 curl 是否使用 snap 安装的版本
sudo snap list |grep curl
sudo snap remove curl

# https://launchpad.net/~ubuntu-mozilla-security/+archive/ubuntu/rust-next
sudo add-apt-repository ppa:ubuntu-mozilla-security/rust-next
sudo apt-get update
sudo apt-get install rustc cargo
sudo add-apt-repository --remove ppa:ubuntu-mozilla-security/rust-next
```

```shell
sudo update-alternatives --config editor
```

**rust**
```shell
sudo add-apt-repository ppa:jonathonf/rust-nightly
sudo apt update
sudo apt install rustc cargo

curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
rustup install 1.84.1
rustup default 1.84.1
rustup default nightly  # 将默认工具链设置为 nightly
rustup update stable  # 更新到最新稳定版
rustup update beta    # 更新到最新 beta 版
rustup update nightly # 更新到最新 nightly 版
rustup self uninstall
```

**nodejs**
```shell
curl -sL https://deb.nodesource.com/setup_14.x | sudo -E bash -
sudo apt-get install -y nodejs
```

**llvm**
```shell
sudo bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)"
sudo apt-get install llvm -y
sudo apt-get install clang libclang-dev -y
sudo apt-get install gcc-multilib -y
```

```shell
sudo apt-get install build-essential
sudo apt-get install autoconf automake
sudo apt update && sudo apt install curl python3 python3-pip


sudo apt-get install libalsa-ocaml-dev
sudo apt-get install libpulse-ocaml-dev
cargo install --force cbindgen
sudo apt install libpango1.0-dev -y
sudo apt install libxi-dev
sudo apt install libx11-xcb-dev libxrandr-dev libxcomposite-dev libxcursor-dev libxdamage-dev
sudo apt-get install nasm
```
