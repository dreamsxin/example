https://www.browserbase.com/blog/recommending-playwright

## 配置环境
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
