Ansible是新出现的自动化运维工具，基于Python开发，集合了众多运维工具（puppet、cfengine、chef、func、fabric）的优点，实现了批量系统配置、批量程序部署、批量运行命令等功能。ansible是基于模块工作的，本身没有批量部署的能力。真正具有批量部署的是ansible所运行的模块，ansible只是提供一种框架。主要包括：

1. 连接插件connection plugins：负责和被监控端实现通信；
2. host inventory：指定操作的主机，是一个配置文件里面定义监控的主机；
3. 各种模块核心模块、command模块、自定义模块；
4. 借助于插件完成记录日志邮件等功能；
5. playbook：剧本执行多个任务时，非必需可以让节点一次性运行多个任务。

从github下载源码：

```shell
git clone https://github.com/ansible/ansible.git --recursive
./ansible
```
运行环境配置脚本：

```shell
source ./hacking/env-setup
```
如果没有安装pip，请先安装对应于你的Python版本的pip：
```shell
sudo easy_install pip
```

    以下的Python模块也需要安装：
```shell
sudo pip install paramiko PyYAML Jinja2 httplib2 six pycrypto
```

如果安装paramiko 的时候系统报openssl相关的错误，那么需要再安装以下文件：

```shell
sudo apt-get install -y libffi-dev libssl-dev python-dev
```

当更新ansible版本时,不只要更新git的源码树，也要更新git中指向Ansible自身模块的 “submodules” (不是同一种模块)：

```shell
git pull --rebase
git submodule update --init --recursive
```

## Ansible 配置

1、SSH免密钥登录设置

使用ssh-keygen生成key-pair，然后将公钥配置在每台服务器`.ssh/authorized_keys`文件中：

```shell
ssh-keygen -t rsa -P ''
```

写入信任文件（将/root/.ssh/id_rsa.pub分发到其他服务器，并在所有服务器上执行如下指令）：

```shell
cat /root/.ssh/id_rsa.pub >> /root/.ssh/authorized_keys
chmod 600 /root/.ssh/authorized_keys
```

如果需要配置公钥的服务器数量比较多的话，也可以通过使用ansible中的authorize_key模块来进行分布式部署。

2、Ansible配置

Ansible使用前通常需要配置/etc/anisble/路径下的hosts和ansible.cfg这个两个文件。要注意的是，编辑这两个文件需要root权限。

```shell
mkdir -p /etc/ansible`
vim /etc/ansible/ansible.cfg`
```

```conf
remote_port = 36000
private_key_file = /root/.ssh/id_rsa
```

Ansible 可同时操作属于一个组的多台主机，组和主机之间的关系通过 inventory 文件(INI 格式)配置，默认的文件路径为 /etc/ansible/hosts。一个系统可以属于不同的组，比如一台服务器可以同时属于 webserver组 和 dbserver组。

`/etc/ansible/hosts`
```conf
[test]
10.139.13.80
```

`/etc/ansible/hosts`是配置分组的默认文件，还有一种方式可以通过定义环境变量ANSIBLE_HOSTS来改变配置文件的引用路径：
```shell
export ANSIBLE_HOSTS=~/test/qiuyi_ansible/test_hosts
```

