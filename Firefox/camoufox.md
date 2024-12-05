
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


## `fingerprint-injection.patch`
### navigator.
- userAgent
- appCodeName
appVersion
appName
languages
language
platform
oscpu
product
productSub
pdfViewerEnabled
cookieEnabled
onLine
buildID
doNotTrack
globalPrivacyControl
hardwareConcurrency
maxTouchPoints
globalPrivacyControl

### window.
innerWidth
innerHeight
outerWidth
outerHeight
screenX
screenY
devicePixelRatio
scrollMinX
scrollMinY
scrollMaxX
scrollMaxY
history.length

### screen.
pageXOffset
pageYOffset
colorDepth
pixelDepth
screen.availLeft, screen.availTop, screen.availWidth, screen.availHeight

### battery:
charging
dischargingTime
chargingTime
level

## `network-patches.patch`
### headers.
User-Agent
Accept-Language
Accept-Encoding

### locale:
language
region
script

`timezone-spoofing.patch`
### timezone

## `screen-hijacker.patch`
### screen.
height

## `audio-context-spoofing.patch`

### AudioContext:
sampleRate
outputLatency
maxChannelCount

## `font-hijacker.patch`
fonts

## `geolocation-spoofing.patch`
### geolocation:
latitude
longitude
accuracy

## `webgl-spoofing.patch`
### webGl: / webGl2:
renderer
vendor
shaderPrecisionFormats:blockIfNotDefined
supportedExtensions

### webGl2:contextAttributes / webGl:contextAttributes
`GetAttribute` 获取
{
	alpha
	depth
	stencil
	antialias
}
webGl:contextAttributes.premultipliedAlpha
preserveDrawingBuffer
failIfMajorPerformanceCaveat
powerPreference

### webGl2:parameters / webGl:parameters
`MParamGL` 获取
- ret[LOCAL_GL_BLEND] = false;
- ret[LOCAL_GL_CULL_FACE] = false;
- ret[LOCAL_GL_DEPTH_TEST] = false;
- ret[LOCAL_GL_DITHER] = true;
- ret[LOCAL_GL_POLYGON_OFFSET_FILL] = false;
- ret[LOCAL_GL_SAMPLE_ALPHA_TO_COVERAGE] = false;
- ret[LOCAL_GL_SAMPLE_COVERAGE] = false;
- ret[LOCAL_GL_SCISSOR_TEST] = false;
- ret[LOCAL_GL_STENCIL_TEST] = false;
- ret[LOCAL_GL_RASTERIZER_DISCARD] = false;

`GLParam`
- LOCAL_GL_DEPTH_RANGE:
- LOCAL_GL_ALIASED_POINT_SIZE_RANGE:
- LOCAL_GL_ALIASED_LINE_WIDTH_RANGE:
- LOCAL_GL_COLOR_CLEAR_VALUE:
- LOCAL_GL_BLEND_COLOR:
- LOCAL_GL_MAX_VIEWPORT_DIMS:
- LOCAL_GL_SCISSOR_BOX:
- LOCAL_GL_VIEWPORT:
- LOCAL_GL_COMPRESSED_TEXTURE_FORMATS:
- LOCAL_GL_COLOR_WRITEMASK:
- dom::WEBGL_debug_renderer_info_Binding::UNMASKED_RENDERER_WEBGL:
- dom::WEBGL_debug_renderer_info_Binding::UNMASKED_VENDOR_WEBGL:

## `webrtc-ip-spoofing.patch`
### webrtc:
ipv4
ipv6
