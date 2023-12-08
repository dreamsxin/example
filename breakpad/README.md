

```shell
git clone https://chromium.googlesource.com/linux-syscall-support
git clone https://github.com/google/breakpad.git
cd breakpad
mkdir src/third_party/lss/ -p
sudo apt-get install zlib1g-dev
cp ../linux-syscall-support/linux_syscall_support.h src/third_party/lss/
./configure
make -j4
```
