## 

**安装 choco**
```shell
Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))
```

**安装 rabbitmq**
```shell
choco install rabbitmq
```

**启动 RabbitMQ 服务**
使用管理员身份运行 PowerShell，运行以下命令以启动 RabbitMQ 服务：
```shell
Start-Service rabbitmq-server
```
**启用 RabbitMQ 管理插件**
使用管理员身份运行 PowerShell，运行以下命令以启用 RabbitMQ 管理插件：
```shell
rabbitmq-plugins enable rabbitmq_management
```
访问管理界面，账户密码guest，`http://localhost:15672/`
## 使用

加入环境变量 path `C:\Program Files\RabbitMQ Server\rabbitmq_server-3.12.10\sbin`


**停止**
```shell
rabbitmqctl.bat stop
rabbitmqctl.bat shutdown
```
**启动**
```shell
rabbitmq-server.bat
```
```shell
# lists commands provided by rabbitmqctl.bat
rabbitmqctl.bat help

# lists commands provided by rabbitmq-diagnostics.bat
rabbitmq-diagnostics.bat help

# ...you guessed it!
rabbitmq-plugins.bat help
```
发生错误 `Error: unable to perform an operation on node 'rabbit@YC-20231127CPCD'. Please see diagnostics information and suggestions below.`
复制 `C:\Windows\System32\config\systemprofile\.erlang.cookie` 内容到 `C:\Users\xxx\.erlang.cookie`
