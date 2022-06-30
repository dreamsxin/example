# 如何使用Docker部署Go Web应用

  >使用 docker 的主要目标是容器化。也就是为你的应用程序提供一致的环境，而不依赖于它运行的主机。
  
## 准备部署的服务

通过 `8888` 端口对外提供服务。

创建项目目录 `goweb`，并编写 `main.go`
```go
package main

import (
	"fmt"
	"net/http"
)

func main() {
	http.HandleFunc("/", hello)
	server := &http.Server{
		Addr: ":8888",
	}
  fmt.Println("server startup...")
	if err := server.ListenAndServe(); err != nil {
		fmt.Printf("server startup failed, err:%v\n", err)
	}
}

func hello(w http.ResponseWriter, _ *http.Request) {
	w.Write([]byte("hello liwenzhou.com!"))
}
```

## 创建 Dockerfile 文件
```Dockerfile
FROM golang:alpine

# 为我们的镜像设置必要的环境变量
ENV GO111MODULE=on \
    CGO_ENABLED=0 \
    GOOS=linux \
    GOARCH=amd64

# 移动到工作目录：/build
WORKDIR /build

# 将 Dockerfile 文件所在目录的代码复制到容器中
COPY . .

# 将我们的代码编译成二进制可执行文件app
RUN go build -o app .

# 移动到用于存放生成的二进制文件的 /dist 目录
WORKDIR /dist

# 将二进制文件从 /build 目录复制到这里
RUN cp /build/app .

# 声明服务端口
EXPOSE 8888

# 启动容器时运行的命令
CMD ["/dist/app"]
```

## 构建镜像并运行

在项目目录下，执行下面的命令创建镜像，并指定镜像名称为goweb_app：
```shell
docker build . -t goweb_app

docker run -p 8888:8888 goweb_app
```

## 分阶段构建示例

Dockerfile:
```Dockerfile
FROM golang:alpine AS builder

# 为我们的镜像设置必要的环境变量
ENV GO111MODULE=on \
    CGO_ENABLED=0 \
    GOOS=linux \
    GOARCH=amd64

# 移动到工作目录：/build
WORKDIR /build

# 将代码复制到容器中
COPY . .

# 将我们的代码编译成二进制可执行文件 app
RUN go build -o app .

###################
# 接下来创建一个小镜像
###################
FROM scratch

# 从builder镜像中把/dist/app 拷贝到当前目录
COPY --from=builder /build/app /

# 需要运行的命令
ENTRYPOINT ["/app"]
```

## 附带其他文件的部署示例

我们需要将templates、static、conf三个文件夹中的内容拷贝到最终的镜像文件中，目录结构如下：
```text
bubble
├── README.md
├── bubble
├── conf
│   └── config.ini
├── controller
│   └── controller.go
├── dao
│   └── mysql.go
├── example.png
├── go.mod
├── go.sum
├── main.go
├── models
│   └── todo.go
├── routers
│   └── routers.go
├── setting
│   └── setting.go
├── static
│   ├── css
│   │   ├── app.8eeeaf31.css
│   │   └── chunk-vendors.57db8905.css
│   ├── fonts
│   │   ├── element-icons.535877f5.woff
│   │   └── element-icons.732389de.ttf
│   └── js
│       ├── app.007f9690.js
│       └── chunk-vendors.ddcb6f91.js
└── templates
    ├── favicon.ico
    └── index.html
```
更新后的 Dockerfile 如下：
```Dockerfile
FROM golang:alpine AS builder

# 为我们的镜像设置必要的环境变量
ENV GO111MODULE=on \
    CGO_ENABLED=0 \
    GOOS=linux \
    GOARCH=amd64

# 移动到工作目录：/build
WORKDIR /build

# 复制项目中的 go.mod 和 go.sum文件并下载依赖信息
COPY go.mod .
COPY go.sum .
RUN go mod download

# 将代码复制到容器中
COPY . .

# 将我们的代码编译成二进制可执行文件 bubble
RUN go build -o bubble .

###################
# 接下来创建一个小镜像
###################
FROM scratch

COPY ./templates /templates
COPY ./static /static
COPY ./conf /conf

# 从builder镜像中把/dist/app 拷贝到当前目录
COPY --from=builder /build/bubble /

# 需要运行的命令
ENTRYPOINT ["/bubble", "conf/config.ini"]
```
* 编译
```shell
docker build . -t bubble_app
```

## 关联其他容器

容器之间可以通过别名联通。

使用如下命令启动一个MySQL容器，挂载容器中的 `/var/lib/mysql` 到本地的 `/local/docker/mysql` 目录；内部服务端口为3306，映射到外部的13306端口。
```shell
docker run --name mysql8019 -p 13306:3306 -e MYSQL_ROOT_PASSWORD=root1234 -v /local/docker/mysql:/var/lib/mysql -d mysql:8.0.19
```
我们这里运行bubble_app容器的时候需要使用--link的方式与上面的mysql8019容器关联起来，具体命令如下
```shell
docker run --link=mysql8019:mysql8019 -p 8888:8888 bubble_app
```

## Docker Compose 模式

除了像上面一样使用 `--link` 的方式来关联两个容器之外，我们还可以使用Docker Compose来定义和运行多个容器。
`Compose` 是用于定义和运行多容器 Docker 应用程序的工具。
通过 Compose，你可以使用 YML 文件来配置应用程序需要的所有服务。然后，使用一个命令，就可以从 YML 文件配置中创建并启动所有服务。

使用Compose基本上是一个三步过程：

- 使用 Dockerfile 定义你的应用环境以便可以在任何地方复制。
- 定义组成应用程序的服务，docker-compose.yml 以便它们可以在隔离的环境中一起运行。
- 执行 `docker-compose up` 命令来启动并运行整个应用程序。

我们的项目需要两个容器分别运行 mysql 和 bubble_app，我们编写的 `docker-compose.yml` 文件内容如下：
```yml
# yaml 配置
version: "3.7"
services:
  mysql8019:
    image: "mysql:8.0.19"
    ports:
      - "33061:3306"
    command: "--default-authentication-plugin=mysql_native_password --init-file /data/application/init.sql"
    environment:
      MYSQL_ROOT_PASSWORD: "root1234"
      MYSQL_DATABASE: "bubble"
      MYSQL_PASSWORD: "root1234"
    volumes:
      - ./init.sql:/data/application/init.sql
  bubble_app:
    # 使用当前目录下的 Dockerfile 构建
    build: .
    command: sh -c "./wait-for.sh mysql8019:3306 -- ./bubble ./conf/config.ini"
    depends_on:
      - mysql8019
    ports:
      - "8888:8888"
```
- https://github.com/Q1mi/deploy_bubble_using_docker/blob/master/wait-for.sh

* bubble_app
使用当前目录下的Dockerfile文件构建镜像，并通过depends_on指定依赖mysql8019服务，声明服务端口8888并绑定对外8888端口。

* mysql8019
mysql8019 服务使用 Docker Hub 的公共 `mysql:8.0.19` 镜像，内部端口3306，外部端口33061。

这里需要注意一个问题就是，我们的 `bubble_app` 容器需要等待 `mysql8019` 容器正常启动之后再尝试启动，因为我们的web程序在启动的时候会初始化MySQL连接。
这里共有两个地方要更改，第一个就是我们Dockerfile中要把最后一句注释掉：
```Dockerfile
FROM golang:alpine AS builder

# 为我们的镜像设置必要的环境变量
ENV GO111MODULE=on \
    CGO_ENABLED=0 \
    GOOS=linux \
    GOARCH=amd64

# 移动到工作目录：/build
WORKDIR /build

# 复制项目中的 go.mod 和 go.sum文件并下载依赖信息
COPY go.mod .
COPY go.sum .
RUN go mod download

# 将代码复制到容器中
COPY . .

# 将我们的代码编译成二进制可执行文件 bubble
RUN go build -o bubble .

###################
# 接下来创建一个小镜像
###################
FROM debian:stretch-slim

COPY ./wait-for.sh /
COPY ./templates /templates
COPY ./static /static
COPY ./conf /conf


# 从builder镜像中把/dist/app 拷贝到当前目录
COPY --from=builder /build/bubble /

RUN set -eux; \
	apt-get update; \
	apt-get install -y \
		--no-install-recommends \
		netcat; \
        chmod 755 wait-for.sh

# 需要运行的命令
# ENTRYPOINT ["/bubble", "conf/config.ini"]
```
# 运行
```shell
docker-compose up
```

## Docker Swarm

Swarm 是 Docker 引擎内置（原生）的集群管理和编排工具。
使用 Swarm 集群之前需要了解以下几个概念。

### 节点
运行 Docker 的主机可以主动初始化一个 Swarm 集群或者加入一个已存在的 Swarm 集群，这样这个运行 Docker 的主机就成为一个 Swarm 集群的节点 (node)。

* 节点分为管理 (manager) 节点和工作 (worker) 节点：

* 管理节点
用于 Swarm 集群的管理，`docker swarm` 命令基本只能在管理节点执行（除了节点退出集群命令 docker swarm leave 可以在工作节点执行）。
一个 Swarm 集群可以有多个管理节点，但只有一个管理节点可以成为 `leader`，通过 `raft` 协议实现。

* 工作节点
用于执行任务，工作节点执行的任务就是由管理节点下发的服务 (service)，管理节点默认也作为工作节点。
你也可以通过配置让服务只运行在管理节点。

### 服务和任务

任务 （Task）是 Swarm 中的最小的调度单位，目前来说就是一个单一的容器。
服务 （Services） 是指一组任务的集合，服务定义了任务的属性。

服务有两种模式：

- replicated services 按照一定规则在各个工作节点上运行指定个数的任务。
- global services 每个工作节点上运行一个任务
两种模式通过 `docker service create` 的 `–mode` 参数指定。

### 创建 Swarm 集群

我们以创建一个包含一个管理节点和两个工作节点的最小 Swarm 集群为例子。提前准备好三个主机。
- 192.168.175.100
- 192.168.175.101
- 192.168.175.102

* 初始化集群
我们使用 `docker swarm init` 在本机初始化一个 Swarm 集群。
避免防火墙在创建集群过程中干扰，建议关掉在实验。

执行 `docker swarm init` 命令的节点自动成为管理节点。
```shell
docker swarm init --advertise-addr 192.168.175.100
```

* 增加工作节点
进入节点所在主机，
```shell
docker swarm join --token SWMTKN-1-xxx-xxxx 192.168.175.100:2377
```

* 查看集群
```shell
docker node ls    //只能在管理节点上执行
```

* 部署服务

新建服务，只能在管理节点运行：
```shell
# –replicas 3 指定任务数量
docker service create --replicas 3 -p 80:80 --name nginx nginx:1.13.7-alpine
```
现在我们使用浏览器，输入任意节点 IP ,即可看到 nginx 默认页面。

* 查看服务

使用 `docker service ls` 来查看当前 Swarm 集群运行的服务。
使用 `docker service ps` 来查看某个服务的详情。
使用 `docker service logs` 来查看某个服务的日志。

* 删除服务

```shell
docker service rm nginx
```

## 在 Swarm 集群中使用 compose 文件

当我们使用 docker service create 一次只能部署一个服务，
使用 `docker-compose.yml` 我们可以一次启动多个关联的服务。
我们以在 Swarm 集群中部署 WordPress 为例进行说明

```yml
version: "3.8"
services:
  wordpress:
    image: wordpress
    ports:
      - 80:80
    networks:
      - overlay
    environment:
      WORDPRESS_DB_HOST: db:3306
      WORDPRESS_DB_USER: wordpress
      WORDPRESS_DB_PASSWORD: wordpress
    deploy:
      mode: replicated
      replicas: 3
 
  db:
    image: mysql
    networks:
       - overlay
    volumes:
      - db-data:/var/lib/mysql
    environment:
      MYSQL_ROOT_PASSWORD: somewordpress
      MYSQL_DATABASE: wordpress
      MYSQL_USER: wordpress
      MYSQL_PASSWORD: wordpress
    deploy:
      placement:
        constraints: [node.role == manager]
 
  visualizer:
    image: dockersamples/visualizer:stable
    ports:
      - "8080:8080"
    stop_grace_period: 1m30s
    volumes:
      - "/var/run/docker.sock:/var/run/docker.sock"
    deploy:
      placement:
        constraints: [node.role == manager]
 
volumes:
  db-data:
networks:
  overlay:
```

* 部署服务

```shell
docker stack deploy -c docker-compose.yml wordpress
```

* 查看
```shell
docker stack ls
```

* 移除服务

```shell
docker stack down wordpress
```
```shell
docker volume rm
```

### 在 Swarm 集群中管理敏感数据

我们可以用 docker secret 命令来管理敏感信息。

```shell
#通过文件创建
docker secret create mysql_root_password ./password
#通过管道的方式创建
echo 'admin123' | docker secret create mysql_root_password -
#查看
docker secret ls
#删除
docker secret rm mysql_root_password
```

secret 默认通过 tmpfs 文件系统挂载到容器的 `/run/secrets` 目录中，我们要用的时候，像下面的写就对了：
```yml
version: "3.8"
services:
  wordpress:
    image: wordpress
    ports:
      - 80:80
    networks:
      - overlay
    environment:
      WORDPRESS_DB_HOST: db:3306
      WORDPRESS_DB_USER: wordpress
      WORDPRESS_DB_PASSWORD_FILE: /run/secrets/wp_db_password
    secrets:
      - wp_db_password
    deploy:
      mode: replicated
      replicas: 3
 
  db:
    image: mysql
    networks:
       - overlay
    volumes:
      - db-data:/var/lib/mysql
    environment:
      MYSQL_ROOT_PASSWORD_FILE: /run/secrets/mysql_root_password
      MYSQL_DATABASE: wordpress
      MYSQL_USER: wordpress
      MYSQL_PASSWORD_FILE: /run/secrets/mysql_password
    secrets:
      - mysql_password
      - mysql_root_password
    deploy:
      placement:
        constraints: [node.role == manager]
 
  visualizer:
    image: dockersamples/visualizer:stable
    ports:
      - "8080:8080"
    stop_grace_period: 1m30s
    volumes:
      - "/var/run/docker.sock:/var/run/docker.sock"
    deploy:
      placement:
        constraints: [node.role == manager]
 
volumes:
  db-data:
networks:
  overlay:
secrets:
  mysql_password:
    external: true
  mysql_root_password:
    external: true
  wp_db_password:
    external: true
```

### 在 Swarm 集群中管理配置数据

如果你没有在 `target` 中显式的指定路径时，默认的 redis.conf 以 tmpfs 文件系统挂载到容器的 /redis.conf。

```shell
docker config create redis.conf  redis.conf
docker config ls
docker service create --name redis --config redis.conf \         #也可以使用此方式 --config source=redis.conf,target=/etc/redis.conf \
     -p 6379:6380 --replicas 3  redis:latest redis-server /redis.conf

```
