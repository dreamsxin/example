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

## 构建和运行说明

1. **构建镜像**：
```bash
docker build -t ubuntu-ssh:noble .
```

2. **运行容器**：
```bash
docker run -d -p 2222:22 --name ubuntu-ssh ubuntu-ssh:noble
```

3. **连接 SSH**：
```bash
ssh root@localhost -p 2222
# 密码: password
```

## 安全增强建议

对于生产环境，建议进行以下改进：

```Dockerfile
# 创建非特权用户
RUN useradd -m -s /bin/bash admin && \
    echo 'admin:securepassword' | chpasswd && \
    usermod -aG sudo admin

# 禁用 root 登录
RUN sed -i 's/PermitRootLogin yes/PermitRootLogin no/' /etc/ssh/sshd_config

# 启用密钥认证
RUN sed -i 's/#PasswordAuthentication yes/PasswordAuthentication no/' /etc/ssh/sshd_config
```

## 使用密钥认证的版本

```Dockerfile
FROM ubuntu:noble

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y openssh-server sudo && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

RUN mkdir /var/run/sshd && \
    ssh-keygen -A

# 创建用户并设置 sudo 权限
RUN useradd -m -s /bin/bash -G sudo admin && \
    echo 'admin ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers

# 配置 SSH
RUN sed -i 's/#PermitRootLogin prohibit-ssh/PermitRootLogin no/' /etc/ssh/sshd_config && \
    sed -i 's/#PasswordAuthentication yes/PasswordAuthentication no/' /etc/ssh/sshd_config && \
    sed -i 's@session\s*required\s*pam_loginuid.so@session optional pam_loginuid.so@g' /etc/pam.d/sshd

# 创建 SSH 密钥目录
RUN mkdir -p /home/admin/.ssh && \
    chown admin:admin /home/admin/.ssh && \
    chmod 700 /home/admin/.ssh

EXPOSE 22

CMD ["/usr/sbin/sshd", "-D"]
```
