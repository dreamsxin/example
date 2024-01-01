# Genymotion

## 下载安装 Genymotion

地址：https://www.genymotion.com/download

执行以下命令:
```shell
chmod +x genymotion-2.12.0-linux_x64.bin
./genymotion-2.12.0-linux_x64.bin
```
执行完成之后，进入 genymotion 目录,双击 genymotion 图标就可以启动 Genymotion 了

## shell

https://docs.genymotion.com/desktop/05_Genymotion_Shell/

### 命令行选项
- -h 输出帮助信息

- -r ip_address 连接到特定Genymotion虚拟设备

- -c "command" 在原生shell环境中执行给定命令而后返回

- -f 执行该文件内容，每条命令只作用于当前行

### 可用命令

- battery getmode 使选定虚拟设备返回当前电池模式，该模式只能是：

host:虚拟电池符合当前主机电池状态（如果主机拥有电池）

manual:在本模式下，您可以设定电池电量数值及电源状态

- battery setmode设定电池模式。该模式只能是：

host: 虚拟电池符合当前主机电池状态（如果主机拥有电池）

manual:在本模式下，您可以设定电池电量数值及电源状态

- battery getlevel返回当前电池电量值。该数值只能在0%到100%之间。如果电池模式为“主机”，则返回当前主机的电池电量值。

- battery setlevel设定当前电池电量值。该数值只能在0%到100%之间。设定电池电量为“手动”模式：如果最后一次使用的模式为“主机”，则将其设置为“手动”。

- battery getstatus返回当前电池状态。共有四种可能存在的状态：

Discharging:电池未连接、电池未充电。

Charging:电源已连接，电池正在充电。

Full:电池电量已满。

Unknown:有时候电池状态会无法确认，例如主机并不具备电池。

- battery setstatus设定当前电池状态。共有四种可能存在的状态：

Discharging: 电池未连接、电池未充电。

Charging:电源已连接，电池正在充电。

Full:电池电量已满。

Unknown:有时候电池状态会无法确认，例如主机并不具备电池。

- devices list列出可用的Genymotion虚拟设备并提供其相关信息，例如当前状态或者IP地址。

- devices ping发出ping命令检查虚拟设备是否响应。

- devices refresh刷新Genymotion虚拟设备列表。利用本命令保持列表处于最新状态。

- devices select选择您想要与之交互的Genymotion虚拟设备。

- devices show列出可用的Genymotion虚拟设备并提供相关信息，例如当前状态或者IP地址。

- gps activate激活GPS传感器（如果尚未激活）

- gps desactivate关闭GPS传感器（如果已经激活）

- gps getlatitude返回当前纬度值（如果GPS已经激活且存在纬度值）或者0。

- gps setlatitude设定纬度值（如果GPS尚未激活则将其激活）。

- gps getlongitude返回当前经度值（如果GPS已经激活且存在经度值）或者0。

- gps setlongitude设定经度值（如果GPS尚未激活则将其激活）。

- gps getaltitude返回当前海拔高度值（如果GPS已经激活且存在海拔高度值）或者0。

- gps setaltitude设定海拔高度值（如果GPS尚未激活则将其激活）。

- gps getaccuracy返回当前准确度值（如果GPS已经激活且存在精度值）或者0。

- gps setaccuracy设定精度值（如果GPS尚未激活则将其激活）。

- gps getorientation返回当前方位值（如果GPS已经激活且存在精度值）或者0。

- gps setorientation设定方位值（如果GPS尚未激活则将其激活）。

- rotation setangle为虚拟设备设定旋转角度。

- android version返回选定虚拟设备的Android版本号。

- build number返回genymotion shell数量。

- help提示帮助信息。

- pause暂停执行（以秒为单位）。

- version返回Genymotion Shell版本。

- exit 或者 quit关闭Genymotion Shell。

## 安装 apk
可以直接拖拽 apk 安装，也可以用 shell 安装，先查看启动的虚拟机的菜单里点击 setting 查看地址，然后连接
```shell
adb.exe connect 192.168.56.101:5555
adb devices

gmtool device install xxx.apk
```
