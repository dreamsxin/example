

查看文件类型

```shell
file libxxx.so
```

* objdump

```shell
nm -D libxxx.so
objdump -tT libxxx.so
objdump -s -j .rodata -d libxxx.so
``

* arm-linux-androideabi-objdumop

```shell
sudo apt-get install gcc-arm-linux-androideabi
arm-linux-androideabi-objdump -d libvivosgmain.so
```

* gcc-linaro-aarch64-linux-gnu

https://releases.linaro.org/components/toolchain/binaries/latest-5/aarch64-linux-gnu/

```shell
sudo add-apt-repository ppa:linaro-maintainers/toolchain
```

```shell
sudo apt-get install gcc-aarch64-linux-gnu
aarch64-linux-gnu-objdump -d libxxx.so
```