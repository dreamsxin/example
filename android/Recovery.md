说明：本教程以中兴的[N881F]为例，Android源码为CyanogenMod 10.1

地址：https://github.com/Gaojiquan/android_device_zte_N881F
----




*1 准备Ubuntu作为您的操作系统，笔者的版本是12.04_amd64。

*2 准备 Android 源码的编译环境，主要是安装一些编译用到的lib库，以及同步源码的一些工具，如GIT，CURL，REPO等。具体可参考：http://source.android.com/source/index.html[source.android.com]

*3 在确保环境已准备妥当之后，接下来开始下载 Android 源码，此文以cm 10.1  源码为例。

1).创建一个用于存放源码的目录，如：jellybean/system

      mkdir –p jellybean/system


2).切换到该目录

      cd  jellybean/system


3).初始化cm-10.1的分支

      repo init -u git://github.com/CyanogenMod/android.git -b cm-10.1

4).同步源码

      repo sync –j30

*4 适配你的Vendor

1).前提条件

* 已root;

* 有boot.img或者recovery.img

2).手动建立Vendor文件夹：device/ZTE/N881F，规则为“device/device_manufacturer_name/device_name”，这里  “device_manufacturer_name” 指的是你要编译的设备的厂商（如ZTE，HTC，MOTO等），“device_name” 指的是你的设备名称。

      mkdir –p device/ZTE/N881F

3).获取boot.img

adb连接手机状态下，输入

      adb shell

进入shell模式，保证手机已root，输入 su，转入root权限，输入

      cat /proc/mtd

查看有recovery（和/或）boot路径，如果存在，分别执行

      dd if=/your_boot_partition of=/your_sdcard_if_exists/boot.img
      dd if=/your_recovery_partition of=/your_sdcard_if_exists/recovery.img

your_boot_partition 是mtd文件中记载的boot的路径，your_sdcard_if_exists 是你的sdcard路径，recovery同理，完成后退出shell,在终端或命令行下执行

      adb pull /your_sdcard_if_exists/boot.img d:\boot.img
      adb pull /your_sdcard_if_exists/recovery.img d:\recovery.img

这样就获取了boot.img和recovery.img文件

如果cat /proc/mtd 提示 No such file or directory，请在adb shell的root权限下执行

      mount

在输出中查找 system,userdata,cache的路径，通常情况下,boot,recovery路径会和他们保持一致，一个可能的输出是

    root@edison:/ # mount
    mount
    rootfs / rootfs ro,relatime 0 0
    tmpfs /dev tmpfs rw,nosuid,relatime,mode=755 0 0
    devpts /dev/pts devpts rw,relatime,mode=600 0 0
    proc /proc proc rw,relatime 0 0
    sysfs /sys sysfs rw,relatime 0 0
    none /acct cgroup rw,relatime,cpuacct 0 0
    tmpfs /mnt/asec tmpfs rw,relatime,mode=755,gid=1000 0 0
    tmpfs /mnt/obb tmpfs rw,relatime,mode=755,gid=1000 0 0
    none /dev/cpuctl cgroup rw,relatime,cpu 0 0
    /dev/block/system /system ext3 ro,relatime,barrier=1,data=ordered 0 0
    /dev/block/pds /pds ext3 rw,nosuid,nodev,noatime,nodiratime,errors=continue,barrier=1,data=ordered 0 0
    /dev/block/preinstall /preinstall ext3 rw,nosuid,nodev,noatime,nodiratime,barrier=1,data=ordered 0 0
    /dev/block/userdata /data ext3 rw,relatime,errors=continue,barrier=0,data=ordered 0 0
    /dev/block/cache /cache ext3 rw,nosuid,nodev,noatime,nodiratime,errors=continue,barrier=1,data=ordered 0 0
    /dev/fuse /mnt/sdcard fuse      rw,nosuid,nodev,relatime,user_id=1023,group_id=1023,default_permissions,allow_other 0 0
    /dev/block/vold/179:97 /mnt/external1 vfat rw,dirsync,nosuid,nodev,noexec,relatime,uid=1000,gid=1015,fmask=0702,dmask=0702,allow_utime=0020,codepage=cp437,iocharset=iso8859-1,shortname=mixed,utf8,errors=remount-ro 0 0

分析内容发现，没有boot，没有Recovery分区。。。不急，咱们找规律。
/dev/block/cache，/dev/block/userdata，/dev/block/system。。。这样是不是boot和recovery分区路径为/dev/block/boot和/dev/block/recovery？

此时查看 /dev/block 目录

    root@edison:/ # ls /dev/block
    ls /dev/block                    
    boot                             
    cache                            
    cdrom                            
    cid   
    ...
    recovery
    ...

真有，那么 boot 和 recovery 即是我们要寻找的文件，接下来，同样使用 dd 命令导出它们到sdcard，再使用adb pull命令导出到本地磁盘                           
      
4).生成vendor
     
       source build/envsetup.sh
       make -j4 otatools
      build/tools/device/mkvendor.sh device_manufacturer_name device_name /your/path/to/the/boot.img

实际命令如下： 

      source build/envsetup.sh
      make -j4 otatools
      build/tools/device/mkvendor.sh ZTE N881F boot.img

上述命令假设你的boot.img存放于源码目录下。

5).提取recovery.fstab。经过第4)步骤后，会生成一份默认的recovery.fstab到你的N881F目录下，这时我们需要获取一份属于你机器的挂载点，这时候有两种办法：

* 直接查看手机里的“/cache/recovery/last_log”，如果该文件存在，则查找如下内容：

      recovery filesystem table

=========================

如果有上述内容，那就将它拷贝粘贴至recovery.fstab。

* 解包recovery.img，解压后的etc目录下有此文件，直接拷过来用。文件内容：

      mount point    fstype    device                        [device2]
      /boot        emmc        /dev/block/mmcblk0p8
      /cache        ext4        /dev/block/mmcblk0p15
      /data        ext4        /dev/block/mmcblk0p13
      /misc        emmc        /dev/block/mmcblk0p17
      /recovery        mtd        /dev/block/mmcblk0p16
      /sdcard        vfat        /dev/block/mmcblk1p1        /dev/block/mmcblk1
      /system        ext4        /dev/block/mmcblk0p12
      /sdcard2        vfat        /dev/block/mmcblk0p20

6).准备 recovery.rc

    import /init.recovery.${ro.hardware}.rc
    on early-init
    start ueventd
    on init
    export PATH /sbin
    export ANDROID_ROOT /system
    export ANDROID_DATA /data
    export EXTERNAL_STORAGE /sdcard
    symlink /system/etc /etc
    mkdir /boot
    mkdir /recovery
    mkdir /sdcard
    mkdir /internal_sd
    mkdir /external_sd
    mkdir /sd-ext
    mkdir /datadata
    mkdir /emmc
    mkdir /system
    mkdir /data
    mkdir /cache
    mount /tmp /tmp tmpfs
    chown root shell /tmp
    chmod 0775 /tmp
    write /sys/class/android_usb/android0/enable 0

    #可能会修改的地方
    write /sys/class/android_usb/android0/idVendor 19d2
    write /sys/class/android_usb/android0/idProduct 1361
    write /sys/class/android_usb/android0/functions mass_storage,adb


    write /sys/class/android_usb/android0/iManufacturer ${ro.product.manufacturer}
    write /sys/class/android_usb/android0/iProduct ${ro.product.model}
    write /sys/class/android_usb/android0/iSerial ${ro.serialno}
    on boot
    ifup lo
    hostname localhost
    domainname localdomain
    class_start default
    service ueventd /sbin/ueventd
    critical
    service recovery /sbin/recovery
    service adbd /sbin/adbd recovery
    disabled
    # Always start adbd on userdebug and eng builds
    on property:ro.debuggable=1
    write /sys/class/android_usb/android0/enable 1
    start adbd
    setprop service.adb.root 1
    # Restart adbd so it can run as root
    on property:service.adb.root=1
    write /sys/class/android_usb/android0/enable 0
    restart adbd
    write /sys/class/android_usb/android0/enable 1

7).修改 recovery.rc

这份文件大致是通用的，可能需要修改的地方单独指出来了，如若Recovery编译完后USB大容量不能正常挂载，则可去手机里查看“/sys/class/android_usb/android0/idVendor”和“/sys/class/android_usb/android0/idProduct”这两个文件的内容，然后依次填入上述可修改处的值（如 idVendor 19d2，把“19d2“换成“/sys/class/android_usb/android0/idVendor”对应的值即可）。

在Ubuntu下还可以用lsusb命令查看这两个值:

    
    focus@ubuntu:/media/linux/jellybeanplus/system$ lsusb
    Bus 001 Device 001: ID 1d6b:0002 Linux Foundation 2.0 root hub
    Bus 002 Device 001: ID 1d6b:0002 Linux Foundation 2.0 root hub
    Bus 003 Device 001: ID 1d6b:0002 Linux Foundation 2.0 root hub
    Bus 004 Device 001: ID 1d6b:0003 Linux Foundation 3.0 root hub
    Bus 001 Device 002: ID 8087:0024 Intel Corp. Integrated Rate Matching Hub
    Bus 002 Device 002: ID 8087:0024 Intel Corp. Integrated Rate Matching Hub
    Bus 001 Device 003: ID 064e:d20c Suyin Corp. 
    Bus 002 Device 003: ID 046d:c52b Logitech, Inc. Unifying Receiver
    Bus 003 Device 013: ID 19d2:1361 ZTE WCDMA Technologies MSM 




最后一行的19d2和1361即为idVendor和idProduct。

8).重写按键对应的c文件 recovery_ui.c

9).提取设备的根文件系统的rc或sh文件，如init.qcom.usb.rc，init.qcom.usb.sh。（仅在根文件系统的目录下有这些文件的情况下成立，如无则忽略）

10).查看内核基址，ramdisk文件的offset偏移量等

11).提取system.prop

    adb pull /system/build.prop

从手机里pull过来build.prop后，拷贝到N881F目录，打开文件，将文件内容拷贝至system.prop，然后将build.prop文件删除。

*5 开始编译

1).设置源码环境变量

    gedit ~/.bashrc

添加如下环境变量：

    # Android environment
    export TARGET_ARCH=arm
    export ANDROID_SDK_TOOLS=<YOUR_ANDROID_SDK_DIR>/tools
    export ANDROID_TOOLCHAIN=<YOUR_SOURCE_CODE_DIR>/prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin
    export PATH=${ANDROID_SDK_TOOLS}:${ANDROID_TOOLCHAIN}:$PATH

    # Java environment
    JAVA_HOME=<YOUR_JAVA_SDK_DIR> 
    export JRE_HOME=$JAVA_HOME/jre
    export CLASSPATH=.:$JAVA_HOME/lib:$JRE_HOME/lib:$CLASSPATH
    export PATH=$JAVA_HOME/bin:$JRE_HOME/bin:$PATH

执行命令使环境变量生效：
    source ~/.bashrc

2).编译你的Vendor

   cd <源码目录>
   source build/envsetup.sh
   lunch cm_N881F-eng
   make -j4 recoveryimage