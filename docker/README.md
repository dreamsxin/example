# 安装

```shell
sudo apt install docker.io
```

## 编写 Dockerfile

所有`Dockerfile`都必须以FROM命令开始。`FROM`命令会指定镜像基于哪个基础镜像创建，接下来的命令也会基于这个基础镜像。`FROM`命令可以多次使用，表示会创建多个镜像。具体语法如下：
```command
FROM <image name>
```

`ADD` 命令添加文件到镜像。

See: https://github.com/dreamsxin/docker-phalcon

## 根据 Dockerfile 创建镜像

- `-t`	标记来添加 tag，指定新的镜像的用户信息。
- `.`	是 Dockerfile 所在的路径（当前目录），也可以替换为一个具体的 Dockerfile 的路径。

可以用 `docker tag` 命令来修改镜像的标签。

```shell
sudo docker build -t="dreamsxin/phalcon:v1.3" .
```

## 上传镜像

用户可以通过 `docker push` 命令，把自己创建的镜像上传到仓库中来共享。例如，用户在 Docker Hub 上完成注册后，可以推送自己的镜像到仓库中。

```shell
sudo docker push dreamsxin/phalcon
```

## 查看本地镜像

```shell
sudo docker images
sudo docker images dreamsxin/phalcon
```

## 从本地文件系统导入

要从本地文件系统导入一个镜像，可以使用 openvz 的镜像模板来创建：openvz 的模板下载地址为 http://openvz.org/Download/templates/precreated。

比如，先下载了一个 ubuntu-14.04 的镜像模板，之后使用以下命令导入：

```shell
sudo cat ubuntu-14.04-x86_64-minimal.tar.gz  |docker import - ubuntu:14.04
```

## 新建并启动容器

```shell
sudo docker run -ti -p 8080:80 dreamsxin/phalcon:v1.3 -v
```

可以在 docker run 的时候用 `--name` 参数指定容器的名称。

## 停止容器

```shell
docker stop xxxxx
```

## 启动已终止容器

```shell
sudo docker start xxxxx
```

## 进入容器 

```shell
sudo docker attach xxxxx
```

## 容器重命名

```shell
docker rename old容器名  new容器名
```

## 查看运行的容器

```shell
docker ps
docker ps -a
```

## 删除容器

可以使用`docker rm`来删除一个处于终止状态的容器。

```shell
sudo docker rm xxxx
```

## 其他工具

https://github.com/weaveworks/scope

https://github.com/swarmpit/swarmpit
