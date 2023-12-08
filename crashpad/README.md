https://github.com/backtrace-labs/crashpad/blob/backtrace/doc/developing.md

```shell
git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git
vim ~/.bashrc
# .bashrc添加如下一行
export PATH="$PATH://home/zzx/depot_tools"
source ~/.bashrc

mkdir ~/crashpad
cd ~/crashpad
fetch crashpad
cd ~/crashpad/crashpad
git pull -r
gclient sync

cd ~/crashpad/crashpad
gn gen out/Default
ninja -C out/Default

git clone https://chromium.googlesource.com/crashpad/crashpad
https://github.com/TheAssemblyArmada/crashpad-cmake.git

git clone https://github.com/backtrace-labs/crashpad.git
git checkout backtrace
git submodule update --init --recursive

sudo apt-get install libcurl-ocaml-dev

# make a build directory (note: build is already used by Crashpad)
mkdir -p cbuild && cd cbuild
# run CMake (additional options like -DCMAKE_EXPORT_COMPILE_COMMANDS=TRUE are possible)
cmake ..
# alternatively: cmake --build .
make -j
```
