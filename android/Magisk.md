Magisk 
- https://github.com/topjohnwu/Magisk
- Systemless Root
- Zygisk-Assistant
https://github.com/snake-4/Zygisk-Assistant

- https://github.com/hang666/JustTrustMePro
## DenyList不生效
```bash
# 1. 确保Zygisk已启用
magisk --zygisk enable

# 2. 检查DenyList是否正确配置
magisk --denylist status

# 3. 清除目标应用数据
pm clear <package_name>

# 4. 强制停止应用后重新打开
am force-stop <package_name>

# 5. 重启设备
reboot

```
## Magisk伪装后无法找到
```bash
# 方法1：通过ADB查找
adb shell pm list packages | grep -v "com.topjohnwu.magisk"

# 方法2：查看最近安装的应用
adb shell pm list packages -i | sort -k 2 -t '=' -r | head -n 10

# 方法3：使用Activity启动
adb shell am start -n <伪装后的包名>/.ui.MainActivity

```

## Zygisk 源码深度分析

```
native/jni/zygisk/
├── core/
│   ├── zygisk.hpp     # 核心头文件，定义API接口
│   ├── entry.cpp      # 模块入口定义
│   └── logging.cpp    # 日志系统
├── include/            # 公开头文件
├── module.cpp         # 模块管理
├── denylist.cpp       # 拒绝列表实现
├── entry.cpp          # Zygisk主入口
├── inject/            # 注入相关
│   ├── inject.cpp
│   └── ptrace.cpp     # ptrace注入实现
└── hijack/            # 劫持相关
    ├── dl_monitor.cpp # 动态库加载监控
    └── ...           # 架构特定的劫持代码
```

### 启动时注入（关键代码）

在 `native/jni/zygisk/entry.cpp` 中：

```cpp
// 这是Zygisk的入口点，通过magiskinit在系统启动时设置
extern "C" void zygisk_inject_entry() {
    // 1. 通过ptrace或unix socket连接到zygote进程
    int zygote_pid = find_zygote();
    
    // 2. 注入libzygisk.so到zygote进程
    if (inject_into_process(zygote_pid, "libzygisk.so")) {
        // 3. 触发zygote中libzygisk.so的初始化
        trigger_zygisk_init(zygote_pid);
    }
}
```

### 动态库劫持机制

在 `native/jni/zygisk/hijack/dl_monitor.cpp` 中，Zygisk 实现了创新的动态库加载劫持：

```cpp
void DLMonitor::monitor_dlopen() {
    // Hook关键的系统dlopen函数
    hook_dlopen_android();
    
    // 当Zygote加载libandroid_runtime.so时触发
    auto original_dlopen = get_original_dlopen();
    auto new_dlopen = [](const char* filename, int flags) {
        void* handle = original_dlopen(filename, flags);
        
        if (filename && strstr(filename, "libandroid_runtime.so")) {
            // 关键：在libandroid_runtime加载后注入Zygisk
            inject_after_runtime_loaded(handle);
        }
        
        return handle;
    };
    
    replace_dlopen_function(new_dlopen);
}
```

### Ptrace 注入实现

在 `native/jni/zygisk/inject/ptrace.cpp` 中：

```cpp
bool ptrace_inject(pid_t pid, const char* library_path) {
    // 1. 附加到目标进程
    ptrace_attach(pid);
    
    // 2. 调用目标进程的dlopen来加载libzygisk.so
    void* remote_mem = alloc_remote_memory(pid, library_path, strlen(library_path) + 1);
    
    // 3. 查找目标进程的dlopen函数地址
    void* dlopen_addr = find_remote_symbol(pid, "__dl_dlopen");
    
    // 4. 调用远程dlopen
    call_remote_function(pid, dlopen_addr, remote_mem, RTLD_NOW | RTLD_GLOBAL);
    
    // 5. 清理并脱离
    ptrace_detach(pid);
    return true;
}
```

## Zygisk 核心类分析

### ZygiskContext（核心上下文）

在 `native/jni/zygisk/core/zygisk.hpp` 中：

```cpp
class ZygiskContext {
private:
    // 单例模式
    static ZygiskContext *instance;
    
    // 模块管理
    std::vector<ZygiskModule*> modules;
    
    // 拒绝列表状态
    DenyListContext deny_ctx;
    
    // 通信管道
    int magiskd_socket;
    
public:
    // 初始化入口
    static void OnModuleLoaded() {
        instance = new ZygiskContext();
        instance->initialize();
    }
    
    void initialize() {
        // 1. 连接到magiskd获取配置
        connect_to_magiskd();
        
        // 2. 加载所有Zygisk模块
        load_modules();
        
        // 3. 设置fork处理器
        setup_fork_handler();
        
        // 4. 注册JNI原生方法
        register_jni_methods();
    }
    
    // 关键：fork事件处理
    void preFork() {
        // 在Zygote fork前执行
        for (auto& mod : modules) {
            mod->onPreFork();
        }
    }
    
    void postForkChild(int pid, bool is_system_server) {
        // fork完成后在子进程中执行
        if (deny_ctx.isDenied(pid)) {
            // 如果在拒绝列表中，清理Zygisk痕迹
            cleanupForDeniedProcess();
        } else {
            // 正常初始化模块
            for (auto& mod : modules) {
                mod->onPostForkChild(is_system_server);
            }
        }
    }
};
```

### 模块系统实现

在 `native/jni/zygisk/module.cpp` 中：

```cpp
void ZygiskContext::load_modules() {
    // 扫描模块目录
    std::vector<std::string> module_dirs = {
        "/data/adb/modules/*/zygisk/*.so",
        "/data/adb/zygisk-modules/*.so"
    };
    
    for (const auto& dir : module_dirs) {
        glob_t glob_result;
        glob(dir.c_str(), GLOB_NOSORT, nullptr, &glob_result);
        
        for (size_t i = 0; i < glob_result.gl_pathc; ++i) {
            const char* path = glob_result.gl_pathv[i];
            
            // 加载模块动态库
            void* handle = dlopen(path, RTLD_LAZY);
            if (!handle) continue;
            
            // 获取模块入口函数
            auto entry = (zygisk_module_entry_fn)dlsym(handle, "zygisk_module_entry");
            if (!entry) {
                dlclose(handle);
                continue;
            }
            
            // 创建模块实例
            auto* module = new ZygiskModule(handle, entry);
            
            // 调用模块初始化
            module->onLoad(this);
            
            modules.push_back(module);
        }
        
        globfree(&glob_result);
    }
}
```

### 拒绝列表实现

在 `native/jni/zygisk/denylist.cpp` 中：

```cpp
class DenyListContext {
private:
    // 拒绝列表配置（从magiskd获取）
    std::unordered_set<std::string> denied_packages;
    std::unordered_set<int> denied_uids;
    
    // 进程隐藏状态
    std::unordered_map<pid_t, HideState> hide_states;
    
public:
    bool isDenied(pid_t pid) {
        // 1. 通过进程名判断
        std::string process_name = get_process_name(pid);
        if (denied_packages.count(process_name) > 0) {
            return true;
        }
        
        // 2. 通过UID判断
        int uid = get_uid_for_pid(pid);
        if (denied_uids.count(uid) > 0) {
            return true;
        }
        
        // 3. 通过包名判断（需要读取/proc/pid/cmdline）
        std::string package = get_package_for_pid(pid);
        if (denied_packages.count(package) > 0) {
            return true;
        }
        
        return false;
    }
    
    void hideFromProcess(pid_t pid) {
        // 关键：从进程中移除所有Zygisk痕迹
        HideState& state = hide_states[pid];
        
        // 1. 卸载所有Zygisk模块
        for (auto& module : get_loaded_modules()) {
            if (module->is_loaded_in(pid)) {
                module->unload_from(pid);
            }
        }
        
        // 2. 清理内存映射
        unmap_zygisk_memory(pid);
        
        // 3. 清理环境变量
        cleanup_environment(pid);
        
        // 4. 恢复被Hook的系统调用
        restore_syscalls(pid);
        
        // 5. 隐藏Magisk文件系统
        hide_magisk_filesystem(pid);
        
        state.is_hidden = true;
    }
};
```

### JNI 方法注册

```cpp
// 注册Zygisk的JNI方法到Android运行时
void register_native_methods(JNIEnv* env) {
    // 获取AndroidRuntime类
    jclass runtime_class = env->FindClass("com/android/internal/os/Zygote");
    
    // Hook关键的Zygote方法
    JNINativeMethod methods[] = {
        {"forkAndSpecialize", "(...)I", (void*)zygisk_forkAndSpecialize},
        {"specializeAppProcess", "(...)V", (void*)zygisk_specializeAppProcess},
        {"forkSystemServer", "(...)I", (void*)zygisk_forkSystemServer}
    };
    
    // 替换原始方法
    for (const auto& method : methods) {
        void* original = nullptr;
        
        // 保存原始方法指针
        original = (void*)env->GetMethodID(
            runtime_class, 
            method.name, 
            method.signature
        );
        
        // 替换为新方法
        replace_jni_method(env, runtime_class, method, original);
        
        // 保存原始指针供后续调用
        save_original_method(original);
    }
}
```

### Fork 处理流程

```cpp
// Zygisk的forkAndSpecialize实现
jint zygisk_forkAndSpecialize(...) {
    ZygiskContext* ctx = ZygiskContext::GetInstance();
    
    // 1. fork前处理
    ctx->preFork();
    
    // 2. 调用原始forkAndSpecialize
    jint pid = original_forkAndSpecialize(...);
    
    if (pid == 0) {
        // 3. 在子进程中执行
        ctx->postForkChild(getpid(), false);
    }
    
    return pid;
}

// forkSystemServer处理
jint zygisk_forkSystemServer(...) {
    ZygiskContext* ctx = ZygiskContext::GetInstance();
    
    ctx->preFork();
    jint pid = original_forkSystemServer(...);
    
    if (pid == 0) {
        // 系统服务器进程不隐藏Zygisk
        ctx->postForkChild(getpid(), true);
    }
    
    return pid;
}
```

### 模块接口定义

```cpp
// 在zygisk.hpp中定义的模块API
struct api_version {
    int major;
    int minor;
};

struct ZygiskModule {
    api_version version;
    
    // 生命周期回调
    void (*onLoad)(ZygiskModule* module);
    void (*onUnload)();
    
    // fork事件回调
    void (*preAppSpecialize)(ZygiskModule* module, AppSpecializeArgs* args);
    void (*postAppSpecialize)(ZygiskModule* module, const AppSpecializeArgs* args);
    void (*preServerSpecialize)(ZygiskModule* module, ServerSpecializeArgs* args);
    void (*postServerSpecialize)(ZygiskModule* module, const ServerSpecializeArgs* args);
    
    // 功能接口
    void* (*getApi)(int api_id);
    void (*setOption)(ZygiskModule* module, int option);
    
    // 模块私有数据
    void* impl;
};
```

### 模块通信机制

```cpp
// 模块与Zygisk核心的IPC通信
class ModuleIPC {
private:
    // Unix domain socket通信
    int module_socket;
    
public:
    void sendToModule(ZygiskModule* module, const void* data, size_t len) {
        // 通过socket发送数据到模块进程
        write_to_socket(module_socket, data, len);
    }
    
    void receiveFromModule(ZygiskModule* module, void* buffer, size_t len) {
        // 从模块接收数据
        read_from_socket(module_socket, buffer, len);
    }
    
    // 远程过程调用（RPC）支持
    template<typename R, typename... Args>
    R callModuleFunction(ZygiskModule* module, const char* func_name, Args... args) {
        // 序列化参数
        auto request = serialize_call(func_name, args...);
        
        // 发送请求
        sendToModule(module, request.data(), request.size());
        
        // 接收响应
        auto response = receive_response<R>();
        
        // 反序列化并返回结果
        return deserialize<R>(response);
    }
};
```

### 文件系统隐藏

```cpp
void hide_magisk_filesystem(pid_t pid) {
    // 1. 挂载命名空间隔离
    enter_mount_namespace(pid);
    
    // 2. 覆盖Magisk相关目录
    mount_overlay("/data/adb", "/dev/null");
    mount_overlay("/sbin/.magisk", "/dev/null");
    
    // 3. 隐藏模块目录
    hide_module_directories(pid);
    
    // 4. 恢复原始挂载点
    restore_original_mounts(pid);
}
```

### 进程信息隐藏

```cpp
void hide_process_info(pid_t pid) {
    // Hook /proc/pid/ 相关的文件访问
    hook_proc_filesystem();
    
    // 1. 隐藏模块加载的库
    filter_proc_maps(pid, [](const char* line) {
        return !strstr(line, "zygisk") && !strstr(line, "magisk");
    });
    
    // 2. 隐藏环境变量
    filter_proc_environ(pid, [](const char* var) {
        return !strstr(var, "MAGISK") && !strstr(var, "ZYGISK");
    });
    
    // 3. 隐藏命令行参数
    filter_proc_cmdline(pid, [](const char* arg) {
        return !strstr(arg, "zygisk");
    });
}
```

### SELinux 上下文维护

```cpp
void maintain_selinux_context() {
    // 确保Zygisk操作在正确的SELinux上下文中
    setcon("u:r:zygote:s0");
    
    // 为模块设置适当的域
    if (is_denied_process()) {
        // 被拒绝的进程使用原始上下文
        restore_original_context();
    } else {
        // 允许的进程可以使用扩展权限
        transition_to_magisk_context();
    }
}
```

### 兼容性适配层

```cpp
class CompatibilityLayer {
public:
    // 处理不同Android版本的差异
    static void adapt_for_api_level(int api_level) {
        switch (api_level) {
            case __ANDROID_API_R__:
                adapt_for_android_r();
                break;
            case __ANDROID_API_S__:
                adapt_for_android_s();
                break;
            case __ANDROID_API_T__:
                adapt_for_android_t();
                break;
            default:
                LOGW("Unsupported API level %d", api_level);
        }
    }
    
private:
    static void adapt_for_android_r() {
        // Android 11的特定适配
        hook_android_r_zygote();
    }
    
    static void adapt_for_android_s() {
        // Android 12的特定适配
        // 处理新的seccomp策略
        handle_android_s_seccomp();
    }
};
```

### 调试与日志系统

```cpp
class ZygiskLogger {
private:
    static constexpr const char* LOG_TAG = "Zygisk";
    
public:
    static void log(LogLevel level, const char* fmt, ...) {
        if (!should_log(level)) return;
        
        va_list args;
        va_start(args, fmt);
        
        // 输出到logcat
        __android_log_vprint(to_android_priority(level), LOG_TAG, fmt, args);
        
        // 同时写入内部日志文件（仅debug版本）
#ifdef ZYGISK_DEBUG
        write_to_internal_log(fmt, args);
#endif
        
        va_end(args);
    }
    
    // 进程特定的日志
    static void log_for_pid(pid_t pid, const char* fmt, ...) {
        char tag[64];
        snprintf(tag, sizeof(tag), "Zygisk[%d]", pid);
        
        va_list args;
        va_start(args, fmt);
        __android_log_vprint(ANDROID_LOG_INFO, tag, fmt, args);
        va_end(args);
    }
};
```
