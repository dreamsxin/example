
## 使用 Docker 创建镜像编译

### 下载源码构建镜像
```shell
git clone --depth 1 https://github.com/daijro/camoufox
cd camoufox
docker build -t camoufox-builder .
```

### 打补丁

```shell
#!/bin/bash
 
 
for patch in ./patches/*.patch; do
  patch -p1 < "$patch"
done

for patch in ./patches/playwright/*.patch; do
  patch -p1 < "$patch"
done
```

### 编译

```shell
#--target linux windows macos --arch x86_64 arm64 i686
# docker run -v "$(pwd)/dist:/app/dist" camoufox-builder --target <os> --arch <arch>
docker run -v "$(pwd)/dist:/app/dist" camoufox-builder --target windows --arch x86_64

docker ps
docker stop <容器ID或名称> 
docker start <容器ID或名称> 
docker exec -it <容器ID或名称> it
```

## 查看 

```shell
docker volume ls
docker volume inspect
```

## Dockerfile

```conf
FROM ubuntu:latest

WORKDIR /app

# Copy the current directory into the container at /app
COPY . /app

# Install necessary packages
RUN apt-get update && apt-get install -y openssh-server \
    # Mach build tools
    make wget unzip \
    # Python
    python3 python3-dev python3-pip \
    # Camoufox build system tools
    git p7zip-full aria2 \
    # CA certificates
    ca-certificates \
    && update-ca-certificates
RUN echo 'root:123456' |chpasswd

# Fetch Firefox & apply initial patches
# RUN make setup-minimal && \
#     make mozbootstrap && \
#     mkdir /app/dist
RUN make setup-minimal


# Mount .mozbuild directory and dist folder
VOLUME /root/.mozbuild
VOLUME /app/dist

EXPOSE 22
ENTRYPOINT /etc/init.d/ssh start && /bin/bash
```

```shell
docker build -t camoufox-builder .
# 运行容器 -d 后台运行 -t 表示返回一个 tty 终端，-i 表示打开容器的标准输入，使用这个命令可以得到一个容器的 shell 终端 -p 端口映射
sudo docker run -dti --name camoufox -p 22022:22 camoufox-builder -v
```

## 使用 Docker 创建镜像编译

### 下载源码构建镜像
```shell
git clone --depth 1 https://github.com/daijro/camoufox
cd camoufox
docker build -t camoufox-builder .
```

### 打补丁

```shell
#!/bin/bash
 
 
for patch in ./patches/*.patch; do
  patch -p1 < "$patch"
done

for patch in ./patches/playwright/*.patch; do
  patch -p1 < "$patch"
done
```

### 编译

```shell
#--target linux windows macos --arch x86_64 arm64 i686
# docker run -v "$(pwd)/dist:/app/dist" camoufox-builder --target <os> --arch <arch>
docker run -v "$(pwd)/dist:/app/dist" camoufox-builder --target windows --arch x86_64

docker ps
docker stop <容器ID或名称> 
docker start <容器ID或名称> 
docker exec -it <容器ID或名称> it
```

## 查看 

```shell
docker volume ls
docker volume inspect
```

## Dockerfile

```conf
FROM ubuntu:latest

WORKDIR /app

# Copy the current directory into the container at /app
COPY . /app

# Install necessary packages
RUN apt-get update && apt-get install -y openssh-server \
    # Mach build tools
    make wget unzip \
    # Python
    python3 python3-dev python3-pip \
    # Camoufox build system tools
    git p7zip-full aria2 \
    # CA certificates
    ca-certificates \
    && update-ca-certificates
RUN echo 'root:123456' |chpasswd

# Fetch Firefox & apply initial patches
# RUN make setup-minimal && \
#     make mozbootstrap && \
#     mkdir /app/dist
RUN make setup-minimal


# Mount .mozbuild directory and dist folder
VOLUME /root/.mozbuild
VOLUME /app/dist

EXPOSE 22
ENTRYPOINT /etc/init.d/ssh start && /bin/bash
```

```shell
docker build -t camoufox-builder .
# 运行容器 -d 后台运行 -t 表示返回一个 tty 终端，-i 表示打开容器的标准输入，使用这个命令可以得到一个容器的 shell 终端 -p 端口映射
sudo docker run -dti --name camoufox -p 22022:22 camoufox-builder -v
```

## 配置
`CAMOU_CONFIG`
```json
{
	"navigator.userAgent": "Mozilla/15.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/126.0.0.0 Safari/537.36",
	"geolocation:latitude": 116.407957,
	"geolocation:longitude": 39.887764,
	"debug": true
}
```
