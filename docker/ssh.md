## 开启 SSH 服务的 Dockerfile：

```Dockerfile
# 使用 Ubuntu 24.04 LTS (Noble Numbat) 作为基础镜像
FROM ubuntu:noble

# 设置环境变量避免交互式提示
ENV DEBIAN_FRONTEND=noninteractive

# 更新软件包列表并安装必要的软件
RUN apt-get update && \
    apt-get install -y \
        openssh-server \
        sudo \
        curl \
        wget \
        vim \
        net-tools \
        iputils-ping \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# 创建 SSH 目录并生成主机密钥
RUN mkdir /var/run/sshd && \
    ssh-keygen -A

# 设置 root 密码（可选，建议在生产环境中使用更安全的方式）
RUN echo 'root:password' | chpasswd

# 允许 root SSH 登录（生产环境建议禁用）
RUN sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config

# 保持 SSH 服务在后台运行
RUN sed -i 's@session\s*required\s*pam_loginuid.so@session optional pam_loginuid.so@g' /etc/pam.d/sshd

# 创建数据目录和配置目录作为卷
RUN mkdir -p /data && \
    mkdir -p /config && \
    mkdir -p /logs

# 定义数据卷
VOLUME ["/data", "/config", "/logs"]

# 暴露 SSH 端口
EXPOSE 22

# 启动 SSH 服务
CMD ["/usr/sbin/sshd", "-D"]
```

## 更完整的版本（包含用户创建和权限设置）

```Dockerfile
# 使用 Ubuntu 24.04 LTS (Noble Numbat) 作为基础镜像
FROM ubuntu:noble

# 设置环境变量避免交互式提示
ENV DEBIAN_FRONTEND=noninteractive

# 更新软件包列表并安装必要的软件
RUN apt-get update && \
    apt-get install -y \
        openssh-server \
        sudo \
        curl \
        wget \
        vim \
        net-tools \
        iputils-ping \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# 创建 SSH 目录并生成主机密钥
RUN mkdir /var/run/sshd && \
    ssh-keygen -A

# 创建普通用户并设置密码
RUN useradd -m -s /bin/bash -G sudo developer && \
    echo 'developer:password' | chpasswd && \
    echo 'root:password' | chpasswd

# 允许 root SSH 登录（生产环境建议禁用）
RUN sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config

# 保持 SSH 服务在后台运行
RUN sed -i 's@session\s*required\s*pam_loginuid.so@session optional pam_loginuid.so@g' /etc/pam.d/sshd

# 创建多个数据卷目录
RUN mkdir -p /data && \
    mkdir -p /config && \
    mkdir -p /logs && \
    mkdir -p /app && \
    mkdir -p /backup

# 设置卷目录权限
RUN chown -R developer:developer /data /app && \
    chmod 755 /data /config /logs /app /backup

# 定义数据卷
VOLUME ["/data", "/config", "/logs", "/app", "/backup"]

# 设置工作目录
WORKDIR /app

# 暴露 SSH 端口
EXPOSE 22

# 启动 SSH 服务
CMD ["/usr/sbin/sshd", "-D"]
```

## 构建和运行说明

1. **构建镜像**：
```bash
docker build -t ubuntu-ssh-volumes:noble .
```

2. **运行容器（使用命名卷）**：
```bash
# 使用命名卷
docker run -d \
  -p 2222:22 \
  --name ubuntu-ssh \
  -v app_data:/data \
  -v app_config:/config \
  -v app_logs:/logs \
  ubuntu-ssh-volumes:noble
```

3. **运行容器（使用主机目录挂载）**：
```bash
# 使用主机目录挂载
docker run -d \
  -p 2222:22 \
  --name ubuntu-ssh \
  -v /host/path/data:/data \
  -v /host/path/config:/config \
  -v /host/path/logs:/logs \
  ubuntu-ssh-volumes:noble
```

4. **查看卷信息**：
```bash
# 查看所有卷
docker volume ls

# 查看容器卷信息
docker inspect ubuntu-ssh | grep -A 10 Mounts
```

## 高级版本（包含初始化脚本）

```Dockerfile
FROM ubuntu:noble

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y \
        openssh-server \
        sudo \
        curl \
        wget \
        vim \
        net-tools \
        iputils-ping \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

RUN mkdir /var/run/sshd && \
    ssh-keygen -A

# 创建用户
RUN useradd -m -s /bin/bash -G sudo developer && \
    echo 'developer:password' | chpasswd && \
    echo 'root:password' | chpasswd

RUN sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config && \
    sed -i 's@session\s*required\s*pam_loginuid.so@session optional pam_loginuid.so@g' /etc/pam.d/sshd

# 创建卷目录
RUN mkdir -p /data /config /logs /app /backup

# 设置权限
RUN chown -R developer:developer /data /app && \
    chmod 755 /data /config /logs /app /backup

# 创建初始化脚本
RUN echo '#!/bin/bash\n\
# 初始化卷目录\n\
if [ ! -f /config/initialized ]; then\n\
    echo "Initializing volumes..."\n\
    # 复制默认配置\n\
    cp -r /default-config/* /config/ 2>/dev/null || true\n\
    touch /config/initialized\n\
    echo "Volume initialization completed at $(date)" > /logs/init.log\n\
fi\n\
# 启动 SSH 服务\n\
exec /usr/sbin/sshd -D' > /init.sh && \
    chmod +x /init.sh

# 创建默认配置目录
RUN mkdir -p /default-config && \
    echo "Default configuration" > /default-config/README.txt

# 定义数据卷
VOLUME ["/data", "/config", "/logs", "/app", "/backup"]

WORKDIR /app

EXPOSE 22

# 使用初始化脚本启动
CMD ["/init.sh"]
```

