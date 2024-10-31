
## 使用 Docker 创建镜像编译

```shell
git clone --depth 1 https://github.com/daijro/camoufox
cd camoufox
docker build -t camoufox-builder
#--target linux windows macos --arch x86_64 arm64 i686
docker run -v "$(pwd)/dist:/app/dist" camoufox-builder --target <os> --arch <arch>
```
