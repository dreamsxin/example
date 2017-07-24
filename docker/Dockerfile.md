# 使用Dockerfile构建镜像

构建环境上下文(context)

```shell
mkdir static_web
cd static_web
touch Dockerfile
```
写入Dockerfile文件内容

```shell
FROM ubuntu  #基础镜像
MAINTAINER roubin "test@example.com"  #作者和联系方式
RUN apt-get update
RUN apt-get install -y nginx #安装web服务
RUN echo 'hi, I am in your container' > /usr/share/nginx/html/index.html
EXPOSE 80  #开放80口
```
使用docker build命令构建镜像

```shell
docker build -t="roubin/static_web" . #最后的点表示从本地寻找dockerfile文件
docker build -t="roubin/static_web" git@github.com:roubin/static_web #从github寻找dockerfile文件
docker build --no-cache -t="roubin/static_web" . #重新构建镜像时不使用之前的缓存
```
基于缓存构建的dockerfile模板

```shell
FROM ubuntu  #基础镜像
MAINTAINER roubin "test@example.com"  #作者和联系方式
ENV REFRESHED_AT 2015-07-01 #创建环境变量REFRESHED_AT,下次构建前更改最新时间，则后续更新包命令也会得以执行
RUN apt-get -qq update
```

查看新image的构建历史指令

```shell
docker history 22d47c8cb65
```

启用基于新镜像的容器，并开启端口

```shell
docker run -d -p 8080:80 --name webserver roubin/static_web nginx -g "daemon off;" #宿主机8080端口与容器80口绑定
docker run -d -P --name webserver roubin/static_web nginx -g "daemon off;" #宿主机随机的一个端口与容器80口绑定
```

查看端口映射
```shell
docker port webserver
```

# Dockerfile详解

利用Dockerfile文件，可以构建docker的image镜像

## 命令使用

通过-f参数指定Dockerfile路径，进行构建image
```shell
docker build -f /path/to/a/Dockerfile . 
```
或者，Dockerfile在当前目录，使用
```shell
docker build  . 
```
使用-t参数，指定image名称

```shell
docker build -t shykes/myapp:1.0.2 -t shykes/myapp:latest .
```

## Dockerfile 说明

- 内容大小写不敏感
- 指令是按顺序执行
- 第一条指令是FROM
- `#`后面跟的是说明内容

## FROM 指令

关于FROM命令说明
必须指定且需要在Dockerfile其他指令的前面。后续的指令都依赖于该指令指定的image。FROM指令指定的基础image可以是官方远程仓库中的，也可以位于本地仓库。
1.FROM必需是除了注释语句外的第一行
2.可以用多个FROM命令，用来创建多个image
3.tag or digest是可选的，如果不输入，则默认使用latest
4.如果找不到tag或者digest则返回错误
命令格式如下：

- FROM <image>
- FROM <image>:<tag>
- FROM <image>@<digest>

## ENV环境变量

构建指令，在image中设置一个环境变量。
设置了后，后续的RUN命令都可以使用，container启动后，可以通过docker inspect查看这个环境变量，也可以通过在docker run --env key=value时设置或修改环境变量。

- ENV <key> <value>
- ENV <key>=<value> ...

eg:示例
```dockerfile
ENV foo /bar
WORKDIR ${foo}   # WORKDIR /bar
ADD . $foo       # ADD . /bar
COPY \$foo /quux # COPY $foo /quux

ENV abc=hello
ENV abc=bye def=$abc
ENV ghi=$abc
```
以上命令def = hello, ghi=bye

`$`变量可被以下命令使用

- ADD
- COPY
- ENV
- EXPOSE
- LABEL
- USER
- WORKDIR
- VOLUME
- STOPSIGNAL

## .dockerignore 文件

作用：排除某些文件或文件夹不包含在构建的image中
内容格式如下
```text
*/temp*    #For example, the plain file /somedir/temporary.txt is excluded, as is the directory /somedir/temp.
*/*/temp*  #For example, /somedir/subdir/temporary.txt is excluded.
temp?      #For example, /tempa and /tempb are excluded.
```
以下是除了README.md外，其他所有的md文件都不包含进去
```text
*.md
!README.md
```
以下所有的README文件将被包含，其他md文件不包含，其中第二行无效，因为第三行会匹配第二行
*.md
README-secret.md
!README*.md

## MAINTAINER 指令

维护人员信息说明,用于将image的制作者相关的信息写入到image中。当我们对该image执行docker inspect命令时，输出中有相应的字段记录该信息。

- MAINTAINER <name>

## RUN 指令

RUN命令用于执行一些命令，如安装软件等
RUN可以运行任何被基础image支持的命令。如基础image选择了ubuntu，那么软件管理部分只能使用ubuntu的命令。
RUN命令的结果会被缓存，供下一个指令使用
如果不需要缓存，使用docker build --no-cache=true

### shell模式

- RUN <command> (shell form, the command is run in a shell - /bin/sh -c)

使用用`\`换行
```dockerfile
RUN /bin/bash -c 'source $HOME/.bashrc ;\
echo $HOME'
```
等价于
```dockerfile
RUN /bin/bash -c 'source $HOME/.bashrc ; echo $HOME'
```


### exec模式

使用的是JSON array格式，必需使用双引号。
```dockerfile
RUN ["executable", "param1", "param2"] (exec form)
RUN ["/bin/bash", "-c", "echo hello"]
RUN [ "sh", "-c", "echo", "$HOME" ]
```
> 命令及参数使用双引号

## CMD 指令

只能有一个CMD指令，如果有多个，最后一个将被生效

When used in the shell or exec formats, the CMD instruction sets the command to be executed when running the image.
CMD使用shell,或者exec方式，则运行容器的时候将运行命令

The main purpose of a CMD is to provide defaults for an executing container. These defaults can include an executable, or they can omit the executable, in which case you must specify an ENTRYPOINT instruction as well.

一个CMD的主要目的是为执行容器提供默认值。这些设置可以包括一个可执行文件，也可以不执行，在这种情况下，您必须指定一个入口指令以及。

CMD指令有3种使用方式

### exec方式，优先

- CMD ["executable","param1","param2"]
如果不想用shell方式执行命令，必需采用 exec json array方式（命令及参数使用双引号），如：
- CMD ["/usr/bin/wc","--help"]

### 为ENTRYPOINT传递参数
当Dockerfile指定了ENTRYPOINT，那么使用下面的格式：
- CMD ["param1","param2"] (as default parameters to ENTRYPOINT)
ENTRYPOINT指定的是一个可执行的脚本或者程序的路径，该指定的脚本或者程序将会以param1和param2作为参数执行。所以如果CMD指令使用上面的形式，那么Dockerfile中必须要有配套的ENTRYPOINT。

### shell方式

- CMD command param1 param2 (shell form)
使用以下CMD命令，将以/bin/sh -c 方式运行命令
- CMD echo "This is a test." | wc -

如果使用docker run 指定了参数，将重写CMD的默认值

注意：不要混淆RUN 命令和 CMD。
RUN 在构建 IMAGE 时会运行命令，并提交结果
CMD 在启动IMAGE时执行

## LABEL 指令

指定image标签，如版本，说明等
```dockerfile
LABEL "com.example.vendor"="ACME Incorporated"
LABEL com.example.label-with-value="foo"
LABEL version="1.0"
LABEL description="This text illustrates that label-values can span multiple lines."
```
如果有多个标签，推荐使用单一的LABEL来说明
```dockerfile
LABEL multi.label1="value1" multi.label2="value2" other="value3"
# 或者
LABEL multi.label1="value1" \
      multi.label2="value2" \
      other="value3"
```

使用`docker inspect`查看`label`

## EXPOSE 指令

设置指令，该指令会将容器中的端口映射成宿主机器中的某个端口。当你需要访问容器的时候，可以不是用容器的IP地址而是使用宿主机器的IP地址和映射后的端口。要完成整个操作需要两个步骤，首先在Dockerfile使用EXPOSE设置需要映射的容器端口，然后在运行容器的时候指定-p选项加上EXPOSE设置的端口，这样EXPOSE设置的端口号会被随机映射成宿主机器中的一个端口号。也可以指定需要映射到宿主机器的那个端口，这时要确保宿主机器上的端口号没有被使用。EXPOSE指令可以一次设置多个端口号，相应的运行容器的时候，可以配套的多次使用-p选项。

- EXPOSE <port> [<port>...]
映射一个端口  
- EXPOSE port1  
相应的运行容器使用的命令
```shell
docker run -p port1 image  
```
映射多个端口  
- EXPOSE port1 port2 port3  
相应的运行容器使用的命令  
```shell
docker run -p port1 -p port2 -p port3 image
```
还可以指定需要映射到宿主机器上的某个端口号
```shell
docker run -p host_port1:port1 -p host_port2:port2 -p host_port3:port3 image  
```
对于一个运行的容器，可以使用docker port加上容器中需要映射的端口和容器的ID来查看该端口号在宿主机器上的映射端口。

## ADD指令

构建指令，所有拷贝到container中的文件和文件夹权限为0755，uid和gid为0；如果是一个目录，那么会将该目录下的所有文件添加到container中，不包括目录；如果文件是可识别的压缩格式，则docker会帮忙解压缩（注意压缩格式）；如果是文件且中不使用斜杠结束，则会将视为文件，的内容会写入；如果是文件且中使用斜杠结束，则会文件拷贝到目录下。

## COPY指令

指令说明：从src拷备文件或目录到容器中的dest路径

- COPY <src>... <dest>
- COPY ["<src>",... "<dest>"]

注意事项说明：
1.可以使用多个COPY指令
2.每个<src>可以包含通配符
```dockerfile
COPY hom* /mydir/        # adds all files starting with "hom"
COPY hom?.txt /mydir/    # ? is replaced with any single character, e.g., "home.txt"
```

3.<src>是相对被构建的源目录的相对路径，可以是文件或目录的路径，也可以是一个远程的文件url;
不能使用如../路径
```dockerfile
COPY ../something /something
```

4.dest 必需是容器中的绝对路径，或者相对于WORKDIR的路径，如
```dockerfile
COPY test relativeDir/   # adds "test" to `WORKDIR`/relativeDir/
COPY test /absoluteDir/  # adds "test" to /absoluteDir/
```

5.如果是一个目录，那么会将该目录下的所有文件添加到container中，不包括目录本身
6.如果是文件且中不使用斜杠结束，则会将视为文件，的内容会写入
7.如果是文件且中使用斜杠结束，则会文件拷贝到目录下。
8.如果src使用了通配符，则dest必需是以/结尾的目录

## ENTRYPOINT指令

指令格式：

- ENTRYPOINT ["executable", "param1", "param2"] (exec form, preferred)
- ENTRYPOINT command param1 param2 (shell form)

指令说明：设置指令，指定容器启动时执行的命令，可以多次设置，但是只有最后一个有效。
该指令的使用分为两种情况，一种是独自使用，另一种和CMD指令配合使用。

当独自使用时，如果你还使用了CMD命令且CMD是一个完整的可执行的命令，那么CMD指令和ENTRYPOINT会互相覆盖只有最后一个CMD或者ENTRYPOINT有效。

```dockerfile
# CMD指令将不会被执行，只有ENTRYPOINT指令被执行  
CMD echo “Hello, World!”  
ENTRYPOINT ls -l  
```

另一种用法和CMD指令配合使用来指定ENTRYPOINT的默认参数，这时CMD指令不是一个完整的可执行命令，仅仅是参数部分；ENTRYPOINT指令只能使用JSON方式指定执行命令，而不能指定参数。

```dockerfile
FROM ubuntu  
CMD ["-l"]  
ENTRYPOINT ["/usr/bin/ls"]  
```

docker run image命令的参数，将会追加到ENTRYPOINT命令以exec方式的参数之后，并且会覆盖所有CMD指令的参数，可以使用--entrypoint参数覆盖ENTRYPOINT命令的参数。

如果使用shell方式执行，则所有的CMD命令，还有以docker run 方式传递的参数无效，这样有一个缺点，ENTRYPOINT将会以子命令/bin/sh -c方式启动，无法接收Unix信号，意味着无法使用docker stop <container>方式停止容器。

## VOLUME指令

指令格式

- VOLUME ["/data"]
- VOLUME /var/log
- VOLUME /var/log /var/db

设置指令，使容器中的一个目录具有持久化存储数据的功能，该目录可以被容器本身使用，也可以共享给其他容器使用。我们知道容器使用的是AUFS，这种文件系统不能持久化数据，当容器关闭后，所有的更改都会丢失。当容器中的应用有持久化数据的需求时可以在Dockerfile中使用该指令。

## USER指令

指令格式：

- USER daemon
- USER UID

指令说明

设置指令，设置启动容器的用户，默认是root用户。RUN、CMD、ENTRYPOINT相关命令会以该用户运行

## WORKDIR指令

指令格式

- WORKDIR /path/to/workdir

设置指令，可以多次切换(相当于cd命令)，对RUN,CMD,ENTRYPOINT,COPY,ADD生效。如果不存在会创建，可以设置多次，每次设置相对于上次路径，如：

```dockerfile
WORKDIR /a
WORKDIR b
WORKDIR c
RUN pwd
```

最终pwd结果是`/a/b/c`。

WORKDIR指令也可以使用环境变量的值：

```dockerfile
ENV DIRPATH /path
WORKDIR $DIRPATH/$DIRNAME
RUN pwd
```
最终pwd结果是/path/$DIRNAME

## ARG指令

设置变量，在docker build创建IMAGE时用--build-arg参数，指定一些参数，如
```shell
FROM busybox
USER ${user:-some_user}
ARG user
USER $user
```

使用如下指令后，user=what_user，默认user=some_user,

```shell
docker build --build-arg user=what_user Dockerfile
```