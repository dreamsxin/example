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
sudo update-alternatives --config editor
```

**rust**
```shell
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
source ~/.cargo/env
cargo install cbindgen

# 如果上面安装出错错误，请检查 curl 是否使用 snap 安装的版本
sudo snap list |grep curl
sudo snap remove curl
```

```shell
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
 wget https://apt.llvm.org/llvm.sh
chmod +x llvm.sh
sudo ./llvm.sh 18 all

sudo bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)"

# 设置环境变量~/.bashrc 添加 export PATH=/usr/lib/llvm-18/bin/:$PATH
source ~/.bashrc
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

## llvm-update-alternatives
```shell
#!/usr/bin/env sh

sudo update-alternatives --install \
        /usr/bin/llvm-config       llvm-config      /usr/bin/llvm-config-3.4  200 \
--slave /usr/bin/llvm-ar           llvm-ar          /usr/bin/llvm-ar-3.4 \
--slave /usr/bin/llvm-as           llvm-as          /usr/bin/llvm-as-3.4 \
--slave /usr/bin/llvm-bcanalyzer   llvm-bcanalyzer  /usr/bin/llvm-bcanalyzer-3.4 \
--slave /usr/bin/llvm-cov          llvm-cov         /usr/bin/llvm-cov-3.4 \
--slave /usr/bin/llvm-diff         llvm-diff        /usr/bin/llvm-diff-3.4 \
--slave /usr/bin/llvm-dis          llvm-dis         /usr/bin/llvm-dis-3.4 \
--slave /usr/bin/llvm-dwarfdump    llvm-dwarfdump   /usr/bin/llvm-dwarfdump-3.4 \
--slave /usr/bin/llvm-extract      llvm-extract     /usr/bin/llvm-extract-3.4 \
--slave /usr/bin/llvm-link         llvm-link        /usr/bin/llvm-link-3.4 \
--slave /usr/bin/llvm-mc           llvm-mc          /usr/bin/llvm-mc-3.4 \
--slave /usr/bin/llvm-mcmarkup     llvm-mcmarkup    /usr/bin/llvm-mcmarkup-3.4 \
--slave /usr/bin/llvm-nm           llvm-nm          /usr/bin/llvm-nm-3.4 \
--slave /usr/bin/llvm-objdump      llvm-objdump     /usr/bin/llvm-objdump-3.4 \
--slave /usr/bin/llvm-ranlib       llvm-ranlib      /usr/bin/llvm-ranlib-3.4 \
--slave /usr/bin/llvm-readobj      llvm-readobj     /usr/bin/llvm-readobj-3.4 \
--slave /usr/bin/llvm-rtdyld       llvm-rtdyld      /usr/bin/llvm-rtdyld-3.4 \
--slave /usr/bin/llvm-size         llvm-size        /usr/bin/llvm-size-3.4 \
--slave /usr/bin/llvm-stress       llvm-stress      /usr/bin/llvm-stress-3.4 \
--slave /usr/bin/llvm-symbolizer   llvm-symbolizer  /usr/bin/llvm-symbolizer-3.4 \
--slave /usr/bin/llvm-tblgen       llvm-tblgen      /usr/bin/llvm-tblgen-3.4 \


sudo update-alternatives --install \
        /usr/bin/llvm-config       llvm-config      /usr/bin/llvm-config-3.5  200 \
--slave /usr/bin/llvm-ar           llvm-ar          /usr/bin/llvm-ar-3.5 \
--slave /usr/bin/llvm-as           llvm-as          /usr/bin/llvm-as-3.5 \
--slave /usr/bin/llvm-bcanalyzer   llvm-bcanalyzer  /usr/bin/llvm-bcanalyzer-3.5 \
--slave /usr/bin/llvm-cov          llvm-cov         /usr/bin/llvm-cov-3.5 \
--slave /usr/bin/llvm-diff         llvm-diff        /usr/bin/llvm-diff-3.5 \
--slave /usr/bin/llvm-dis          llvm-dis         /usr/bin/llvm-dis-3.5 \
--slave /usr/bin/llvm-dwarfdump    llvm-dwarfdump   /usr/bin/llvm-dwarfdump-3.5 \
--slave /usr/bin/llvm-extract      llvm-extract     /usr/bin/llvm-extract-3.5 \
--slave /usr/bin/llvm-link         llvm-link        /usr/bin/llvm-link-3.5 \
--slave /usr/bin/llvm-mc           llvm-mc          /usr/bin/llvm-mc-3.5 \
--slave /usr/bin/llvm-mcmarkup     llvm-mcmarkup    /usr/bin/llvm-mcmarkup-3.5 \
--slave /usr/bin/llvm-nm           llvm-nm          /usr/bin/llvm-nm-3.5 \
--slave /usr/bin/llvm-objdump      llvm-objdump     /usr/bin/llvm-objdump-3.5 \
--slave /usr/bin/llvm-ranlib       llvm-ranlib      /usr/bin/llvm-ranlib-3.5 \
--slave /usr/bin/llvm-readobj      llvm-readobj     /usr/bin/llvm-readobj-3.5 \
--slave /usr/bin/llvm-rtdyld       llvm-rtdyld      /usr/bin/llvm-rtdyld-3.5 \
--slave /usr/bin/llvm-size         llvm-size        /usr/bin/llvm-size-3.5 \
--slave /usr/bin/llvm-stress       llvm-stress      /usr/bin/llvm-stress-3.5 \
--slave /usr/bin/llvm-symbolizer   llvm-symbolizer  /usr/bin/llvm-symbolizer-3.5 \
--slave /usr/bin/llvm-tblgen       llvm-tblgen      /usr/bin/llvm-tblgen-3.5 \
```


## 编译

```shell
./mach configure --without-wasm-sandboxed-libraries --disable-tests
```

## 错误

`error: could not exec the linker `/usr/local/src/firefox130/build/cargo-linker`

```shell
sudo chmod a+x /usr/local/src/firefox130/build/cargo-linker
```
