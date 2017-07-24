Docker run命令用于运行一个新容器，而启动一个容器需要非常多的信息，所以该命令的参数非常多，今天就详细分析下该命令支持的参数。

首先看一下该命令的格式：

```text
    Usage: docker run [OPTIONS] IMAGE [COMMAND] [ARG...]  
      -a, --attach=[]            登录容器（以docker run -d启动的容器）  
      -c, --cpu-shares=0         设置容器CPU权重，在CPU共享场景使用  
      --cap-add=[]               添加权限，权限清单详见：http://linux.die.net/man/7/capabilities  
      --cap-drop=[]              删除权限，权限清单详见：http://linux.die.net/man/7/capabilities  
      --cidfile=""               运行容器后，在指定文件中写入容器PID值，一种典型的监控系统用法  
      --cpuset=""                设置容器可以使用哪些CPU，此参数可以用来容器独占CPU  
      -d, --detach=false         指定容器运行于前台还是后台   
      --device=[]                添加主机设备给容器，相当于设备直通  
      --dns=[]                   指定容器的dns服务器  
      --dns-search=[]            指定容器的dns搜索域名，写入到容器的/etc/resolv.conf文件  
      -e, --env=[]               指定环境变量，容器中可以使用该环境变量  
      --entrypoint=""            覆盖image的入口点  
      --env-file=[]              指定环境变量文件，文件格式为每行一个环境变量  
      --expose=[]                指定容器暴露的端口，即修改镜像的暴露端口  
      -h, --hostname=""          指定容器的主机名  
      -i, --interactive=false    打开STDIN，用于控制台交互  
      --link=[]                  指定容器间的关联，使用其他容器的IP、env等信息  
      --lxc-conf=[]              指定容器的配置文件，只有在指定--exec-driver=lxc时使用  
      -m, --memory=""            指定容器的内存上限  
      --name=""                  指定容器名字，后续可以通过名字进行容器管理，links特性需要使用名字  
      --net="bridge"             容器网络设置，待详述  
      -P, --publish-all=false    指定容器暴露的端口，待详述  
      -p, --publish=[]           指定容器暴露的端口，待详述  
      --privileged=false         指定容器是否为特权容器，特权容器拥有所有的capabilities  
      --restart=""               指定容器停止后的重启策略，待详述  
      --rm=false                 指定容器停止后自动删除容器(不支持以docker run -d启动的容器)  
      --sig-proxy=true           设置由代理接受并处理信号，但是SIGCHLD、SIGSTOP和SIGKILL不能被代理  
      -t, --tty=false            分配tty设备，该可以支持终端登录  
      -u, --user=""              指定容器的用户  
      -v, --volume=[]            给容器挂载存储卷，挂载到容器的某个目录  
      --volumes-from=[]          给容器挂载其他容器上的卷，挂载到容器的某个目录  
      -w, --workdir=""           指定容器的工作目录  
```

故障处理
--restart参数，支持三种逻辑实现：
- no：容器退出时不重启
- on-failure：容器故障退出（返回值非零）时重启
- always：容器退出时总是重启


端口暴露
- -P参数：docker自动映射暴露端口
- docker run -d -P training/webapp
docker自动在host上打开49000到49900的端口，映射到容器（由镜像指定，或者--expose参数指定）的暴露端口

-p参数：指定端口或IP进行映射；

- docker run -d -p 5000:80 training/webapp
host上5000号端口，映射到容器暴露的80端口
- docker run -d -p 127.0.0.1:5000:80 training/webapp
host上127.0.0.1:5000号端口，映射到容器暴露的80端口
- docker run -d -p 127.0.0.1::5000 training/webapp
host上127.0.0.1:随机端口，映射到容器暴露的80端口
- docker run -d -p 127.0.0.1:5000:5000/udp training/webapp
绑定udp端口


网络配置

- --net=bridge： 使用docker daemon指定的网桥
- --net=host： 容器使用主机的网络
- --net=container:NAME_or_ID：使用其他容器的网路，共享IP和PORT等网络资源
- --net=none： 容器使用自己的网络（类似--net=bridge），但是不进行配置
