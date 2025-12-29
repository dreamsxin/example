# unshare 命令参数详解

`unshare` 是 Linux 命名空间管理的核心工具，用于创建隔离的执行环境。以下是所有参数的详细解释：

## 1. 基本语法
```bash
unshare [选项] [程序 [参数]]
```

## 2. 命名空间选项

### 2.1 进程命名空间 (PID Namespace)
```bash
--pid, -p
```
**功能**：创建独立的进程 ID 空间
**效果**：
- 新命名空间中的 PID 从 1 开始
- 与主机进程隔离
- 需要配合 `--fork` 使用
- 需要配合 `--mount-proc` 正确挂载 /proc

**示例**：
```bash
# 创建 PID 命名空间
unshare --pid --fork --mount-proc /bin/bash
ps aux  # 只能看到命名空间内的进程
```

### 2.2 挂载命名空间 (Mount Namespace)
```bash
--mount, -m
```
**功能**：创建独立的文件系统挂载视图
**效果**：
- 独立的挂载点树
- 可以挂载/卸载而不影响主机
- 常用配合 `--propagation` 控制挂载传播

**传播类型**：
```bash
--propagation <type>  # type 可以是：
# shared:    挂载事件双向传播
# slave:     只接收主命名空间的挂载事件
# private:   不传播也不接收（默认）
# unbindable:私有 + 不能被绑定挂载
```

**示例**：
```bash
# 创建私有挂载命名空间
unshare --mount --propagation private /bin/bash
mount -t tmpfs tmpfs /mnt  # 不影响主机
```

### 2.3 网络命名空间 (Network Namespace)
```bash
--net, -n
```
**功能**：创建独立的网络栈
**效果**：
- 独立的网络接口、路由表、iptables 规则
- 只有 loopback 接口，需要手动配置网络
- 可用于网络隔离或创建虚拟网络

**示例**：
```bash
# 创建网络命名空间
unshare --net /bin/bash
ip link set lo up  # 启用 loopback
ip addr show       # 只有 lo 接口
```

### 2.4 UTS 命名空间 (UTS Namespace)
```bash
--uts, -u
```
**功能**：隔离主机名和域名
**效果**：
- 可以独立设置 hostname 和 domainname
- 不影响主机系统

**示例**：
```bash
unshare --uts /bin/bash
hostname mycontainer  # 只影响当前命名空间
```

### 2.5 IPC 命名空间 (IPC Namespace)
```bash
--ipc, -i
```
**功能**：隔离 System V IPC 和 POSIX 消息队列
**效果**：
- 独立的共享内存、信号量、消息队列
- 进程间通信隔离

**示例**：
```bash
unshare --ipc /bin/bash
ipcs -a  # 查看当前 IPC 资源
```

### 2.6 用户命名空间 (User Namespace)
```bash
--user, -U
```
**功能**：创建独立的用户和组 ID 映射
**效果**：
- 可以在命名空间内拥有 root 权限而不影响主机
- 需要配置 /proc/sys/kernel/unprivileged_userns_clone
- 配合 `--map-root-user` 使用

**映射文件**：
```
# /proc/[pid]/uid_map
# /proc/[pid]/gid_map
```

**示例**：
```bash
# 创建用户命名空间
unshare --user --map-root-user /bin/bash
id  # 显示为 root，但实际权限受限
```

### 2.7 Cgroup 命名空间 (Cgroup Namespace)
```bash
--cgroup, -C
```
**功能**：隔离 cgroup 视图
**效果**：
- 独立的 cgroup 文件系统视图
- 控制资源分配（CPU、内存等）
- 需要内核支持

**示例**：
```bash
unshare --cgroup /bin/bash
cat /proc/self/cgroup  # 查看当前 cgroup
```

## 3. 辅助选项

### 3.1 进程管理
```bash
--fork, -f
```
**功能**：在运行程序前 fork 子进程
**必要场景**：
- 与 `--pid` 一起使用时必须
- 新进程成为 PID 命名空间的 init 进程(pid=1)

**示例**：
```bash
# 错误：没有 --fork
unshare --pid sleep 1000  # 无法看到进程

# 正确：使用 --fork
unshare --pid --fork sleep 1000
```

### 3.2 挂载 /proc
```bash
--mount-proc [=<挂载点>]
```
**功能**：在 PID 命名空间中正确挂载 /proc 文件系统
**默认**：挂载到 /proc
**必要场景**：使用 `--pid` 时必须

**示例**：
```bash
# 错误：没有正确挂载 /proc
unshare --pid --fork /bin/bash
ps aux  # 错误：看到的是主机进程

# 正确：使用 --mount-proc
unshare --pid --fork --mount-proc /bin/bash
ps aux  # 正确：只看到命名空间内的进程
```

### 3.3 设置根目录
```bash
--root=<目录>
```
**功能**：设置根目录（类似 chroot）
**效果**：将指定目录设为新的根文件系统

**示例**：
```bash
# 创建隔离的根环境
mkdir -p /tmp/newroot/{bin,lib}
cp /bin/bash /tmp/newroot/bin/
unshare --mount --root=/tmp/newroot /bin/bash
```

### 3.4 工作目录
```bash
--wd=<目录>
```
**功能**：在新命名空间中设置工作目录

**示例**：
```bash
unshare --mount --wd=/tmp /bin/bash
pwd  # 显示 /tmp
```

### 3.5 设置程序
```bash
--program=<程序>
-- <程序> [参数...]
```
**两种指定程序的方式**：
```bash
# 方式1：使用 --program
unshare --mount --program=/bin/bash

# 方式2：直接放在命令末尾（推荐）
unshare --mount /bin/bash -c "echo hello"

# 方式3：使用 -- 分隔
unshare --mount -- /bin/bash -c "echo hello"
```

## 4. 用户映射选项

### 4.1 用户映射
```bash
--map-user=<uid>
--map-group=<gid>
```
**功能**：在用户命名空间中映射用户/组
**格式**：`<内部ID>:<外部ID>:<数量>`

**示例**：
```bash
# 映射用户
unshare --user --map-user=1000 --map-group=1000 /bin/bash
```

### 4.2 映射根用户
```bash
--map-root-user
--map-auto
```
**功能**：
- `--map-root-user`：自动映射当前用户为命名空间内的 root
- `--map-auto`：自动设置用户/组映射

**示例**：
```bash
# 自动映射
unshare --user --map-root-user /bin/bash
id  # 显示为 root(uid=0)，但权限受限
```

### 4.3 设置 uid/gid
```bash
--setuid=<uid>
--setgid=<gid>
```
**功能**：在新命名空间中设置有效用户/组 ID

**示例**：
```bash
unshare --user --setuid=1000 --setgid=1000 /bin/bash
```

## 5. 网络命名空间高级选项

### 5.1 网络配置文件
```bash
--net=<类型>
```
**类型**：
- `container:<路径>`：加入其他容器的网络命名空间
- `/proc/<pid>/ns/net`：加入指定进程的网络命名空间

**示例**：
```bash
# 加入现有网络命名空间
unshare --net=/proc/1234/ns/net /bin/bash
```

## 6. 环境变量选项

### 6.1 保持环境变量
```bash
--keep-caps
```
**功能**：在用户命名空间中保持 capabilities

### 6.2 设置环境变量
```bash
--kill-child
```
**功能**：当父进程退出时杀死子进程

## 7. 常用组合示例

### 7.1 创建容器式环境
```bash
# 完整隔离（类似容器）
unshare \
    --pid --fork --mount-proc \
    --mount --propagation private \
    --uts --hostname mycontainer \
    --ipc \
    --net \
    --user --map-root-user \
    /bin/bash
```

### 7.2 轻量级沙盒
```bash
# 运行不可信程序
unshare \
    --pid --fork --mount-proc \
    --mount --propagation private \
    --net \
    --ipc \
    /path/to/untrusted-program
```

### 7.3 网络测试环境
```bash
# 创建隔离网络测试
unshare --net --uts /bin/bash
# 配置网络
ip link add veth0 type veth peer name veth1
ip link set veth0 up
ip addr add 192.168.1.1/24 dev veth0
```

### 7.4 文件系统实验
```bash
# 安全地进行文件系统操作
unshare --mount --propagation private /bin/bash
# 安全地测试挂载操作
mount -t tmpfs tmpfs /mnt/test
```

## 8. 实际应用场景

### 8.1 应用隔离运行
```bash
#!/bin/bash
# 隔离运行浏览器
unshare \
    --mount --propagation private \
    --net \
    --ipc \
    firefox --profile /tmp/firefox-profile
```

### 8.2 构建环境隔离
```bash
# 隔离的构建环境
unshare \
    --pid --fork --mount-proc \
    --mount --propagation private \
    --uts \
    make  # 构建过程不会污染主机
```

### 8.3 服务隔离
```bash
# 隔离运行服务
unshare \
    --pid --fork --mount-proc \
    --mount \
    --uts --hostname service1 \
    nginx -g "daemon off;"
```

## 9. 参数组合注意事项

### 9.1 必须的组合
```bash
# PID 命名空间必须配合 --fork
unshare --pid --fork --mount-proc ...

# 用户命名空间可能需要 --map-root-user
unshare --user --map-root-user ...

# 设置主机名需要 UTS 命名空间
unshare --uts --hostname newname ...
```

### 9.2 可能冲突的组合
```bash
# 不要同时使用这些
unshare --pid --pid=file  # 冲突
unshare --user --setuid=0  # 可能无效
```

## 10. 查看和管理命名空间

### 10.1 查看当前命名空间
```bash
# 查看进程所在的命名空间
ls -la /proc/$$/ns/

# 查看命名空间信息
cat /proc/$$/status | grep NStrace
```

### 10.2 进入现有命名空间
```bash
# 使用 nsenter 进入
nsenter --target <pid> --mount --pid --net /bin/bash
```

### 10.3 持久化命名空间
```bash
# 创建持久的命名空间引用
touch /var/run/netns/mynet
mount --bind /proc/$$/ns/net /var/run/netns/mynet

# 稍后使用
ip netns exec mynet ip addr show
```

## 11. Android 特殊注意事项

### 11.1 Android 上的限制
```bash
# Android 的 unshare 可能功能有限
# 需要 root 权限
# 某些命名空间可能不可用

# 检查可用功能
unshare --help | grep -E "(pid|mount|net|uts|ipc|user)"
```

### 11.2 Android 示例
```bash
# Android 上创建基本隔离
su -c 'unshare --mount --pid --fork --mount-proc /system/bin/sh'

# 隐藏目录（需要 root）
su -c 'unshare --mount --propagation private sh -c "mount -o bind /dev/null /target && exec \$@"' -- /system/bin/app_process
```

## 12. 调试和故障排除

### 12.1 调试命名空间
```bash
# 查看进程的命名空间
lsns -p $$

# 跟踪系统调用
strace unshare --pid --fork --mount-proc /bin/bash

# 检查内核日志
dmesg | tail -20
```

### 12.2 常见错误解决

**错误1**：无法创建 PID 命名空间
```bash
# 错误：unshare: unshare failed: Invalid argument
# 解决：必须使用 --fork
unshare --pid --fork --mount-proc ...
```

**错误2**：/proc 显示不正确
```bash
# 错误：ps 显示主机进程
# 解决：使用 --mount-proc
unshare --pid --fork --mount-proc ...
```

**错误3**：权限不足
```bash
# 错误：unshare: unshare failed: Operation not permitted
# 解决：需要 root 或相应的 capabilities
sudo unshare ...
# 或设置内核参数
echo 1 > /proc/sys/kernel/unprivileged_userns_clone
```

## 13. 性能和安全考虑

### 13.1 性能影响
- 命名空间创建开销很小
- 网络命名空间可能需要额外配置
- 用户命名空间映射有额外开销

### 13.2 安全建议
- 使用 `--propagation private` 增强隔离
- 配合 capabilities 限制权限
- 考虑使用 seccomp 过滤系统调用
- 监控资源使用情况

--------------------------

在 Android 上使用 `unshare` 隐藏指定目录的完整方案：

## 1. 基础：使用 mount 命名空间隔离目录

### 方法1：创建私有挂载命名空间
```bash
#!/system/bin/sh
# hide_directory.sh

TARGET_DIR="/data/local/tmp/secret"
HIDE_DIR="/dev/null"  # 或使用空目录

# 创建私有挂载命名空间并隐藏目录
unshare --mount --propagation private sh -c "
    # 绑定挂载空内容到目标目录
    mount -o bind /dev/null '$TARGET_DIR' 2>/dev/null || true
    
    # 或者使用 tmpfs 覆盖
    # mount -t tmpfs tmpfs '$TARGET_DIR'
    
    # 运行目标程序
    exec \$@
" -- "$@"
```

### 方法2：完全隔离的文件系统
```bash
#!/system/bin/sh
# complete_isolation.sh

# 创建临时根文件系统
TEMP_ROOT=$(mktemp -d)
mkdir -p $TEMP_ROOT/{proc,sys,dev,tmp,data}

# 复制必要文件
cp -r /system/bin/sh $TEMP_ROOT/system/bin/
cp -r /system/lib $TEMP_ROOT/system/

# 创建挂载命名空间隐藏目录
unshare --mount --propagation private \
    --pid --fork --mount-proc \
    bash -c "
        # 挂载 proc
        mount -t proc proc /proc
        
        # 隐藏特定目录
        mount -o bind /dev/null '$TARGET_DIR'
        
        # 更改根目录（可选）
        # pivot_root $TEMP_ROOT $TEMP_ROOT/oldroot
        
        exec \$@
    " -- "$@"
```

## 2. 针对 Android 应用的目录隐藏

### 方法1：隐藏应用数据目录
```bash
#!/system/bin/sh
# hide_app_data.sh

PACKAGE_NAME="com.target.app"
APP_DATA="/data/data/$PACKAGE_NAME"

# 在隔离环境中运行应用
unshare --mount --propagation private \
    --uts --hostname "isolated-app" \
    --ipc --net \
    sh -c "
        # 隐藏应用数据目录
        if [ -d '$APP_DATA' ]; then
            # 方法1：绑定挂载空 tmpfs
            mount -t tmpfs tmpfs '$APP_DATA'
            
            # 方法2：使用 overlayfs 隐藏
            # mkdir -p /tmp/upper /tmp/work
            # mount -t overlay overlay -o lowerdir='$APP_DATA',upperdir=/tmp/upper,workdir=/tmp/work '$APP_DATA'
        fi
        
        # 设置 Android 环境
        export ANDROID_DATA='/data'
        export ANDROID_ROOT='/system'
        
        # 启动应用
        am start -n '$PACKAGE_NAME/.MainActivity'
        
        # 或者运行特定命令
        exec app_process -Djava.class.path=/path/to/app /system/bin '$PACKAGE_NAME'
    "
```

### 方法2：使用 unshare 运行命令并隐藏目录
```bash
#!/system/bin/sh
# run_hidden.sh

CMD="$1"
HIDE_DIRS="/data/local/tmp/secret /sdcard/private"

unshare --mount --propagation private sh -c "
    # 隐藏所有指定目录
    for dir in $HIDE_DIRS; do
        if [ -d \"\$dir\" ]; then
            # 挂载空目录
            mkdir -p /tmp/empty_\$(basename \"\$dir\")
            mount -o bind /tmp/empty_\$(basename \"\$dir\") \"\$dir\"
            
            # 或者直接隐藏
            # mount -o bind /dev/null \"\$dir\"
        fi
    done
    
    # 运行命令
    exec $CMD
"
```

## 3. OverlayFS 高级隐藏方案

### 创建 OverlayFS 隐藏目录
```bash
#!/system/bin/sh
# overlay_hide.sh

TARGET_DIR="/data/local/tmp/target"
LOWER_DIR="$TARGET_DIR"          # 原始目录（只读）
UPPER_DIR="/tmp/overlay/upper"   # 修改层
WORK_DIR="/tmp/overlay/work"     # 工作目录
MERGED_DIR="/tmp/overlay/merged" # 合并视图

# 创建目录
mkdir -p $UPPER_DIR $WORK_DIR $MERGED_DIR

# 在私有命名空间中设置 overlay
unshare --mount --propagation private sh -c "
    # 创建 overlay 文件系统
    mount -t overlay overlay -o \
        lowerdir=$LOWER_DIR,\
        upperdir=$UPPER_DIR,\
        workdir=$WORK_DIR \
        $MERGED_DIR
    
    # 绑定到原始位置（隐藏原始内容）
    mount -o bind $MERGED_DIR $TARGET_DIR
    
    # 在 upper 层创建假文件来隐藏真实文件
    echo 'Access Denied' > $UPPER_DIR/secret_file.txt
    
    # 运行程序
    exec \$@
" -- "$@"
```

## 4. 结合多种命名空间的完整方案

### 完整隔离脚本
```bash
#!/system/bin/sh
# android_full_isolation.sh

# 配置
PACKAGE_NAME="$1"
HIDE_DIRS="/data/data/$PACKAGE_NAME /sdcard/Android/data/$PACKAGE_NAME"
TEMP_ROOT="/data/local/tmp/container_$$"

# 创建容器环境
mkdir -p $TEMP_ROOT/{dev,proc,sys,tmp,data,system,apex}

# 复制必要文件
cp -r /system/bin/* $TEMP_ROOT/system/bin/ 2>/dev/null
cp -r /system/lib* $TEMP_ROOT/system/ 2>/dev/null

# 启动完全隔离的环境
unshare --mount --propagation private \
    --pid --fork \
    --uts --hostname "android-container" \
    --ipc --net \
    --cgroup \
    --user \
    --map-root-user \
    sh -c "
        # 设置挂载点
        mount -t proc proc $TEMP_ROOT/proc
        mount -t sysfs sysfs $TEMP_ROOT/sys
        mount -t tmpfs tmpfs $TEMP_ROOT/dev
        mknod $TEMP_ROOT/dev/null c 1 3
        mknod $TEMP_ROOT/dev/zero c 1 5
        mknod $TEMP_ROOT/dev/urandom c 1 9
        
        # 隐藏指定目录
        for hide_dir in $HIDE_DIRS; do
            if [ -d \"\$hide_dir\" ]; then
                # 绑定挂载空 tmpfs
                mount -t tmpfs tmpfs \"\$hide_dir\"
                
                # 或者创建假目录结构
                mkdir -p \"\$hide_dir/files\"
                echo 'Permission denied' > \"\$hide_dir/README.txt\"
            fi
        done
        
        # 设置 Android 环境变量
        export ANDROID_DATA='$TEMP_ROOT/data'
        export ANDROID_ROOT='$TEMP_ROOT/system'
        export PATH=\"\$TEMP_ROOT/system/bin:\$PATH\"
        export LD_LIBRARY_PATH=\"\$TEMP_ROOT/system/lib64:\$TEMP_ROOT/system/lib\"
        
        # 更改根目录（pivot root）
        cd \"\$TEMP_ROOT\"
        pivot_root . old_root
        
        # 卸载旧的根
        umount -l /old_root
        
        # 运行目标程序
        if [ -n \"\$PACKAGE_NAME\" ]; then
            # 启动 Android 应用
            am start -n \"\$PACKAGE_NAME/.MainActivity\"
        else
            # 运行 shell
            exec /system/bin/sh
        fi
    "
```

## 5. 针对特定检测的隐藏

### 隐藏 Magisk 相关目录
```bash
#!/system/bin/sh
# hide_magisk_dirs.sh

# Magisk 相关目录列表
MAGISK_DIRS="
/data/adb
/data/adb/magisk
/data/adb/modules
/sbin/.magisk
/dev/.magisk
"

# 在私有命名空间中运行命令
unshare --mount --propagation private sh -c "
    # 隐藏所有 Magisk 目录
    for dir in $MAGISK_DIRS; do
        if [ -e \"\$dir\" ]; then
            # 方法1：绑定挂载空目录
            mkdir -p /tmp/empty_magisk
            mount -o bind /tmp/empty_magisk \"\$dir\"
            
            # 方法2：直接隐藏
            # mount -o bind /dev/null \"\$dir\"
        fi
    done
    
    # 运行需要隐藏的应用
    exec am start -n 'com.bank.app/.MainActivity'
"
```

### 隐藏 su 相关文件
```bash
#!/system/bin/sh
# hide_su_files.sh

# su 文件可能的位置
SU_FILES="
/system/bin/su
/system/xbin/su
/system/sbin/su
/sbin/su
/vendor/bin/su
/data/local/tmp/su
"

# 创建私有命名空间隐藏 su
unshare --mount --propagation private \
    --pid --fork --mount-proc \
    sh -c "
        # 挂载 proc
        mount -t proc proc /proc
        
        # 隐藏所有 su 文件
        for file in $SU_FILES; do
            if [ -e \"\$file\" ]; then
                # 绑定挂载空设备
                mount -o bind /dev/null \"\$file\"
            fi
        done
        
        # 隐藏 su 命令的输出
        alias su='echo \"sh: su: not found\" && false'
        
        # 运行目标命令
        exec \$@
    " -- "$@"
```

## 6. Android 服务模式

### 作为服务运行隐藏环境
```bash
#!/system/bin/sh
# android_namespace_service.sh

# 创建 Unix socket 用于通信
SOCKET_PATH="/data/local/tmp/namespace_socket"

# 清理旧 socket
rm -f $SOCKET_PATH

# 创建命名空间服务
unshare --mount --propagation private \
    --pid --fork \
    --net --ipc --uts \
    sh -c "
        # 设置主机名
        hostname isolated-namespace
        
        # 隐藏目录
        mount -o bind /dev/null /data/adb
        mount -o bind /dev/null /system/bin/su
        
        # 创建通信 socket
        socat UNIX-LISTEN:$SOCKET_PATH,fork EXEC:'/system/bin/sh',pty,stderr,setsid,sigint,sane &
        
        # 保持运行
        while true; do
            sleep 3600
        done
    " &

# 通过 socket 执行命令
socat_exec() {
    echo "$@" | socat - UNIX-CONNECT:$SOCKET_PATH
}

# 示例：在隔离环境中运行命令
socat_exec "ls -la /data/adb"
socat_exec "am start -n com.example.app/.MainActivity"
```

## 7. 结合 Android 特性的方案

### 使用 wrap.sh 属性（Android 8.0+）
```bash
#!/system/bin/sh
# wrap_app.sh - 放在 /data/local/tmp/

# 获取包名和原始命令
PACKAGE_NAME="$1"
ORIG_CMD="$2"
shift 2

# 创建私有命名空间运行应用
exec unshare --mount --propagation private \
    --pid --fork \
    --uts --hostname "wrapped-$PACKAGE_NAME" \
    sh -c "
        # 隐藏应用特定目录
        APP_DIR=\"/data/data/$PACKAGE_NAME\"
        if [ -d \"\$APP_DIR\" ]; then
            mount -t tmpfs tmpfs \"\$APP_DIR\"
            # 创建必要的子目录
            mkdir -p \"\$APP_DIR/{cache,files,databases,shared_prefs}\"
        fi
        
        # 设置 Android 环境
        export CLASSPATH=\"/system/framework/*\"
        export BOOTCLASSPATH=\"/system/framework/*\"
        
        # 运行原始命令
        exec $ORIG_CMD \"\$@\"
    " -- "$@"
```

### 配置应用使用 wrap.sh
```bash
# 设置 wrap 属性
package="com.target.app"
cmd="app_process -Djava.class.path=/path/to/app /system/bin $package"

# 创建 wrap 脚本
echo "#!/system/bin/sh
exec /data/local/tmp/wrap_app.sh '$package' '$cmd' \"\$@\"" > /data/local/tmp/wrap.$package.sh
chmod 755 /data/local/tmp/wrap.$package.sh

# 设置 wrap 属性（需要 root）
setprop wrap.$package /data/local/tmp/wrap.$package.sh

# 重启应用
am force-stop $package
am start -n $package/.MainActivity
```

## 8. 动态目录隐藏监控

### 监控并动态隐藏目录
```bash
#!/system/bin/sh
# dynamic_hide_monitor.sh

# 监控的目录
WATCH_DIR="/data/data/com.target.app"
HIDDEN_FLAG="/tmp/.hidden_$$"

# 创建初始隐藏
mkdir -p /tmp/empty_dir
mount -o bind /tmp/empty_dir "$WATCH_DIR"

# 在命名空间中运行监控
unshare --mount --propagation private sh -c "
    # 持续监控并隐藏
    while true; do
        # 检查是否有应用试图访问
        if [ -d \"$WATCH_DIR\" ] && ! mountpoint -q \"$WATCH_DIR\"; then
            # 重新隐藏
            mount -o bind /tmp/empty_dir \"$WATCH_DIR\"
            echo \"[$(date)] Re-hidden $WATCH_DIR\"
        fi
        
        # 监控文件访问（使用 inotify）
        if command -v inotifywait >/dev/null; then
            inotifywait -e create,delete,modify -r \"$WATCH_DIR\" 2>/dev/null | \
            while read event; do
                echo \"[$(date)] Access attempt: \$event\"
                # 立即重新隐藏
                umount -l \"$WATCH_DIR\" 2>/dev/null
                mount -o bind /tmp/empty_dir \"$WATCH_DIR\"
            done
        fi
        
        sleep 1
    done
" &

# 运行目标应用
am start -n com.target.app/.MainActivity
```

## 9. 恢复原状

```bash
#!/system/bin/sh
# restore_dirs.sh

# 恢复所有被隐藏的目录
for mount_point in $(mount | grep "bind.*/tmp/empty" | awk '{print $3}'); do
    umount -l "$mount_point"
    echo "Restored: $mount_point"
done

# 恢复特定目录
TARGET_DIR="/data/data/com.target.app"
if mountpoint -q "$TARGET_DIR"; then
    umount "$TARGET_DIR"
fi

# 清理临时文件
rm -rf /tmp/empty_* /tmp/overlay /tmp/container_*
```

## 10. 注意事项和优化

### 性能优化
```bash
# 1. 使用 bind mount 而不是 overlayfs（更轻量）
# 2. 避免过度隔离，只隐藏必要的目录
# 3. 使用 lazy unmount (umount -l) 减少冲突

# 最小化隐藏脚本
unshare -m sh -c "mount -o bind /dev/null /target && exec \$@" -- "$cmd"
```

### 兼容性处理
```bash
# 检查 Android 版本
ANDROID_VERSION=$(getprop ro.build.version.sdk)

# 不同版本的 unshare 功能不同
if [ "$ANDROID_VERSION" -ge 26 ]; then
    # Android 8.0+ 支持更多选项
    NS_OPTS="--mount --pid --uts --ipc --net --cgroup --user"
else
    # 旧版本
    NS_OPTS="--mount --pid --fork"
fi

# 执行隔离
unshare $NS_OPTS sh -c "..."
```

### 错误处理
```bash
unshare --mount --propagation private sh -c "
    # 尝试隐藏，失败则继续
    mount -o bind /dev/null /target 2>/dev/null || true
    
    # 设置陷阱，确保清理
    trap 'umount -l /target 2>/dev/null; exit' INT TERM EXIT
    
    # 运行命令
    if ! exec \"\$@\"; then
        echo \"Command failed: \$?\"
        exit 1
    fi
" -- "$@"
```

这些方法可以在 Android 上有效隐藏指定目录，但需要注意：
1. 需要 root 权限
2. 不同 Android 版本可能有差异
3. 某些应用可能有其他检测手段
4. 重启后设置会丢失（需要持久化配置）

-----------------------------------------------

这个命令是使用 `unshare` 创建私有挂载传播的隔离挂载命名空间。让我详细解释：

## 命令解析
```bash
unshare -mount --propagation private
```

### 选项含义：
- **`--mount`** 或 **`-m`**：创建新的挂载命名空间
- **`--propagation private`**：设置所有挂载点的传播类型为私有

## 挂载传播类型

Linux 有 4 种挂载传播类型：

### 1. **shared**（共享）
```bash
mount --make-shared /mnt
```
- 挂载事件会传播到其他命名空间
- 其他命名空间的挂载也会传播回来

### 2. **slave**（从属）
```bash
mount --make-slave /mnt
```
- 接收主命名空间的挂载事件
- 自身的挂载不会传播出去

### 3. **private**（私有）← **你使用的类型**
```bash
mount --make-private /mnt
```
- 完全隔离，不传播也不接收挂载事件
- 最安全的隔离模式

### 4. **unbindable**（不可绑定）
```bash
mount --make-unbindable /mnt
```
- 私有 + 不能被绑定挂载

## 实际应用示例

### 示例1：创建私有挂载空间并测试
```bash
# 创建私有挂载命名空间
unshare --mount --propagation private bash

# 查看当前传播类型
findmnt -o PROPAGATION /

# 创建测试挂载
mkdir /tmp/testmnt
mount -t tmpfs tmpfs /tmp/testmnt

# 在另一个终端查看，这个挂载不会传播到主机
```

### 示例2：运行程序时隔离挂载
```bash
# 运行程序在私有挂载空间中
unshare --mount --propagation private /path/to/your-app

# 或配合其他命名空间
unshare --mount --propagation private --pid --fork --mount-proc /bin/bash
```

### 示例3：临时修改文件系统而不影响主机
```bash
# 创建私有挂载空间
unshare --mount --propagation private bash

# 创建 overlay 文件系统进行修改
mkdir -p /tmp/{lower,upper,work,merged}
mount -t overlay overlay -o lowerdir=/tmp/lower,upperdir=/tmp/upper,workdir=/tmp/work /tmp/merged

# 现在对 /tmp/merged 的修改不会影响原文件系统
```

### 示例4：更精细的控制传播类型
```bash
unshare --mount bash

# 先设置为私有（默认）
mount --make-private /

# 只将特定目录设置为共享（如果需要）
mount --make-shared /tmp

# 查看不同挂载点的传播类型
cat /proc/self/mountinfo | grep -oP '(?<=shared:)\d+'
```

## 应用场景

### 1. **安全沙盒**
```bash
# 运行不可信程序，防止它挂载设备或修改主机挂载点
unshare --mount --propagation private --ipc --net --pid --fork --mount-proc /path/to/untrusted-app
```

### 2. **开发测试**
```bash
# 测试挂载操作而不影响系统
unshare --mount --propagation private bash
# 安全地测试 mount、umount、fstab 等
```

### 3. **容器运行**
```bash
# 类似容器的挂载隔离
unshare --mount --propagation private \
    --pid --fork --mount-proc \
    --uts --hostname container1 \
    --net \
    /bin/bash
```

### 4. **临时文件系统操作**
```bash
# 创建临时文件系统进行实验
unshare --mount --propagation private bash
mkdir /tmp/myroot
mount -t tmpfs tmpfs /tmp/myroot
# 现在可以在隔离环境中自由挂载
```

## 查看当前状态

### 查看传播类型：
```bash
# 查看所有挂载点的传播类型
findmnt -o TARGET,PROPAGATION

# 或查看详细信息
cat /proc/self/mountinfo | awk '{print $4, $5, $6, $7}'
```

### 查看命名空间：
```bash
# 查看当前进程的命名空间
ls -la /proc/$$/ns/

# 查看挂载命名空间ID
readlink /proc/$$/ns/mnt
```

## 注意事项

1. **权限需求**：需要 root 权限或 `CAP_SYS_ADMIN` 能力
2. **传播继承**：默认从父命名空间继承传播类型
3. **子挂载点**：新挂载点会继承父目录的传播类型
4. **递归设置**：使用 `--make-rprivate` 等选项可以递归设置

## 完整示例脚本

```bash
#!/bin/bash
# 创建完全隔离的测试环境

# 创建临时目录结构
TEMP_ROOT=$(mktemp -d)
mkdir -p $TEMP_ROOT/{bin,lib,lib64,dev,proc,sys,tmp}

# 复制必要程序
cp /bin/bash $TEMP_ROOT/bin/
cp /bin/ls $TEMP_ROOT/bin/

# 创建私有挂载命名空间运行
unshare --mount --propagation private \
    --pid --fork \
    --uts --hostname isolated \
    --net \
    chroot $TEMP_ROOT /bin/bash

# 清理
rm -rf $TEMP_ROOT
```
