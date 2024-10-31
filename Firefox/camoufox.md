
## 使用 Docker 创建镜像编译

### 下载源码构建镜像
```shell
git clone --depth 1 https://github.com/daijro/camoufox
cd camoufox
docker build -t camoufox-builder
```

### 编译

```shell
#--target linux windows macos --arch x86_64 arm64 i686
# docker run -v "$(pwd)/dist:/app/dist" camoufox-builder --target <os> --arch <arch>
docker run -v "$(pwd)/dist:/app/dist" camoufox-builder --target windows --arch x86_64

docker exec -it <容器ID或名称> python3 ./multibuild.py
```


## 查看 

```shell
docker volume ls
docker volume inspect
```
