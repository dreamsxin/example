

```shell
git clone https://chromium.googlesource.com/linux-syscall-support
git clone https://github.com/google/breakpad.git
cd breakpad
mkdir src/third_party/lss/ -p
sudo apt-get install zlib1g-dev
cp ../linux-syscall-support/linux_syscall_support.h src/third_party/lss/
./configure
make -j4

./src/processor/minidump_stackwalk xxx.dmp /mnt/d/pdb/x64/xxx.pdb
# 将屏显输出到文件
script -f output.txt
exit
```



## windows VC运行库
```shell
git clone https://github.com/google/googletest.git
git clone https://chromium.googlesource.com/external/gyp
cd gyp
python setup.py install
./gyp.bat --no-circular-check "../breakpad/src/client/windows/breakpad_client.gyp"
./gyp.bat --no-circular-check "..\breakpad\src\tools\tools.gyp"
./gyp.bat --no-circular-check "..\breakpad\src\processor\processor.gyp"
```
创建testing 目录（breakpad/src/testing），并将goolgletest 目录下的两个文件拷贝进去

https://learn.microsoft.com/zh-cn/cpp/windows/latest-supported-vc-redist?view=msvc-170#visual-studio-2010-vc-100-sp1-no-longer-supported

### win10
`regsvr32 msdia120.dll`
```shell
.\dump_syms.exe .\test.exe > .\test.sym
.\dump_syms.exe .\test.pdb > .\test.sym
```
得到
`MODULE windows x86_64 530B0CABE8E149C2A831A8C676B4B0641 test.pdb`

新建目录 
`530B0CABE8E149C2A831A8C676B4B0641`

### VC2008运行库，版本号数字90，大致包含下面dll。

vcomp90.dll、msdia90.dll、ATL90.DLL

msvcm90.dll、msvcp90.dll、msvcr90.dll

mfc90.dll、mfc90u.dll、mfcm90.dll、mfcm90u.dll

mfc90CHS.dll（CHS、CHT、DEU、ENU、ESN、ESP、FRA、ITA、JPN、KOR、RUS）

跟上面的vc2005差不多。

### VC2010运行库，版本号数字100，大致包含下面dll。

vcomp100.dll、msdia100.dll、ATL100.DLL

msvcp100.dll、msvcr100.dll

mfc100.dll、mfc100u.dll、mfcm100.dll、mfcm100u.dll

mfc100CHS.dll（CHS、CHT、DEU、ENU、ESN、FRA、ITA、JPN、KOR、RUS）

除了msdia100.dll跟上面的一样，其余dll均在system32或syswow64目录。

### VC2012运行库，版本号数字110，大致包含下面dll。

vcomp110.dll、vcamp110.dll、vccorlib110.dll、ATL110.DLL

msvcp110.dll、msvcr110.dll

mfc110.dll、mfc110u.dll、mfcm110.dll、mfcm110u.dll

mfc110CHS.dll（CHS、CHT、DEU、ENU、ESN、FRA、ITA、JPN、KOR、RUS）

所有dll都在system32或syswow64目录。

### VC2013运行库，版本号数字120，大致包含下面dll。

vcomp120.dll、vcamp120.dll、vccorlib120.dll

msvcp120.dll、msvcr120.dll

mfc120.dll、mfc120u.dll、mfcm120.dll、mfcm120u.dll

mfc120CHS.dll（CHS、CHT、DEU、ENU、ESN、FRA、ITA、JPN、KOR、RUS）

除了少了ATLxxx.dll，其余跟VC2012差不多。

### VC2015-2022运行库（包含2015、2017、2019、2022），版本号数字140，大致包含下面dll。

concrt140.dll、vcomp140.dll、vcamp140.dll、vccorlib140.dll、vcruntime140.dll、vcruntime140_1.dll

msvcp140.dll、msvcp140_1.dll、msvcp140_2.dll、msvcp140_atomic_wait.dll、msvcp140_codecvt_ids.dll

mfc140.dll、mfc140u.dll、mfcm140.dll、mfcm140u.dl

mfc140CHS.dll（CHS、CHT、DEU、ENU、ESN、FRA、ITA、JPN、KOR、RUS）

dll文件都在system32或syswow64目录。msvcp140_1.dll、msvcp140_2.dll在VC2017后出现；vcruntime140_1.dll、msvcp140_atomic_wait.dll（VC2019某版本后出现）、msvcp140_codecvt_ids.dll在VC2019后出现。xp系统最高能安装vc2015-2019。

VC2015-2022中高版本兼容低版本（即装了vc2015-2022就无需再安装2015、2017、2019这些）。除了极个别软件安装包认死理（如Edius 9报错0x80070666）在装了高版本的情况下就是装不上，除非卸载所有2015、2017、2019、2022后才能安装。

VC2015-2022运行库在win7、win8等版本上安装时，会额外安装kb2999226补丁。这个补丁包含ucrtbase.dll、api-ms-win-xxx.dll系列dll，API sets DLL（API集DLL）。 作者：鸟人飞飞 https://www.bilibili.com/read/cv25257475/ 出处：bilibili
