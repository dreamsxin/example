# SSH 使用

## SSH 参数解释

-f 后台运行
-C 允许压缩数据
-N 不执行任何命令
-R 将端口绑定到远程服务器，反向代理
-L 将端口绑定到本地客户端，正向代理


## SSH 代理

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
ssh -fCNL "*:9999:localhost:8888' localhost
```

### 第三步 在 C 机器上使用 SSH 登录 A 机器

```shell
ssh -p 9999 username@B.B.B.B
```