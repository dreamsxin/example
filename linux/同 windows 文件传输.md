# 
## lrzsz
rz 和 sz 是 Linux/Unix 同 Windows 进行 Zmodem 文件传输的命令工具，都使用Zmodem文件传输协议。
- rz: receive zmodem的缩写，运行该命令会弹出一个文件选择窗口，从本地选择文件上传到Linux服务器。
- sz: send zmodem的缩写，将选定的文件发送（send）到本地服务器，即从Linux下载到Windows系统；
```shell
yum install -y lrzsz
```
### sz命令
sz 命令是利用 ZModem 协议来从 Linux 服务器传送文件到本地，一次可以传送一个或多个文件。
- -a       ：  以文本方式传输（ascii）。
- -b       ：  以二进制方式传输（binary）。
- -e       ：  对控制字符转义（escape），这可以保证文件传输正确。

**常用命令：**

如果能够确定所传输的文件是文本格式的，使用 `sz -a files`
如果是二进制文件，使用 `sz -be files`

### rz命令
rz：运行该命令会弹出一个文件选择窗口，从本地选择文件上传到Linux服务器。使用rz -y 命令时会弹出选择文件对话框

常用参数：
- -b      ：   以二进制方式，默认为文本方式。（Binary (tell it like it is) file transfer override.）
- -e      ：   对所有控制字符转义。（Force sender to escape all control characters; normally XON, XOFF, DLE, CR-@-CR, and Ctrl-X are escaped.）

## trzsz 
