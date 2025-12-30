# Frida 运行参数详解

## 1. **frida 命令基础参数**

### 目标选择参数
```bash
# 附加到正在运行的进程
-p, --attach-pid PID            # 附加到指定PID的进程
-n, --attach-name NAME          # 附加到指定名称的进程
--attach-uid UID                # 附加到指定UID的进程

# 启动新进程
-f, --file PROGRAM              # 启动程序并注入
-f PROGRAM --args ARG1 ARG2     # 带参数启动程序
--stdio=inherit|pipe            # 控制标准IO
--aux option=value              # 设置辅助选项
```

### 设备连接参数
```bash
-U, --usb                       # 连接到USB设备（Android）
-R, --remote ADDR:PORT          # 连接到远程frida-server
-D ID, --device ID              # 指定设备ID
-h HOST, --host HOST            # 连接到指定主机
-P PORT, --port PORT            # 指定端口（默认27042）
--certificate CERTIFICATE       # TLS证书
--origin ORIGIN                 # WebSocket源
--token TOKEN                   # 认证令牌
--keepalive-interval SECONDS    # 保持连接间隔
```

### 脚本控制参数
```bash
-l SCRIPT, --load-script SCRIPT      # 加载JavaScript文件
-e CODE, --eval CODE                  # 执行单行JavaScript代码
--no-pause                            # 不暂停启动的进程
--pause                               # 启动后暂停进程（默认）
--debug                               # 启用Node.js兼容调试器
--enable-jit                          # 启用JavaScript JIT编译器
--disable-jit                         # 禁用JavaScript JIT编译器
--runtime=qjs|v8                      # 选择JavaScript运行时
--bytecode                            # 脚本是字节码
--optimize                            # 启用字节码优化
```

### 输出和控制参数
```bash
-O, --options-file FILE        # 从文件读取选项
-o LOGFILE, --output LOGFILE   # 输出到文件
--json                         # JSON格式输出
-q, --quiet                    # 静默模式（不显示提示）
--no-color                     # 禁用颜色输出
--colors                       # 启用颜色输出
-v, --version                  # 显示版本信息
```

## 2. **frida-trace 特定参数**

```bash
# 函数追踪参数
-i FUNCTION, --include FUNCTION    # 包含函数（支持通配符）
-x FUNCTION, --exclude FUNCTION    # 排除函数
-I MODULE, --include-module MODULE # 包含模块
-X MODULE, --exclude-module MODULE # 排除模块

# 方法追踪（平台特定）
-m OBJC_METHOD, --include-objc-method OBJC_METHOD    # 包含ObjC方法
-M OBJC_METHOD, --exclude-objc-method OBJC_METHOD    # 排除ObjC方法
-j JAVA_METHOD, --include-java-method JAVA_METHOD    # 包含Java方法
-J JAVA_METHOD, --exclude-java-method JAVA_METHOD    # 排除Java方法

# 地址追踪
-a MODULE!OFFSET, --add MODULE!OFFSET  # 追踪指定地址

# 高级追踪选项
-T, --include-imports              # 追踪所有导入函数
-t, --include-module-entries       # 追踪模块入口点
-d, --decorate                     # 显示调用次数和时间戳
```

## 3. **frida-ps 参数**

```bash
-a, --applications        # 列出应用程序
-i, --installed           # 列出已安装应用
-e, --environment         # 显示环境信息
--json                    # JSON格式输出
-U, --usb                 # USB设备
-R, --remote              # 远程设备
```

## 4. **frida-ls-devices 参数**

```bash
--json                    # JSON格式输出
-v, --verbose             # 详细输出
```

## 5. **frida-discover 参数**

```bash
-f FILE, --file FILE      # 要分析的文件
-o FILE, --output FILE    # 输出文件
--json                    # JSON格式输出
```

## 6. **frida-kill 参数**

```bash
-f, --file PROGRAM        # 终止指定程序的所有实例
-p PID, --pid PID         # 终止指定PID
-n NAME, --name NAME      # 终止指定名称的进程
-U, --usb                 # USB设备
```

## 7. **环境变量参数**

```bash
# 设置环境变量
FRIDA_VERBOSE=1                    # 启用详细输出（0-3级别）
FRIDA_TRANSPORT=usb|tcp|local      # 指定传输方式
FRIDA_AGENT_STRING=...             # 自定义User-Agent
FRIDA_CORE_DEVICE=...              # 核心设备设置
FRIDA_REALM=native|emulated        # 指定领域
FRIDA_ARCH=auto|x86|x64|arm|arm64  # 指定架构
FRIDA_PLATFORM=...                 # 指定平台
FRIDA_DISABLE_ABI_CHECK=1          # 禁用ABI检查
```

## 8. **高级使用示例**

### 组合参数示例
```bash
# 复杂追踪示例
frida-trace \
  -U \                          # USB设备
  -n "com.example.app" \        # 应用名称
  -i "open*" \                  # 追踪所有open开头的函数
  -I "libc.so*" \               # 只追踪libc模块
  -x "openat" \                 # 排除openat函数
  -o trace.log \                # 输出到文件
  --json                        # JSON格式

# 多脚本注入
frida -p 1234 \
  -l hook1.js \
  -l hook2.js \
  -e "console.log('Inline code')" \
  --no-pause

# 远程调试
frida -R 192.168.1.100:27042 \
  -n target_process \
  -l script.js \
  --debug                       # 启用调试器

# 带参数启动程序
frida -f /usr/bin/ssh \
  --args user@hostname \
  -l monitor.js \
  --stdio=inherit
```

### 配置文件使用
```bash
# 创建配置文件 options.txt
cat > options.txt << EOF
-l script1.js
-l script2.js
-e "send('started')"
--no-pause
EOF

# 使用配置文件
frida -p 1234 -O options.txt
```

### 条件注入
```bash
# 只在特定条件下注入
frida -f /path/to/app \
  --aux=uid=1000 \
  --aux=gid=1000 \
  -l conditional.js
```

## 9. **调试和分析参数**

```bash
# 内存调试
--dump-module-map              # 导出模块映射
--dump-memory-snapshot         # 导出内存快照
--dump-heap                    # 导出堆信息

# 性能分析
--cpu-profile                  # CPU性能分析
--cpu-profiler-interval MS     # 分析间隔（毫秒）

# 安全检查
--enable-sandbox               # 启用沙箱
--disable-sandbox              # 禁用沙箱
--validate-script              # 验证脚本安全性
```

## 10. **实用组合技巧**

### 批量操作
```bash
# 批量追踪多个函数
frida-trace -p 1234 \
  $(echo "-i open -i read -i write -i close")

# 使用文件包含函数列表
cat functions.txt
# 内容：
# -i malloc
# -i free
# -i strcmp

frida-trace -p 1234 $(cat functions.txt)
```

### 自动化脚本
```bash
#!/bin/bash
# 自动化分析脚本
PID=$1

frida -p $PID -l init.js --no-pause &
sleep 2
frida-trace -p $PID -i "open*" -o "trace_$PID.log" &
wait
```

### 网络分析专用
```bash
# 网络相关函数追踪
frida-trace -p $PID \
  -i "connect" \
  -i "send" \
  -i "recv" \
  -i "socket" \
  -I "libssl*" \
  -I "libcrypto*"
```

## 11. **故障排除参数**

```bash
# 详细日志
FRIDA_VERBOSE=3 frida -p 1234

# 重置连接
frida -p 1234 --kill

# 清理缓存
frida -p 1234 --clear-cache

# 安全模式
frida -p 1234 --safe-mode
```

## 12. **平台特定参数**

### Android 专用
```bash
# 启动Activity
frida -U -f com.example.app \
  --activity=MainActivity \
  -l script.js

# 使用特定启动器
frida -U -f com.example.app \
  --startup=warm \
  -l script.js
```

### iOS 专用
```bash
# 注入SpringBoard
frida -U -n SpringBoard \
  -l tweak.js

# 使用特定注入方法
frida -U -f com.example.app \
  --inject-early \
  -l script.js
```

这些参数可以灵活组合，满足不同的调试和分析需求。建议根据具体场景选择合适的参数组合。
