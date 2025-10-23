以下是使用 Supervisor 启动 Xvfb 和 Chrome 的完整配置：

## 1. 基本 Supervisor 配置

### 方法一：分开配置（推荐）
```ini
; /etc/supervisor/conf.d/xvfb-chrome.conf

; Xvfb 服务
[program:xvfb]
command=Xvfb :99 -screen 0 1920x1080x24 -ac +extension GLX +extension RANDR +extension RENDER
autostart=true
autorestart=true
priority=100
environment=DISPLAY=":99"
user=your-username
stdout_logfile=/var/log/supervisor/xvfb.out.log
stderr_logfile=/var/log/supervisor/xvfb.err.log

; Chrome 服务
[program:chrome]
command=/usr/bin/google-chrome-stable --no-sandbox --disable-dev-shm-usage --disable-gpu --remote-debugging-port=9222 --user-data-dir=/tmp/chrome-profile
autostart=true
autorestart=true
priority=200
environment=DISPLAY=":99",HOME="/tmp"
user=your-username
stdout_logfile=/var/log/supervisor/chrome.out.log
stderr_logfile=/var/log/supervisor/chrome.err.log
depends_on=xvfb
```

### 方法二：组合配置（使用脚本）
```ini
; 使用启动脚本
[program:chrome-with-xvfb]
command=/path/to/start-chrome.sh
autostart=true
autorestart=true
user=your-username
stdout_logfile=/var/log/supervisor/chrome-combined.out.log
stderr_logfile=/var/log/supervisor/chrome-combined.err.log
```

## 2. 启动脚本示例

### start-chrome.sh
```bash
#!/bin/bash

# 启动 Xvfb
Xvfb :99 -screen 0 1920x1080x24 -ac +extension GLX +extension RANDR +extension RENDER &
XVFB_PID=$!

# 设置 DISPLAY
export DISPLAY=:99

# 等待 Xvfb 启动
sleep 2

# 启动 Chrome
/usr/bin/google-chrome-stable \
    --no-sandbox \
    --disable-dev-shm-usage \
    --disable-gpu \
    --remote-debugging-port=9222 \
    --user-data-dir=/tmp/chrome-profile \
    --window-size=1920,1080 \
    --no-first-run \
    --disable-default-apps \
    --disable-translate \
    --disable-extensions \
    --disable-background-networking \
    --disable-sync \
    --metrics-recording-only \
    --safebrowsing-disable-auto-update \
    --disable-client-side-phishing-detection

# 清理
kill $XVFB_PID
```

## 3. Docker 环境中的配置

```ini
[program:xvfb]
command=Xvfb :99 -screen 0 1920x1080x24 -ac
autostart=true
autorestart=true
user=root
environment=DISPLAY=":99"

[program:chrome]
command=/usr/bin/google-chrome-stable --headless --no-sandbox --disable-dev-shm-usage --disable-gpu --remote-debugging-port=9222
autostart=true
autorestart=true
user=root
environment=DISPLAY=":99"
depends_on=xvfb
```

## 4. 无头模式配置（Chrome Headless）

```ini
[program:chrome-headless]
command=/usr/bin/google-chrome-stable --headless --disable-gpu --no-sandbox --remote-debugging-port=9222 --disable-dev-shm-usage
autostart=true
autorestart=true
user=your-username
stdout_logfile=/var/log/supervisor/chrome-headless.out.log
stderr_logfile=/var/log/supervisor/chrome-headless.err.log
; 注意：无头模式不需要 Xvfb
```

## 5. 完整的生产环境配置

```ini
[group:web-browser]
programs=xvfb,chrome

[program:xvfb]
command=Xvfb :99 -screen 0 1920x1080x24 -ac -listen tcp -auth /tmp/xvfb.auth
autostart=true
autorestart=true
startretries=3
user=nobody
environment=DISPLAY=":99"
stdout_logfile=/var/log/supervisor/xvfb.log
stdout_logfile_maxbytes=50MB
stdout_logfile_backups=10
stderr_logfile=/var/log/supervisor/xvfb-error.log
stderr_logfile_maxbytes=50MB
stderr_logfile_backups=10

[program:chrome]
command=/usr/bin/google-chrome-stable 
    --no-sandbox 
    --disable-dev-shm-usage 
    --disable-gpu 
    --remote-debugging-port=9222 
    --user-data-dir=/tmp/chrome-profile 
    --window-size=1920,1080 
    --no-default-browser-check 
    --no-first-run 
    --disable-background-timer-throttling 
    --disable-renderer-backgrounding 
    --disable-backgrounding-occluded-windows
autostart=true
autorestart=true
startretries=3
user=nobody
environment=DISPLAY=":99",HOME="/tmp"
stdout_logfile=/var/log/supervisor/chrome.log
stdout_logfile_maxbytes=50MB
stdout_logfile_backups=10
stderr_logfile=/var/log/supervisor/chrome-error.log
stderr_logfile_maxbytes=50MB
stderr_logfile_backups=10
depends_on=xvfb
```

## 6. 管理命令

```bash
# 重新加载配置
sudo supervisorctl reread
sudo supervisorctl update

# 启动服务
sudo supervisorctl start xvfb chrome

# 查看状态
sudo supervisorctl status

# 重启服务
sudo supervisorctl restart chrome
```

## 7. 重要参数说明

**Xvfb 参数：**
- `:99` - 显示编号
- `-screen 0 1920x1080x24` - 屏幕设置（宽x高x色深）
- `-ac` - 禁用访问控制
- `+extension GLX` - 启用 OpenGL 扩展

**Chrome 参数：**
- `--no-sandbox` - 禁用沙盒（在容器中必需）
- `--disable-dev-shm-usage` - 避免 /dev/shm 大小问题
- `--remote-debugging-port=9222` - 启用远程调试

这种配置确保了 Xvfb 和 Chrome 的高可用性，适合自动化测试、爬虫等场景。
