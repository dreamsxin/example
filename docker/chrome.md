```shell
docker run -d -p 2222:22 --name ubuntu-ssh -v app_data:/data dreamsxin/ubuntu-ssh-volumes:noble
```

```shell
sudo apt-get install xvfb
sudo apt-get install libnspr4 libnss3 libatk1.0-0t64 libatk-bridge2.0-0t64 libcups2t64 libxkbcommon0 libxcomposite1 libxdamage1 libxfixes3 libcairo2 libpango-1.0-0 libasound2t64

Xvfb :99 -screen 0 1024x768x16 -nolisten tcp -nolisten unix &
export DISPLAY=:99
```

## 附录

Dockerfile:
```dockerfile
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
RUN echo 'root:123456' | chpasswd

# 允许 root SSH 登录（生产环境建议禁用）
RUN sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config

# 保持 SSH 服务在后台运行
RUN sed -i 's@session\s*required\s*pam_loginuid.so@session optional pam_loginuid.so@g' /etc/pam.d/sshd

# 创建数据目录和配置目录作为卷
RUN mkdir -p /data

# 定义数据卷
VOLUME ["/data"]

# 暴露 SSH 端口
EXPOSE 22

# 启动 SSH 服务
CMD ["/usr/sbin/sshd", "-D"]
```
