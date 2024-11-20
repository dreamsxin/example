# SSH 使用

## SSH 参数解释

-f 后台运行
-C 允许压缩数据
-N 不执行任何命令
-R 将端口绑定到远程服务器，反向代理
-L 将端口绑定到本地客户端，正向代理


## 设置 SSH 代理

```shell
ssh -C -v -N -D 127.0.0.1:7070 xxx@x.x.x.x -p 22022 -pw 密码
ssh -qTfnN -D 7070 xxx@x.x.x.x -p 22
ssh x.x.x.x -l username -p 22 -D 7070
```

## 通过 SSH 代理服务器访问内网机器

假设 A（192.168.1.1）、C（192.168.0.1）为两个不同局域网内的PC，B（B.B.B.B）为代理服务器。我要实现 C 使用 SSH 直接登录到 A。

### 第一步 在 A 机器上做到 B 机器的反向代理

在 A 机器上运行命令，将代理服务器 B 端口 8888 与 A 机器上的端口 22 绑定：

```shell
ssh -fCNR 8888:localhost:22 username@B.B.B.B
```

### 第二步 在 B 机器上做正向代理

在代理服务器 B 上运行命令，将端口 9999 数据转发到 8888，其中的*表示端口 9999 接受来自任意机器的访问：

```shell
ssh -fCNL "*:9999:localhost:8888" localhost
```

### 第三步 在 C 机器上使用 SSH 登录 A 机器

```shell
ssh -p 9999 username@B.B.B.B
```

## 调用远程服务器指令

从本地系统通过 SSH 在远程系统上运行命令或脚本的典型方法是：
```shell
ssh <username@IP_Address-or-Doman_name> <Command-or-Script>
```

### 查找远程 Linux 系统的内核详细信息。为此，只需运行：
```shell
ssh sk@192.168.225.22 uname -a
```

### 通过 SSH 在远程主机上执行多个命令
你还可以在远程主机上运行多个命令，方法是将它们放在引号中。
```shell
ssh sk@192.168.225.22 "uname -r && lsb_release -a"
ssh sk@192.168.225.22 "uname -r ; lsb_release -a"
```

**提示： 了解 && 和 ; 在命令中的区别**
> 操作符只有在第一个命令成功时才执行第二个命令。

### 通过 SSH 在远程机器上调用有 sudo 权限的命令（带参数 -t）

命令中使用了 `-t` 标志，我们需要使用它来强制进行伪终端分配。它用于在远程机器上执行任意基于屏幕的程序，这非常有用。例如，在实现菜单服务时。

有些命令需要 sudo 权限才能运行。例如，以下命令将在我的远程系统上安装 apache2。

```shell
ssh -t sk@192.168.225.22 sudo apt install apache2
```

### 通过 SSH 在远程系统上运行本地脚本
让我们在本地系统上创建一个简单的脚本来显示关于远程系统的发行版名称、包管理和基本细节等。

`system_information.sh`
添加以下行：
```shell
#!/bin/bash
#Name: Display System Details
#Owner: OSTechNIx
#----------------------------
echo /etc/*_ver* /etc/*-rel*; cat /etc/*_ver* /etc/*-rel*
```

现在，通过 SSH 命令在远程系统上运行这个脚本：
```shell
ssh sk@192.168.225.22 'bash -s' < system_information.sh
```

### 将远程主机的命令输出保存到本地主机
如果你希望与支持团队或同事共享远程系统上运行的命令输出，那么这非常有用。

以下命令将通过 SSH 在远程系统运行 du -ah，并将输出保存在本地系统的 diskusage.txt 文件中。
```shell
ssh sk@192.168.225.22 du -ah > diskusage.txt
```

### 配置 SSH 密钥认证，避免输入密码
如果你经常在远程系统上运行命令，你可能需要配置基于 SSH 密钥的身份验证，以便每次跳过密码输入。更多细节可以在以下链接中找到。

Linux 系统下如何配置 SSH 密钥认证，将客户端的公钥 `~/.ssh/id_rsa.pub` 存入目标服务器的 `~/.ssh/authorized_keys` 文件
```shell
ssh-keygen
# 如果存在一个没有密码的文件，可以更新为有密码
ssh-keygen -p -f ~/.ssh/id_rsa
# 将SSH公钥复制到远程SSH服务器
ssh-copy-id ostechnix@192.168.1.20
# 替换
ssh-copy-id -f ostechnix@192.168.1.20
#在远程SSH服务器中禁用基于SSH密码的身份验证
sudo nano /etc/ssh/sshd_config `PasswordAuthentication no`
sudo systemctl restart sshd
```

配置了基于 SSH 密钥的认证后，我们可以通过 SSH 在远程机器上执行命令，从而不需要输入密码：
```shell
ssh sk@192.168.225.22 sudo apt update
```

### 通过 sshpass 在远程机器上运行命令
如果你不想配置基于 SSH 密钥的身份验证，你可以使用 sshpass 实用程序。

sshpass 是为使用键盘交互密码身份验证模式运行 ssh 而设计的，但它以非交互的方式。简单来说，sshpass 提供了非交互式的方式来验证 SSH 会话。

SSH 使用直接 TTY 访问来确保密码确实是由交互式键盘用户发出的。sshpass 在一个专用 tty 中运行 SSH，让它误以为从交互用户那里获得了密码。

```shell
sudo apt install sshpass
```
```shell
sshpass -p <remote-password> ssh remoteuser@ip-address <command-to-execute>
sshpass -p ubuntu ssh ostechnix@192.168.1.30 uname -a
```
其中，

- -p ubuntu - 提供远程系统的密码。
- ostechnix@192.168.1.30 - 远程系统用户名和地址。
- uname -a - 要在远程计算机上执行的命令

### 密码作为环境变量提供
在这个方法中，我们声明一个名为 SSHPASS 的环境变量，用远程环境的密码作为其值。然后我们使用 -e 标志，如下所示：
```shell
SSHPASS=ubuntu sshpass -e ssh ostechnix@192.168.1.30 uname -a
```
### 从文本文件中读取密码
使用 echo 命令在文本文件中追加密码：
```shell
echo "ubuntu" > mypassword.txt
```
现在，将密码文件传递给带有 -f 标志的 sshpass，如下所示：
```shell
sshpass -f mypassword.txt ssh ostechnix@192.168.1.30 uname -a
```

## 客户端

- MobaXterm Home Edition
- Bitvise SSH Client
