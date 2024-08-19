# Android 开发

https://developer.android.com/studio/install.html?hl=zh-cn

## 创建运行模拟器

```shell
./Android/Sdk/tools/bin/sdkmanager  --list
./Android/Sdk/tools/bin/sdkmanager 'system-images;android-27;google_apis;x86'
./Android/Sdk/tools/bin/avdmanager create avd --force --name phalcon7 -k 'system-images;android-27;google_apis;x86' -p ~/Android/
./Android/Sdk/tools/emulator -list-avds
./Android/Sdk/tools/emulator -avd phalcon7
```

## JDK配置

下载地址：http://www.oracle.com/technetwork/java/javase/downloads/jdk8-downloads-2133151.html

卸载OpenJDK

```shell
sudo apt-get purge openjdk/openjdk*
sudo apt-get clean/autoclean
```

解压 JDK
```shell
tar –zxvf  jdk-8u131-linux-xxx.tar.gz
sudo mv ./jdk1.8.0_131 /opt/Java
```

配置环境变量 `/etc/environment`
```shell
export JAVA_HOME=/opt/Java
export CLASSPATH=.:$JAVA_HOME/lib:$JAVA_HOME/jre/lib
# 使环境变量立即生效
source /etc/environment
```

配置所有用户的环境变量`/etc/profile`
```shell
export JAVA_HOME=/opt/Java
export JRE_HOME=${JAVA_HOME}/jre
export CLASSPATH=.:JAVA_HOME/lib:JRE_HOME/lib:${CLASSPATH}
export PATH=$JAVA_HOME/bin:$JRE_HOME/bin:$PATH
source /etc/profile
```

也可以配置当前用户环境变量 `~/.bashrc`

使用 `apt` 安装
```shell
sudo add-apt-repository ppa:webupd8team/java
sudo apt-get update
sudo apt-get install default-jre
sudo apt-get install default-jdk
sudo apt-get install oracle-java8-installer
sudo apt-get install oracle-java8-set-default

sudo update-alternatives --config java
```

## 安装一些 32 位库

```shell
sudo apt-get install lib32z1 lib32ncurses5 lib32bz2-1.0 lib32stdc++6
```

## 安装Android SDK

```shell
tar -zxvf android-sdk-linux.tgz
sudo mv android-sdk-linux /opt/android-sdk
```
配置环境变量`/etc/profile`
```shell
SDK_HOME=/opt/android-sdk
PATH=$SDK_HOME/tools:$SDK_HOME/platform-tools:$PATH
```

列出sdk相关的列表并安装
```shell
android list sdk --all
android update sdk -u -a -t  1,2,3,10,20,21,22,23
```

## 通过 PPA 安装 Android studio

```shell
sudo add-apt-repository ppa:maarten-fonville/android-studio
sudo apt-get update && sudo apt-get install android-studio
```

## 安装 Gradle

```shell
# sudo add-apt-repository ppa:cwchien/gradle
# sudo apt-get update

sudo apt-get install gradle

# or 
mkdir /opt/gradle
unzip -d /opt/gradle gradle-4.6-bin.zip
```

写入环境变量 `/etc/profile`
```shell
export GRADLE_HOME=/opt/gradle/gradle-4.6
export PATH=$GRADLE_HOME/bin:$PATH
```

# Ubuntu 下载 & 编译 Android 源码

## 安装 openjdk-7-jdk

目前用到的jdk不再是 Oracle 的 jdk ，而是开源的 openjdk，在ubuntu安装好后，使用如下命令安装jdk：

```shell
sudo apt-get install openjdk-7-jdk
```

在 `/etc/profile` 文件末尾加上：

```conf
JAVA_HOME=/usr/lib/jvm/java-7-openjdk-amd64/
PATH=$PATH:$HOME/bin:$JAVA_HOME/bin
export JAVA_HOME
export PATH
```

## 安装编译依赖的软件

使用如下命令安装依赖软件：

```shell
sudo apt-get install git gnupg flex bison gperf build-essential zip curl g++-4.8-multilib
sudo apt-get install libc6-dev libncurses5-dev:i386 x11proto-core-dev libx11-dev:i386 libreadline6-dev:i386 libgl1-mesa-dri:i386 libgl1-mesa-dev g++-multilib mingw32 tofrodos python-markdown libxml2-utils xsltproc zlib1g-dev:i386 dpkg-dev
sudo ln -s /usr/lib/i386-linux-gnu/mesa/libGL.so.1 /usr/lib/i386-linux-gnu/libGL.so
```

## 配置 Cache

使用如下命令配置cache：
```shell
sudo apt-get install ccache
source ~/.bashrc
```

### 下载源码

See:
- https://source.android.com/source/downloading.html
- https://source.android.com/source/build-numbers.html#source-code-tags-and-builds

```shell
mkdir ~/bin
PATH=~/bin:$PATH
curl https://storage.googleapis.com/git-repo-downloads/repo > ~/bin/repo
chmod a+x ~/bin/repo
mkdir android-srouce
cd android-srouce

# export HTTP_PROXY=http://127.0.0.1:8787
# export HTTPS_PROXY=http://127.0.0.1:8787

#repo init -u https://android.googlesource.com/platform/manifest
repo init -u https://android.googlesource.com/platform/manifest -b android-5.1.1_r37
repo sync
```

## 源代码的编译

参考官方指南 Building the System 先用 `build/envsetup.sh` 设置环境变量，再调用 `lunch` 命令选择构建目标。之后即执行 `make -jN` 进行编译。我用的参数为 `-j4` 编译了大约两个小时：

```shell
source build/envsetup.sh
lunch
# lunch full-eng
```

### 选择要编译的机器

- eng: 工程机
- user:最终用户机
- userdebug:调试测试机
- tests:测试机

执行 `lunch full-eng` 显示：
```shell
============================================
PLATFORM_VERSION_CODENAME=REL
PLATFORM_VERSION=5.1.1
TARGET_PRODUCT=full
TARGET_BUILD_VARIANT=eng
TARGET_BUILD_TYPE=release
TARGET_BUILD_APPS=
TARGET_ARCH=arm
TARGET_ARCH_VARIANT=armv7-a
TARGET_CPU_VARIANT=generic
TARGET_2ND_ARCH=
TARGET_2ND_ARCH_VARIANT=
TARGET_2ND_CPU_VARIANT=
HOST_ARCH=x86_64
HOST_OS=linux
HOST_OS_EXTRA=Linux-3.13.0-83-generic-x86_64-with-Ubuntu-14.04-trusty
HOST_BUILD_TYPE=release
BUILD_ID=LMY49J
OUT_DIR=out
============================================
```

### 生成驱动目录

https://developers.google.com/android/nexus/drivers#hammerheadlrx22c

接下来需要在Android官网下载所需要的驱动，下载完成后,解压出来是三个.sh文件,放到Android源码目录下面,然后执行.会将相关驱动放到vender目录下面.

### 执行编译

```shell
make -j4
```

如果出现错误：`Invalid policyvers specified: 26`

修改文件 `external//checkpolicy//checkpolicy.c`：
```c
long int n = strtol(optarg, NULL, 0);
// 改为
long int n;
errno = 0;
n = strtol(optarg, NULL, 0);
```

## 将代码导入 Android Studio

代码目录中含有一个名为 idegen 的模块，它能够为 Android 代码生成 Android Studio 下的工程文件。
大体过程是， idegen 是一个模块，要先使用 mmm 编译一下，之后在 idegen 的文件目录下有一个脚本，在代码根目录下执行这个脚本，它就会遍历代码目录生成工程文件，很方便。

```shell
mmm development/tools/idegen/
development/tools/idegen/idegen.sh
```

## 新建一个 webkit 项目，包名为 org.myleft.webkit

将源码目录下的 frameworks/base/core/java/android/webkit 目录下的所有文件拷贝到新创建的包中。
然后将所有 java 文件中的 `android.webkit` 都替换成 `org.myleft.webkit`

需要在工程中加入包含系统隐藏API的jar包，在之前源代码的编译时生成的三个jar包重命名（名称随意）后拷贝到工程的libs目录下：

- out/target/common/obj/JAVA_LIBRARIES/framework_intermediates/classes.jar
- out/target/common/obj/JAVA_LIBRARIES/core_intermediates/classes.jar
- out/target/common/obj/JAVA_LIBRARIES/bouncycastle_intermediates/classes.jar

## 编译so库

进入到安卓源码的 `external/chromium` 目录下，在终端执行以下命令：

```shell
sudo sed -i "s#android/webkit#org/myleft/webkit#g" `grep android/webkit -rl *`
```

再打开external/chromium目录下的Android.mk文件，将其中的 `libchromium_net` 都替换成 `libchromium_net2`，同时增加一行 `LOCAL_MODULE_TAGS := optional`，修改部分如下：

```mk
LOCAL_MODULE := libchromium_net2
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_TAGS := optional
INTERMEDIATES := $(call local-intermediates-dir)
```

再回到安卓源码根目录，运行以下命令：

```shell
source build/envsetup.sh
mmm external/chromium
```

编译成功后可以在 out/target/product/generic/obj/lib 目录下找到 `libchromium_net2.so` 文件。

接下来进入到安卓源码的 `external/webkit/Source/WebKit/android` 目录下，在终端执行以下命令：

```shell
sudo sed -i "s#android/webkit#org/myleft/webkit#g" `grep android/webkit -rl *`
```

同样可以将源码中的android/webkit都替换成android/webkit2，确保与我们的java代码包名相同。

再打开 `external/webkit` 目录下的 `Android.mk` 文件，将其中的 `libwebcore` 都替换成 `libwebcore2`（三个地方），并且增加一行 `LOCAL_MODULE_TAGS := optional`，另外还要将LOCAL_SHARED_LIBRARIES 里面的 `libchromium_net` 改成 `libchromium_net2`。
修改部分如下：

```mk
# Define our module and find the intermediates directory
LOCAL_MODULE := libwebcore2
LOCAL_MODULE_CLASS := STATIC_LIBRARIES
LOCAL_MODULE_TAGS := optional
base_intermediates := $(call local-intermediates-dir)
...
LOCAL_SHARED_LIBRARIES := \
libEGL \
libGLESv2 \
libandroid \
libandroidfw \
libandroid_runtime \
libchromium_net2 \
libcrypto \
...
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE := libwebcore2
LOCAL_MODULE_TAGS := optional
LOCAL_LDLIBS := $(WEBKIT_LDLIBS)
LOCAL_SHARED_LIBRARIES := $(WEBKIT_SHARED_LIBRARIES)
LOCAL_STATIC_LIBRARIES := libwebcore2 $(WEBKIT_STATIC_LIBRARIES)
LOCAL_LDFLAGS := -fvisibility=hidden
```
同样再回到安卓源码根目录，运行以下命令：

```shell
source build/envsetup.sh
mmm external/webkit
```

编译成功后可以在 out/target/product/generic/obj/lib 目录下找到 `libwebcore2.so` 文件。

完成编译并运行测试代码

下面我们将以上编译生成的两个so文件（`libchromium_net2.so` 和 `libwebcore2.so`）放到我们工程的 `libs/armeabi` 目录下。
再修改工程`org.myleft.webkit`包下的`JniUtil.java` 和 `WebViewCore.java`文件，将其中的

```java
static {
    System.loadLibrary("webcore");
    System.loadLibrary("chromium_net");
}
```

改为（注意，顺序也颠倒了）

```java
static {
    System.loadLibrary("chromium_net2");
    System.loadLibrary("webcore2");
}
```

## 拷贝资源

首先我们需要将安卓源码目录下的 frameworks/base/core/res/res/values/styles.xml 中的
```xml
<style name="ZoomControls">
    <item name="android:gravity">bottom</item>
    <item name="android:paddingLeft">15dip</item>
    <item name="android:paddingRight">15dip</item>
</style>
```
粘贴到我们工程目录下的 `res/values/styles.xml` 中。

将 `frameworks/base/core/res/res/layout/zoom_magnify.xml` 文件复制到我们工程目录下的 res/layout 中。
将 `frameworks/base/core/res/res/drawable/btn_zoom_page.xml` 文件复制到我们工程目录下的 res/drawable 中。

最后将 frameworks/base/core/res/res 下的 drawable-ldpi 、 drawable-mdpi 、 drawable-hdpi 、 drawable-xhdpi 目录下的 `btn_zoom_page_normal.png` 和 `btn_zoom_page_press.png` 文件复制到我们工程目录 res 下的相应文件夹中。

到此为止，与WebView相关的操作都完成了，我们可以开始加入测试代码。

# 从 github 获取源码

```shell
git clone --depth=1 https://github.com/android/platform_dalvik.git dalvik
git clone --depth=1 https://github.com/android/platform_bionic.git bionic
git clone --depth=1 https://github.com/android/platform_system_core.git system/core
git clone --depth=1 https://github.com/android/kernel_common.git kernel/common
git clone --depth=1 https://github.com/android/platform_external_qemu.git external/qemu
git clone --depth=1 https://github.com/android/platform_build.git build
git clone --depth=1 https://github.com/android/platform_development.git development
git clone --depth=1 https://github.com/android/platform_frameworks_base.git frameworks/base
git clone --depth=1 https://github.com/android/platform_manifest.git manifest
git clone --depth=1 https://github.com/android/platform_frameworks_support.git frameworks/support
git clone --depth=1 https://github.com/android/platform_packages_apps_settings.git packages/apps/settings
git clone --depth=1 https://github.com/android/platform_external_dhcpcd.git external/dhcpcd
git clone --depth=1 https://github.com/android/platform_external_webkit.git external/webkit
git clone --depth=1 https://github.com/android/platform_external_protobuf.git external/protobuf
git clone --depth=1 https://github.com/android/platform_packages_providers_mediaprovider.git packages/providers/mediaprovider
git clone --depth=1 https://github.com/android/platform_external_elfutils.git external/elfutils
git clone --depth=1 https://github.com/android/platform_external_strace.git external/strace
git clone --depth=1 https://github.com/android/platform_hardware_libhardware.git hardware/libhardware
git clone --depth=1 https://github.com/android/platform_external_tinyxml.git external/tinyxml
git clone --depth=1 https://github.com/android/platform_external_oprofile.git external/oprofile
git clone --depth=1 https://github.com/android/platform_external_neven.git external/neven
git clone --depth=1 https://github.com/android/platform_external_netperf.git external/netperf
git clone --depth=1 https://github.com/android/platform_external_netcat.git external/netcat
git clone --depth=1 https://github.com/android/platform_external_libxml2.git external/libxml2
git clone --depth=1 https://github.com/android/platform_external_libpng.git external/libpng
git clone --depth=1 https://github.com/android/platform_external_libpcap.git external/libpcap
git clone --depth=1 https://github.com/android/platform_external_libffi.git external/libffi
git clone --depth=1 https://github.com/android/platform_external_jpeg.git external/jpeg
git clone --depth=1 https://github.com/android/platform_external_jhead.git external/jhead
git clone --depth=1 https://github.com/android/platform_external_jdiff.git external/jdiff
git clone --depth=1 https://github.com/android/platform_external_iptables.git external/iptables
git clone --depth=1 https://github.com/android/platform_external_icu4c.git external/icu4c
git clone --depth=1 https://github.com/android/platform_external_giflib.git external/giflib
git clone --depth=1 https://github.com/android/platform_external_freetype.git external/freetype
git clone --depth=1 https://github.com/android/platform_external_fdlibm.git external/fdlibm
git clone --depth=1 https://github.com/android/platform_external_expat.git external/expat
git clone --depth=1 https://github.com/android/platform_external_esd.git external/esd
git clone --depth=1 https://github.com/android/platform_external_emma.git external/emma
git clone --depth=1 https://github.com/android/platform_external_dropbear.git external/dropbear
git clone --depth=1 https://github.com/android/platform_external_apache-http.git external/apache-http
git clone --depth=1 https://github.com/android/platform_external_openssl.git external/openssl
git clone --depth=1 https://github.com/android/platform_external_sonivox.git external/sonivox
git clone --depth=1 https://github.com/android/platform_packages_providers_telephonyprovider.git packages/providers/telephonyprovider
git clone --depth=1 https://github.com/android/platform_packages_providers_downloadprovider.git packages/providers/downloadprovider
git clone --depth=1 https://github.com/android/platform_packages_providers_contactsprovider.git packages/providers/contactsprovider
git clone --depth=1 https://github.com/android/platform_packages_providers_calendarprovider.git packages/providers/calendarprovider
git clone --depth=1 https://github.com/android/platform_packages_apps_voicedialer.git packages/apps/voicedialer
git clone --depth=1 https://github.com/android/platform_packages_apps_stk.git packages/apps/stk
git clone --depth=1 https://github.com/android/platform_packages_apps_soundrecorder.git packages/apps/soundrecorder
git clone --depth=1 https://github.com/android/platform_packages_apps_phone.git packages/apps/phone
git clone --depth=1 https://github.com/android/platform_packages_apps_packageinstaller.git packages/apps/packageinstaller
git clone --depth=1 https://github.com/android/platform_packages_apps_music.git packages/apps/music
git clone --depth=1 https://github.com/android/platform_packages_apps_mms.git packages/apps/mms
git clone --depth=1 https://github.com/android/platform_packages_apps_htmlviewer.git packages/apps/htmlviewer
git clone --depth=1 https://github.com/android/platform_packages_apps_email.git packages/apps/email
git clone --depth=1 https://github.com/android/platform_packages_apps_contacts.git packages/apps/contacts
git clone --depth=1 https://github.com/android/platform_packages_apps_camera.git packages/apps/camera
git clone --depth=1 https://github.com/android/platform_packages_apps_calendar.git packages/apps/calendar
git clone --depth=1 https://github.com/android/platform_packages_apps_calculator.git packages/apps/calculator
git clone --depth=1 https://github.com/android/platform_packages_apps_browser.git packages/apps/browser
git clone --depth=1 https://github.com/android/platform_hardware_ril.git hardware/ril
git clone --depth=1 https://github.com/android/platform_external_zlib.git external/zlib
git clone --depth=1 https://github.com/android/platform_external_yaffs2.git external/yaffs2
git clone --depth=1 https://github.com/android/platform_external_tcpdump.git external/tcpdump
git clone --depth=1 https://github.com/android/platform_external_tagsoup.git external/tagsoup
git clone --depth=1 https://github.com/android/platform_external_srec.git external/srec
git clone --depth=1 https://github.com/android/platform_external_sqlite.git external/sqlite
git clone --depth=1 https://github.com/android/platform_external_skia.git external/skia
git clone --depth=1 https://github.com/android/platform_external_safe-iop.git external/safe-iop
git clone --depth=1 https://github.com/android/platform_external_ppp.git external/ppp
git clone --depth=1 https://github.com/android/kernel_msm.git kernel/msm
git clone --depth=1 https://github.com/android/platform_system_bluetooth.git system/bluetooth
git clone --depth=1 https://github.com/android/platform_prebuilt.git prebuilt
git clone --depth=1 https://github.com/android/platform_packages_providers_drmprovider.git packages/providers/drmprovider
git clone --depth=1 https://github.com/android/platform_external_ping.git external/ping
git clone --depth=1 https://github.com/android/platform_external_dbus.git external/dbus
git clone --depth=1 https://github.com/android/tools_repo.git tools/repo
git clone --depth=1 https://github.com/android/android.github.io.git android.github.io
git clone --depth=1 https://github.com/android/platform_external_wpa_supplicant.git external/wpa/supplicant
git clone --depth=1 https://github.com/android/platform_packages_apps_googlesearch.git packages/apps/googlesearch
git clone --depth=1 https://github.com/android/platform_system_wlan_ti.git system/wlan/ti
git clone --depth=1 https://github.com/android/platform_packages_providers_googlecontactsprovider.git packages/providers/googlecontactsprovider
git clone --depth=1 https://github.com/android/platform_packages_apps_im.git packages/apps/im
git clone --depth=1 https://github.com/android/platform_packages_providers_improvider.git packages/providers/improvider
git clone --depth=1 https://github.com/android/platform_external_tremor.git external/tremor
git clone --depth=1 https://github.com/android/platform_packages_apps_sync.git packages/apps/sync
git clone --depth=1 https://github.com/android/platform_packages_apps_alarmclock.git packages/apps/alarmclock
git clone --depth=1 https://github.com/android/platform_external_aes.git external/aes
git clone --depth=1 https://github.com/android/platform_external_clearsilver.git external/clearsilver
git clone --depth=1 https://github.com/android/platform_external_googleclient.git external/googleclient
git clone --depth=1 https://github.com/android/platform_packages_apps_launcher.git packages/apps/launcher
git clone --depth=1 https://github.com/android/platform_external_bluez.git external/bluez
git clone --depth=1 https://github.com/android/platform_external_gdata.git external/gdata
git clone --depth=1 https://github.com/android/platform_frameworks_opt_com.google.android.git frameworks/opt/com.google.android
git clone --depth=1 https://github.com/android/platform_external_elfcopy.git external/elfcopy
git clone --depth=1 https://github.com/android/platform_frameworks_policies_base.git frameworks/policies/base
git clone --depth=1 https://github.com/android/platform_packages_apps_updater.git packages/apps/updater
git clone --depth=1 https://github.com/android/platform_external_opencore.git external/opencore
```
