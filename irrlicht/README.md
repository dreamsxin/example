# 安装编译

```shell
svn checkout svn://svn.code.sf.net/p/irrlicht/code/trunk irrlicht

sudo apt-get -y install build-essential xserver-xorg-dev x11proto-xf86vidmode-dev libxxf86vm-dev mesa-common-dev libgl1-mesa-dev libglu1-mesa-dev libxext-dev libxcursor-dev
cd irrlicht/source/Irrlicht && make

cd ../../
sudo cp -r include/* /usr/local/include/
sudo cp lib/Linux/libIrrlicht.a /usr/local/lib
```

# 编译例子

```shell
cd examples
chmod +x buildAllExamples.sh
./buildAllExamples.sh
cd ../bin/Linux 
```