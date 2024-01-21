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

https://github.com/trzsz/trzsz

### trz 上传文件
trz 命令可以不带任何参数，将上传文件到当前目录。也可以带一个目录参数，指定上传到哪个目录。
`trz /tmp/ `

### tsz 下载文件
tsz 可以带一个或多个文件名（可使用相对路径或绝对路径，也可使用通配符），将下载指定的文件。

 
`tsz file1 file2 file3 `
-q 静默模式
trz -q 或 tsz -q xxx ( 加上 -q 选项 )，则在传输文件时不显示进度条。

-y 覆盖模式
trz -y 或 tsz -y xxx ( 加上 -y 选项 )，如果存在相同文件名的文件就直接覆盖。

-b 二进制模式
trz -b 或 tsz -b xxx ( 加上 -b 选项 )，二进制传输模式，对于压缩包、图片、影音等较快。

-e 转义控制字符
二进制传输模式时，控制字符可能会导致传输失败，trz -eb 或 tsz -eb xxx ( 加上 -e 选项 ) 转义所有已知的控制字符。

-B 缓冲区大小
trz -B 10k 或 tsz -B 2M xxx 等，设置缓存区大小 ( 默认 1M )。太小会导致传输速度慢，太大会导致进度条更新不及时。

-t 超时时间
trz -t 10 或 tsz -t 30 xxx 等，设置超时秒数 ( 默认 100 秒 )。在超时时间内，如果无法传完一个缓冲区大小的数据则会报错并退出。设置为 0 或负数，则永不超时。
