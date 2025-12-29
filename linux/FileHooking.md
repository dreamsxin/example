使用文件钩子（File Hooking）来隐藏或修改文件访问行为是高级的 root 隐藏技术。以下是多种实现方法：

## 1. LD_PRELOAD 方法（用户空间）

### 基本实现
```c
// hide_file.c
#define _GNU_SOURCE
#include <dlfcn.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <dirent.h>

// 要隐藏的文件列表
const char *hidden_files[] = {
    "/system/bin/su",
    "/system/xbin/su",
    "/system/sbin/su",
    "/sbin/su",
    "/vendor/bin/su",
    "/su",
    "/magisk",
    NULL
};

// 检查文件是否需要隐藏
int should_hide(const char *path) {
    if (!path) return 0;
    
    for (int i = 0; hidden_files[i] != NULL; i++) {
        if (strstr(path, hidden_files[i]) != NULL) {
            return 1;
        }
    }
    
    // 还可以检查文件名是否包含 "su"
    if (strstr(path, "/su") != NULL) {
        const char *basename = strrchr(path, '/');
        if (basename && strcmp(basename + 1, "su") == 0) {
            return 1;
        }
    }
    
    return 0;
}

// Hook stat 系列函数
int __lxstat(int ver, const char *path, struct stat *buf) {
    static int (*real_lxstat)(int, const char*, struct stat*) = NULL;
    if (!real_lxstat) {
        real_lxstat = dlsym(RTLD_NEXT, "__lxstat");
    }
    
    if (should_hide(path)) {
        errno = ENOENT;
        return -1;
    }
    
    return real_lxstat(ver, path, buf);
}

int __lxstat64(int ver, const char *path, struct stat64 *buf) {
    static int (*real_lxstat64)(int, const char*, struct stat64*) = NULL;
    if (!real_lxstat64) {
        real_lxstat64 = dlsym(RTLD_NEXT, "__lxstat64");
    }
    
    if (should_hide(path)) {
        errno = ENOENT;
        return -1;
    }
    
    return real_lxstat64(ver, path, buf);
}

// Hook access
int access(const char *path, int mode) {
    static int (*real_access)(const char*, int) = NULL;
    if (!real_access) {
        real_access = dlsym(RTLD_NEXT, "access");
    }
    
    if (should_hide(path)) {
        errno = ENOENT;
        return -1;
    }
    
    return real_access(path, mode);
}

// Hook open
int open(const char *path, int flags, ...) {
    static int (*real_open)(const char*, int, ...) = NULL;
    if (!real_open) {
        real_open = dlsym(RTLD_NEXT, "open");
    }
    
    if (should_hide(path)) {
        errno = ENOENT;
        return -1;
    }
    
    va_list ap;
    va_start(ap, flags);
    mode_t mode = 0;
    if (flags & O_CREAT) {
        mode = va_arg(ap, mode_t);
    }
    va_end(ap);
    
    return real_open(path, flags, mode);
}

// Hook opendir/readdir（隐藏目录中的 su 文件）
DIR *opendir(const char *name) {
    static DIR* (*real_opendir)(const char*) = NULL;
    if (!real_opendir) {
        real_opendir = dlsym(RTLD_NEXT, "opendir");
    }
    
    return real_opendir(name);
}

struct dirent *readdir(DIR *dirp) {
    static struct dirent* (*real_readdir)(DIR*) = NULL;
    if (!real_readdir) {
        real_readdir = dlsym(RTLD_NEXT, "readdir");
    }
    
    struct dirent *entry;
    while ((entry = real_readdir(dirp)) != NULL) {
        if (strstr(entry->d_name, "su") == NULL) {
            break;
        }
    }
    
    return entry;
}

// Hook fopen
FILE *fopen(const char *path, const char *mode) {
    static FILE* (*real_fopen)(const char*, const char*) = NULL;
    if (!real_fopen) {
        real_fopen = dlsym(RTLD_NEXT, "fopen");
    }
    
    if (should_hide(path)) {
        errno = ENOENT;
        return NULL;
    }
    
    return real_fopen(path, mode);
}
```

### 编译和使用
```bash
# 编译
gcc -shared -fPIC hide_file.c -o libhidefile.so -ldl

# Android 上编译（使用 NDK）
$NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android21-clang \
    -shared -fPIC hide_file.c -o libhidefile.so -ldl

# 使用
export LD_PRELOAD=/path/to/libhidefile.so
# 然后运行目标应用
```

## 2. Frida 动态注入（无需编译）

```javascript
// frida-file-hook.js
Java.perform(function() {
    console.log("[*] Starting file hook...");
    
    // Hook Java 层的 File 类
    var File = Java.use('java.io.File');
    
    File.exists.implementation = function() {
        var path = this.getAbsolutePath();
        if (isSuFile(path)) {
            console.log("[+] Hiding su file (Java): " + path);
            return false;
        }
        return this.exists();
    };
    
    File.canRead.implementation = function() {
        var path = this.getAbsolutePath();
        if (isSuFile(path)) {
            return false;
        }
        return this.canRead();
    };
    
    File.canExecute.implementation = function() {
        var path = this.getAbsolutePath();
        if (isSuFile(path)) {
            return false;
        }
        return this.canExecute();
    };
    
    // Hook Runtime.exec
    var Runtime = Java.use('java.lang.Runtime');
    Runtime.exec.overload('[Ljava.lang.String;').implementation = function(cmdarray) {
        var cmd = cmdarray.join(' ');
        if (cmd.indexOf('su') !== -1 || cmd.indexOf('which su') !== -1) {
            console.log("[+] Blocking su command: " + cmd);
            throw Java.use('java.io.IOException').$new("Command not found");
        }
        return this.exec(cmdarray);
    };
    
    // Native hook (通过 Module)
    var Module = Process.findModuleByName("libc.so");
    
    // Hook access()
    var access = Module.findExportByName("access");
    Interceptor.attach(access, {
        onEnter: function(args) {
            var path = Memory.readUtf8String(args[0]);
            if (isSuFile(path)) {
                console.log("[+] Hiding su file (native access): " + path);
                this.hide = true;
            }
        },
        onLeave: function(retval) {
            if (this.hide) {
                retval.replace(-1);
                var errno = Module.findExportByName("__errno");
                if (errno) {
                    Memory.writeInt(ptr(errno), 2); // ENOENT
                }
            }
        }
    });
    
    // Hook stat()
    var stat = Module.findExportByName("stat");
    if (stat) {
        Interceptor.attach(stat, {
            onEnter: function(args) {
                var path = Memory.readUtf8String(args[0]);
                if (isSuFile(path)) {
                    console.log("[+] Hiding su file (native stat): " + path);
                    this.hide = true;
                }
            },
            onLeave: function(retval) {
                if (this.hide) {
                    retval.replace(-1);
                }
            }
        });
    }
    
    // Hook fopen()
    var fopen = Module.findExportByName("fopen");
    Interceptor.attach(fopen, {
        onEnter: function(args) {
            var path = Memory.readUtf8String(args[0]);
            if (isSuFile(path)) {
                console.log("[+] Hiding su file (native fopen): " + path);
                this.hide = true;
            }
        },
        onLeave: function(retval) {
            if (this.hide) {
                retval.replace(ptr(0));
            }
        }
    });
    
    function isSuFile(path) {
        if (!path) return false;
        
        var suPaths = [
            '/system/bin/su',
            '/system/xbin/su', 
            '/system/sbin/su',
            '/sbin/su',
            '/vendor/bin/su',
            '/su',
            '/magisk'
        ];
        
        for (var i = 0; i < suPaths.length; i++) {
            if (path.indexOf(suPaths[i]) !== -1) {
                return true;
            }
        }
        
        // 检查文件名是否为 su
        if (path.lastIndexOf('/su') === path.length - 3) {
            return true;
        }
        
        return false;
    }
});

// 使用 Frida 注入
// frida -U -f com.target.app -l frida-file-hook.js --no-pause
```

## 3. Xposed 模块方法

```java
// XposedFileHide.java
package com.example.filehide;

import de.robv.android.xposed.IXposedHookLoadPackage;
import de.robv.android.xposed.XC_MethodHook;
import de.robv.android.xposed.XposedHelpers;
import de.robv.android.xposed.callbacks.XC_LoadPackage;

public class XposedFileHide implements IXposedHookLoadPackage {
    
    private static final String[] HIDDEN_FILES = {
        "/system/bin/su",
        "/system/xbin/su",
        "/system/sbin/su",
        "/sbin/su",
        "/vendor/bin/su"
    };
    
    @Override
    public void handleLoadPackage(XC_LoadPackage.LoadPackageParam lpparam) throws Throwable {
        // 只处理目标应用
        if (!lpparam.packageName.equals("com.target.app")) {
            return;
        }
        
        // Hook File.exists()
        XposedHelpers.findAndHookMethod("java.io.File", lpparam.classLoader,
            "exists", new XC_MethodHook() {
                @Override
                protected void beforeHookedMethod(MethodHookParam param) throws Throwable {
                    File file = (File) param.thisObject;
                    String path = file.getAbsolutePath();
                    
                    for (String hidden : HIDDEN_FILES) {
                        if (path.equals(hidden)) {
                            param.setResult(false);
                            return;
                        }
                    }
                }
            });
        
        // Hook Runtime.exec()
        XposedHelpers.findAndHookMethod("java.lang.Runtime", lpparam.classLoader,
            "exec", String[].class, new XC_MethodHook() {
                @Override
                protected void beforeHookedMethod(MethodHookParam param) throws Throwable {
                    String[] cmd = (String[]) param.args[0];
                    if (cmd != null) {
                        String fullCmd = String.join(" ", cmd);
                        if (fullCmd.contains("su") || fullCmd.contains("which su")) {
                            throw new IOException("Command not found");
                        }
                    }
                }
            });
        
        // Hook ProcessBuilder.start()
        XposedHelpers.findAndHookMethod("java.lang.ProcessBuilder", lpparam.classLoader,
            "start", new XC_MethodHook() {
                @Override
                protected void beforeHookedMethod(MethodHookParam param) throws Throwable {
                    ProcessBuilder pb = (ProcessBuilder) param.thisObject;
                    List<String> command = pb.command();
                    if (command != null) {
                        String fullCmd = String.join(" ", command);
                        if (fullCmd.contains("su") || fullCmd.contains("which su")) {
                            throw new IOException("Command not found");
                        }
                    }
                }
            });
    }
    
    private boolean isSuFile(String path) {
        if (path == null) return false;
        
        for (String hidden : HIDDEN_FILES) {
            if (path.equals(hidden)) {
                return true;
            }
        }
        
        // 检查文件名
        return path.endsWith("/su");
    }
}
```

## 4. Magisk 模块实现

```bash
#!/system/bin/sh
# Magisk 模块：通过 bind mount 隐藏文件

MODPATH=$1

# 创建空文件用于覆盖
mkdir -p $MODPATH/system/bin
touch $MODPATH/system/bin/su

# 创建 module.prop
echo "id=filehide
name=File Hide Module
version=v1.0
versionCode=1
author=You
description=Hide su files from apps" > $MODPATH/module.prop

# 创建 service.sh 在启动时挂载
cat > $MODPATH/service.sh << 'EOF'
#!/system/bin/sh

# 等待系统启动完成
while [ "$(getprop sys.boot_completed)" != "1" ]; do
    sleep 1
done

# 挂载覆盖 su 文件
mount -o bind /dev/null /system/bin/su 2>/dev/null
mount -o bind /dev/null /system/xbin/su 2>/dev/null
mount -o bind /dev/null /system/sbin/su 2>/dev/null
mount -o bind /dev/null /sbin/su 2>/dev/null
mount -o bind /dev/null /vendor/bin/su 2>/dev/null

# 也可以使用 tmpfs 完全隐藏
# mount -t tmpfs tmpfs /system/bin
# cp /system/bin/* /tmp/system_bin/
# mount -o bind /tmp/system_bin /system/bin
EOF

chmod 755 $MODPATH/service.sh
```

## 5. Kernel 模块（最底层）

```c
// kernel_file_hook.c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/dcache.h>
#include <linux/namei.h>
#include <linux/string.h>

static int (*orig_permission)(struct inode *, int);

// Hook inode_permission
static int hook_permission(struct inode *inode, int mask) {
    struct dentry *dentry;
    struct path path;
    char *filepath;
    int ret;
    
    // 获取文件路径
    dentry = d_find_alias(inode);
    if (!dentry)
        goto out;
    
    path.mnt = NULL;
    path.dentry = dentry;
    
    filepath = d_path(&path, kmalloc(PATH_MAX, GFP_KERNEL), PATH_MAX);
    if (IS_ERR(filepath))
        goto out_dput;
    
    // 检查是否为 su 文件
    if (strstr(filepath, "/su") != NULL) {
        kfree(filepath);
        dput(dentry);
        return -ENOENT;  // 权限被拒绝
    }
    
    kfree(filepath);
    
out_dput:
    dput(dentry);
out:
    return orig_permission(inode, mask);
}

// Hook file open
static int (*orig_filp_open)(const char *, int, umode_t);

static struct file *hook_filp_open(const char *filename, int flags, umode_t mode) {
    if (strstr(filename, "/su") != NULL) {
        return ERR_PTR(-ENOENT);
    }
    return orig_filp_open(filename, flags, mode);
}

static int __init file_hook_init(void) {
    printk(KERN_INFO "File hook module loaded\n");
    
    // Hook 系统调用表（需要获取地址）
    // 这里简化处理，实际需要找到 sys_call_table
    
    return 0;
}

static void __exit file_hook_exit(void) {
    printk(KERN_INFO "File hook module unloaded\n");
}

module_init(file_hook_init);
module_exit(file_hook_exit);

MODULE_LICENSE("GPL");
```

## 6. 使用 ptrace 拦截系统调用

```c
// ptrace_hook.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/reg.h>

#ifdef __x86_64__
#define SYSCALL_OPENAT 257
#elif defined(__aarch64__)
#define SYSCALL_OPENAT 56
#endif

void hide_su_files(pid_t pid, struct user_regs_struct *regs) {
    long path_addr;
    char path[256];
    
    // 获取文件路径参数
    #ifdef __x86_64__
    path_addr = regs->rdi;
    #elif defined(__aarch64__)
    path_addr = regs->regs[1];
    #endif
    
    // 读取路径
    long *data = (long*)&path[0];
    for (int i = 0; i < sizeof(path)/sizeof(long); i++) {
        data[i] = ptrace(PTRACE_PEEKDATA, pid, path_addr + i * sizeof(long), NULL);
    }
    
    // 检查是否访问 su 文件
    if (strstr(path, "/su") != NULL) {
        // 修改返回值为 -ENOENT
        #ifdef __x86_64__
        regs->rax = -2;  // ENOENT
        #elif defined(__aarch64__)
        regs->regs[0] = -2;
        #endif
        
        ptrace(PTRACE_SETREGS, pid, NULL, regs);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <program>\n", argv[0]);
        return 1;
    }
    
    pid_t pid = fork();
    
    if (pid == 0) {
        // 子进程：跟踪目标程序
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execvp(argv[1], &argv[1]);
    } else {
        // 父进程：监控系统调用
        int status;
        struct user_regs_struct regs;
        
        while (1) {
            waitpid(pid, &status, 0);
            
            if (WIFEXITED(status))
                break;
            
            // 获取寄存器
            ptrace(PTRACE_GETREGS, pid, NULL, &regs);
            
            // 检查系统调用
            #ifdef __x86_64__
            long syscall = regs.orig_rax;
            #elif defined(__aarch64__)
            long syscall = regs.regs[8];
            #endif
            
            // 拦截 openat 系统调用
            if (syscall == SYSCALL_OPENAT) {
                hide_su_files(pid, &regs);
            }
            
            // 继续执行
            ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
        }
    }
    
    return 0;
}
```

## 7. 综合脚本：自动设置文件钩子

```bash
#!/system/bin/sh
# auto_file_hook.sh

echo "[*] Setting up file hooks..."

# 1. 编译 LD_PRELOAD 库
cat > /data/local/tmp/hide_file.c << 'EOF'
// 上面的 C 代码
EOF

# 在 Android 上编译（需要 NDK）
CLANG="/data/local/tmp/clang"  # 假设 clang 已上传
$CLANG -shared -fPIC /data/local/tmp/hide_file.c -o /data/local/tmp/libhidefile.so

# 2. 注入到目标进程
TARGET_PID=$(pidof com.target.app)

if [ -n "$TARGET_PID" ]; then
    # 方法1：使用 LD_PRELOAD 注入
    echo "/data/local/tmp/libhidefile.so" > /proc/$TARGET_PID/environ
    kill -STOP $TARGET_PID
    kill -CONT $TARGET_PID
    
    # 方法2：使用 /etc/ld.so.preload（系统级）
    echo "/data/local/tmp/libhidefile.so" >> /system/etc/ld.so.preload
    
    # 方法3：使用 app_process 注入
    export CLASSPATH=/data/local/tmp/frida-gadget.so
    export LD_PRELOAD=/data/local/tmp/libhidefile.so
    app_process -Djava.class.path=/data/local/tmp/frida-server /system/bin com.target.app &
fi

# 3. 设置 Magisk 模块
if [ -d /data/adb/modules ]; then
    mkdir -p /data/adb/modules/file_hide
    cat > /data/adb/modules/file_hide/service.sh << 'EOF'
#!/system/bin/sh
# 系统启动时设置钩子
while [ "$(getprop sys.boot_completed)" != "1" ]; do
    sleep 1
done

# 设置 LD_PRELOAD 全局
setprop wrap.com.target.app "LD_PRELOAD=/data/local/tmp/libhidefile.so"

# 绑定挂载空文件
for path in /system/bin/su /system/xbin/su /system/sbin/su; do
    if [ -f "$path" ]; then
        mount -o bind /dev/null "$path"
    fi
done
EOF
fi

echo "[+] File hooks installed"
```

## 8. 检测和绕过技巧

```bash
# 检测应用是否在检查文件
strace -f -e trace=file -p <pid>

# 查看应用加载的库
cat /proc/<pid>/maps

# 检查 LD_PRELOAD 环境
cat /proc/<pid>/environ | tr '\0' '\n' | grep LD_PRELOAD

# 绕过检测的方法：
# 1. 随机化钩子库名称
# 2. 使用 inline hook 而不是 LD_PRELOAD
# 3. 在 zygote 中早期注入
# 4. 使用内核模块完全隐藏
```

## 注意事项

1. **权限问题**：需要 root 权限
2. **稳定性**：不正确的钩子可能导致崩溃
3. **检测风险**：某些应用会检测钩子的存在
4. **性能影响**：过多的钩子会影响性能
5. **兼容性**：不同 Android 版本可能需要不同实现

**建议**：对于普通用户，使用 Magisk Hide 或 Shamiko 模块是最简单有效的方法。文件钩子适合高级用户或特定需求。
