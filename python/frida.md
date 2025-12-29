#

- https://github.com/frida/frida

Frida 允许在运行时对即将执行的操作进行操作。
比如，当我们从一个简单的 C++ 程序开始，该程序使用一个函数将两个值相加并返回结果。我们想要操作的函数声明为 `Add(int,int)`。首先，我们将更改其中一个 `int` 参数，然后，更改返回的结果。

## 修改内存

我来演示如何在 Ubuntu 下创建一个简单的 C 程序，并用 Frida 动态修改内存中的字符串。

### 1. 创建 C 程序

```c
// hello.c
#include <stdio.h>
#include <unistd.h>

void print_message() {
    char message[] = "Hello World!";
    printf("Message: %s\n", message);
    printf("Address of message: %p\n", (void*)message);
    
    // 持续打印，方便我们修改
    while(1) {
        printf("Current: %s\n", message);
        sleep(3);
    }
}

int main() {
    printf("PID: %d\n", getpid());
    print_message();
    return 0;
}
```

或者更简单的版本：

```c
// hello_simple.c
#include <stdio.h>
#include <unistd.h>

int main() {
    char msg[] = "Hello World!";
    printf("PID: %d\n", getpid());
    printf("Message address: %p\n", (void*)msg);
    
    while(1) {
        printf("Current message: %s\n", msg);
        fflush(stdout);
        sleep(2);
    }
    
    return 0;
}
```

### 2. 编译和运行

```bash
# 编译程序
gcc hello_simple.c -o hello -no-pie -fno-pie
# -no-pie 选项使地址固定，便于查找

# 运行程序
./hello
# 输出类似：
# PID: 12345
# Message address: 0x7ffc3a4b8a20
# Current message: Hello World!
```

### 3. Frida 脚本修改内存

```javascript
// modify_hello.js
'use strict';

// 方法1: 直接搜索和修改内存
function method1_searchAndReplace() {
    console.log("[*] Method 1: Searching for string in memory...");
    
    // 搜索 "Hello World!" 字符串
    var targetString = "Hello World!";
    var replacement = "Good Job!!!";
    
    Process.enumerateRanges('rw-').forEach(function(range) {
        console.log(`[*] Scanning range: ${range.base} - ${range.base.add(range.size)}`);
        
        Memory.scan(range.base, range.size, targetString, {
            onMatch: function(address, size) {
                console.log(`[+] Found "${targetString}" at: ${address}`);
                
                // 读取原始值确认
                var original = Memory.readUtf8String(address);
                console.log(`    Original: "${original}"`);
                
                // 检查长度（新字符串不能比原字符串长）
                if (replacement.length > targetString.length) {
                    console.log(`[-] Replacement too long! ${replacement.length} > ${targetString.length}`);
                    return;
                }
                
                // 修改内存
                Memory.writeUtf8String(address, replacement);
                
                // 验证修改
                var modified = Memory.readUtf8String(address);
                console.log(`    Modified: "${modified}"`);
                console.log(`    Success: ${modified === replacement}`);
            },
            onError: function(reason) {
                console.log(`[-] Scan error: ${reason}`);
            },
            onComplete: function() {
                console.log("[*] Scan completed for this range");
            }
        });
    });
}

// 方法2: 通过已知地址修改（如果知道字符串地址）
function method2_directAddress(pid) {
    console.log("[*] Method 2: Direct address modification");
    
    // 首先运行程序获取地址，然后在这里硬编码
    // 或者使用 Pattern 搜索
    var patterns = [
        "48 65 6C 6C 6F 20 57 6F 72 6C 64 21",  // "Hello World!" 的十六进制
        "Hello World!"
    ];
    
    patterns.forEach(function(pattern) {
        Memory.scan(Process.getRangeByAddress(Process.enumerateModules()[0].base).base, 
                    0x100000, pattern, {
            onMatch: function(address, size) {
                console.log(`[+] Pattern found at: ${address}`);
                Memory.writeUtf8String(address, "Good Job!!!");
            }
        });
    });
}

// 方法3: Hook printf 函数并修改参数
function method3_hookPrintf() {
    console.log("[*] Method 3: Hooking printf function");
    
    var printfAddr = Module.findExportByName(null, "printf");
    if (!printfAddr) {
        console.log("[-] printf not found");
        return;
    }
    
    console.log(`[+] printf found at: ${printfAddr}`);
    
    Interceptor.attach(printfAddr, {
        onEnter: function(args) {
            try {
                // 获取 printf 的第一个参数（格式化字符串）
                var format = Memory.readUtf8String(args[0]);
                
                // 检查是否包含我们的目标字符串
                if (format && format.indexOf("Hello World!") !== -1) {
                    console.log(`[!] printf called with: ${format.substring(0, 50)}...`);
                    
                    // 修改格式字符串中的内容
                    var newFormat = format.replace("Hello World!", "Good Job!!!");
                    
                    // 分配新内存并替换参数
                    var newFormatPtr = Memory.allocUtf8String(newFormat);
                    args[0] = newFormatPtr;
                    
                    console.log(`[*] Modified to: ${newFormat.substring(0, 50)}...`);
                    
                    // 保存指针以便清理
                    this.newFormatPtr = newFormatPtr;
                }
            } catch(e) {
                // 忽略错误
            }
        },
        onLeave: function(retval) {
            // 清理分配的内存（可选）
            // if (this.newFormatPtr) {
            //     Memory.free(this.newFormatPtr);
            // }
        }
    });
}

// 方法4: 更精确地修改 - 查找栈上的字符串
function method4_modifyStackVariable() {
    console.log("[*] Method 4: Looking for stack variable");
    
    // 查找 main 函数
    var mainAddr = Module.findExportByName(null, "main");
    if (!mainAddr) {
        // 尝试从 entry point 开始
        mainAddr = Process.enumerateModules()[0].base;
    }
    
    console.log(`[*] Main function at: ${mainAddr}`);
    
    // Hook main 函数访问局部变量
    Interceptor.attach(mainAddr, {
        onEnter: function(args) {
            console.log("[+] Main function entered");
            
            // 遍历栈帧，查找字符串
            var stackTop = this.context.sp;
            var stackBottom = stackTop.add(0x1000);  // 检查 4KB 栈空间
            
            Memory.scan(stackTop, 0x1000, "Hello World!", {
                onMatch: function(address, size) {
                    console.log(`[+] Found on stack at: ${address}`);
                    
                    // 验证它是栈地址
                    if (address.compare(stackTop) >= 0 && address.compare(stackBottom) <= 0) {
                        console.log(`    It's on the stack!`);
                        
                        // 尝试修改
                        try {
                            Memory.writeUtf8String(address, "Good Job!!!");
                            console.log(`    Modified successfully`);
                        } catch(e) {
                            console.log(`[-] Modification failed: ${e}`);
                        }
                    }
                }
            });
        }
    });
}

// 方法5: 使用内存断点监控访问
function method5_memoryBreakpoint() {
    console.log("[*] Method 5: Setting memory breakpoint");
    
    // 先找到字符串
    var found = false;
    Process.enumerateRanges('rw-').forEach(function(range) {
        if (found) return;
        
        Memory.scan(range.base, range.size, "Hello World!", {
            onMatch: function(address, size) {
                if (found) return;
                
                console.log(`[+] Found string at: ${address}`);
                found = true;
                
                // 设置内存访问监控
                MemoryAccessMonitor.enable({
                    base: address,
                    size: size
                }, {
                    onAccess: function(details) {
                        console.log("\n[!] Memory accessed!");
                        console.log(`    Address: ${details.address}`);
                        console.log(`    From: ${details.from}`);
                        console.log(`    Operation: ${details.operation}`);
                        
                        // 如果是写操作，可以修改值
                        if (details.operation === 'write') {
                            console.log(`[*] Write operation detected`);
                        }
                        
                        // 读取时修改返回值
                        if (details.operation === 'read') {
                            // 这里可以修改读取的值
                            console.log(`[*] Read operation - current value: ${Memory.readUtf8String(address)}`);
                        }
                    }
                });
                
                console.log(`[*] Memory breakpoint set at ${address}`);
            }
        });
    });
}

// 主函数
function main() {
    console.log(`[*] Attached to PID: ${Process.id}`);
    console.log(`[*] Process name: ${Process.getCurrentDir()}`);
    
    // 列出所有模块
    Process.enumerateModules().forEach(function(m, i) {
        if (i < 5) {  // 只显示前5个
            console.log(`    ${m.name} @ ${m.base}`);
        }
    });
    
    // 尝试各种方法
    try {
        method1_searchAndReplace();
    } catch(e) {
        console.log(`[-] Method 1 failed: ${e}`);
    }
    
    // 等待一会儿再试其他方法
    setTimeout(function() {
        try {
            method3_hookPrintf();
        } catch(e) {
            console.log(`[-] Method 3 failed: ${e}`);
        }
    }, 1000);
    
    // 设置定时器定期检查
    setInterval(function() {
        console.log("[*] Periodic check...");
        method1_searchAndReplace();
    }, 5000);
}

// 执行主函数
setTimeout(main, 1000);
```

### 4. 专门针对这个程序的优化脚本

```javascript
// hello_mod_simple.js
'use strict';

// 专用脚本 - 针对我们编译的 hello 程序
function modifyHelloWorld() {
    console.log("[*] Starting Hello World modification...");
    
    var target = "Hello World!";
    var replacement = "Good Job!!!";
    
    var attempts = 0;
    var maxAttempts = 10;
    
    var timer = setInterval(function() {
        attempts++;
        if (attempts > maxAttempts) {
            console.log("[-] Max attempts reached, giving up");
            clearInterval(timer);
            return;
        }
        
        console.log(`[*] Attempt ${attempts} to find "${target}"`);
        
        // 搜索所有可写内存区域
        Process.enumerateRanges('rw-').forEach(function(range) {
            // 快速检查范围大小，避免扫描过大区域
            if (range.size > 1000000) return;
            
            Memory.scan(range.base, range.size, target, {
                onMatch: function(address, size) {
                    console.log(`[+] Found at ${address} in range ${range.base}`);
                    
                    // 读取验证
                    var current = Memory.readUtf8String(address);
                    console.log(`    Current value: "${current}"`);
                    
                    if (current === target) {
                        // 修改
                        Memory.writeUtf8String(address, replacement);
                        
                        // 再次验证
                        var modified = Memory.readUtf8String(address);
                        console.log(`    Modified to: "${modified}"`);
                        
                        if (modified === replacement) {
                            console.log(`[SUCCESS] String modified successfully!`);
                            clearInterval(timer);
                        }
                    }
                },
                onComplete: function() {
                    // 扫描完成
                }
            });
        });
    }, 1000);
}

// 另一种方法：Hook puts 函数（更简单）
function hookPutsFunction() {
    var putsAddr = Module.findExportByName(null, "puts");
    if (!putsAddr) {
        putsAddr = Module.findExportByName(null, "printf");
    }
    
    if (putsAddr) {
        console.log(`[+] Found puts/printf at: ${putsAddr}`);
        
        Interceptor.attach(putsAddr, {
            onEnter: function(args) {
                try {
                    var str = Memory.readUtf8String(args[0]);
                    if (str && str.indexOf("Hello World!") !== -1) {
                        console.log(`[!] Intercepted string: ${str}`);
                        
                        // 修改字符串
                        var newStr = str.replace("Hello World!", "Good Job!!!");
                        var newStrPtr = Memory.allocUtf8String(newStr);
                        
                        args[0] = newStrPtr;
                        this.newStrPtr = newStrPtr;
                        
                        console.log(`[*] Changed to: ${newStr}`);
                    }
                } catch(e) {
                    // 忽略错误
                }
            }
        });
    }
}

// 直接修改 .rodata 段（如果字符串在只读段）
function modifyRodata() {
    var mainModule = Process.enumerateModules()[0];
    console.log(`[*] Main module: ${mainModule.name} @ ${mainModule.base}`);
    
    // 获取 .rodata 段
    mainModule.enumerateSections().forEach(function(section) {
        if (section.name.indexOf("rodata") !== -1 || 
            section.name === ".data") {
            console.log(`[*] Scanning section ${section.name} (${section.size} bytes)`);
            
            Memory.scan(section.virtualAddress, section.size, "Hello World!", {
                onMatch: function(address, size) {
                    console.log(`[+] Found in ${section.name} at ${address}`);
                    
                    // 尝试修改内存保护
                    try {
                        Memory.protect(address, size, 'rwx');
                        Memory.writeUtf8String(address, "Good Job!!!");
                        console.log(`[SUCCESS] Modified in ${section.name}`);
                    } catch(e) {
                        console.log(`[-] Failed to modify: ${e}`);
                    }
                }
            });
        }
    });
}

// 执行
Java.perform(function() {
    console.log("[*] Frida script loaded");
    
    // 尝试多种方法
    modifyHelloWorld();
    
    setTimeout(hookPutsFunction, 2000);
    setTimeout(modifyRodata, 4000);
});
```

### 5. Python 控制脚本

```python
#!/usr/bin/env python3
# frida_hello.py

import frida
import sys
import time

def on_message(message, data):
    if message['type'] == 'send':
        print(f"[*] {message['payload']}")
    elif message['type'] == 'error':
        print(f"[-] Error: {message['stack']}")
    else:
        print(message)

def main():
    # 方法1: 附加到运行中的进程
    # 首先运行 ./hello，然后获取 PID
    pid = input("Enter PID of hello process: ")
    
    # 方法2: 直接启动进程
    # device = frida.get_local_device()
    # pid = device.spawn(["./hello"])
    
    session = frida.attach(int(pid))
    
    # 加载 JavaScript 脚本
    with open("hello_mod_simple.js", "r") as f:
        js_code = f.read()
    
    script = session.create_script(js_code)
    script.on('message', on_message)
    
    print("[*] Injecting script...")
    script.load()
    
    # 保持运行
    print("[*] Script injected. Press Ctrl+C to stop.")
    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        print("\n[*] Detaching...")
        session.detach()

if __name__ == "__main__":
    main()
```

### 6. 一步到位的完整脚本

```javascript
// one_shot_mod.js
'use strict';

// 直接运行这个脚本来修改
Interceptor.attach(Module.findExportByName(null, "printf") || 
                   Module.findExportByName(null, "puts"), {
    onEnter: function(args) {
        var str = Memory.readUtf8String(args[0]);
        if (!str) return;
        
        // 检查是否包含目标字符串
        if (str.indexOf("Hello World!") !== -1) {
            console.log(`[INTERCEPTED] ${str}`);
            
            // 替换
            var newStr = str.replace("Hello World!", "Good Job!!!");
            var newPtr = Memory.allocUtf8String(newStr);
            args[0] = newPtr;
            
            // 保存引用防止垃圾回收
            this.newPtr = newPtr;
        }
    }
});

console.log("[READY] String interceptor active!");
```

### 7. 使用方法

```bash
# 1. 编译并运行程序
gcc hello_simple.c -o hello -no-pie -fno-pie
./hello &
# 记下 PID，比如 12345

# 2. 运行 Frida 脚本
frida -p 12345 -l modify_hello.js

# 或者使用专用脚本
frida -p 12345 -l hello_mod_simple.js

# 3. 使用 Python 控制
python3 frida_hello.py
# 输入 PID: 12345

# 4. 一键修改
frida -p 12345 -l one_shot_mod.js
```

### 8. 验证效果

程序输出会从：
```
Current message: Hello World!
```

变为：
```
Current message: Good Job!!!
```

### 注意事项：

1. **编译选项**：使用 `-no-pie` 使地址可预测
2. **字符串长度**：新字符串不能比原字符串长
3. **内存保护**：可能需要修改内存权限
4. **局部变量**：栈上的变量地址每次运行可能不同
5. **优化**：如果编译时使用 `-O2`，字符串可能被优化掉

### 故障排除：

如果找不到字符串：
1. 检查字符串是否在堆/栈/全局区
2. 使用 `objdump -s hello` 查看二进制中的字符串
3. 尝试搜索 "Hello" 或 "World" 部分匹配
4. 查看字符串的十六进制表示：`echo -n "Hello World!" | xxd -p`

## 拦截文件

写一个完整的示例，包含 C 程序和 Frida 脚本，演示如何拦截文件存在性检查并根据参数返回不同结果。

### 1. C 程序代码

```c
// check_file.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

// 函数1: 使用 access 函数检查
int check_with_access(const char* path) {
    printf("[access] Checking: %s\n", path);
    int result = access(path, F_OK);
    if (result == 0) {
        printf("[access] ✓ File exists\n");
        return 1;
    } else {
        printf("[access] ✗ File does not exist (errno: %d - %s)\n", 
               errno, strerror(errno));
        return 0;
    }
}

// 函数2: 使用 stat 函数检查
int check_with_stat(const char* path) {
    printf("[stat] Checking: %s\n", path);
    struct stat buffer;
    int result = stat(path, &buffer);
    if (result == 0) {
        printf("[stat] ✓ File exists, size: %ld bytes\n", buffer.st_size);
        return 1;
    } else {
        printf("[stat] ✗ File does not exist (errno: %d - %s)\n", 
               errno, strerror(errno));
        return 0;
    }
}

// 函数3: 使用 fopen 检查
int check_with_fopen(const char* path) {
    printf("[fopen] Checking: %s\n", path);
    FILE* file = fopen(path, "r");
    if (file != NULL) {
        printf("[fopen] ✓ File exists and can be opened\n");
        fclose(file);
        return 1;
    } else {
        printf("[fopen] ✗ Cannot open file (errno: %d - %s)\n", 
               errno, strerror(errno));
        return 0;
    }
}

// 函数4: 使用 open 系统调用检查
int check_with_open(const char* path) {
    printf("[open] Checking: %s\n", path);
    int fd = open(path, O_RDONLY);
    if (fd >= 0) {
        printf("[open] ✓ File exists, fd: %d\n", fd);
        close(fd);
        return 1;
    } else {
        printf("[open] ✗ Cannot open file (errno: %d - %s)\n", 
               errno, strerror(errno));
        return 0;
    }
}

// 交互式检查函数
void interactive_check() {
    char input[256];
    char path[256] = "./hello.txt";
    
    while (1) {
        printf("\n=== File Checker ===\n");
        printf("Current target: %s\n", path);
        printf("1. Check with access()\n");
        printf("2. Check with stat()\n");
        printf("3. Check with fopen()\n");
        printf("4. Check with open()\n");
        printf("5. Change target file\n");
        printf("6. Exit\n");
        printf("Select (1-6): ");
        
        fgets(input, sizeof(input), stdin);
        
        switch (input[0]) {
            case '1':
                check_with_access(path);
                break;
            case '2':
                check_with_stat(path);
                break;
            case '3':
                check_with_fopen(path);
                break;
            case '4':
                check_with_open(path);
                break;
            case '5':
                printf("Enter new file path: ");
                fgets(path, sizeof(path), stdin);
                // 去掉换行符
                path[strcspn(path, "\n")] = 0;
                if (strlen(path) == 0) {
                    strcpy(path, "./hello.txt");
                }
                printf("New target: %s\n", path);
                break;
            case '6':
                printf("Exiting...\n");
                return;
            default:
                printf("Invalid option\n");
        }
        
        sleep(1);  // 方便观察
    }
}

// 命令行模式
void command_line_mode(int argc, char* argv[]) {
    printf("PID: %d\n", getpid());
    printf("Arguments: ");
    for (int i = 0; i < argc; i++) {
        printf("%s ", argv[i]);
    }
    printf("\n\n");
    
    char* target = "./hello.txt";
    if (argc > 1) {
        target = argv[1];
    }
    
    printf("Checking file: %s\n", target);
    printf("================================\n");
    
    int results[4];
    results[0] = check_with_access(target);
    results[1] = check_with_stat(target);
    results[2] = check_with_fopen(target);
    results[3] = check_with_open(target);
    
    printf("\n=== Summary ===\n");
    printf("access(): %s\n", results[0] ? "EXISTS" : "NOT EXISTS");
    printf("stat():   %s\n", results[1] ? "EXISTS" : "NOT EXISTS");
    printf("fopen():  %s\n", results[2] ? "EXISTS" : "NOT EXISTS");
    printf("open():   %s\n", results[3] ? "EXISTS" : "NOT EXISTS");
    
    int exists_count = 0;
    for (int i = 0; i < 4; i++) {
        if (results[i]) exists_count++;
    }
    
    printf("\nVerdict: %d/4 methods say the file %s\n", 
           exists_count, exists_count >= 2 ? "EXISTS" : "does NOT exist");
}

int main(int argc, char* argv[]) {
    printf("=== File Existence Checker ===\n");
    
    if (argc == 1) {
        // 交互模式
        interactive_check();
    } else if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        printf("Usage:\n");
        printf("  %s              - Interactive mode\n", argv[0]);
        printf("  %s <filepath>   - Check specific file\n", argv[0]);
        printf("  %s --test       - Run test suite\n", argv[0]);
        printf("  %s --help       - Show this help\n", argv[0]);
    } else if (argc == 2 && strcmp(argv[1], "--test") == 0) {
        printf("=== Running Test Suite ===\n");
        
        // 测试不同文件
        char* test_files[] = {
            "./hello.txt",
            "/etc/passwd",
            "/nonexistent/file",
            ".",
            "/dev/null",
            NULL
        };
        
        for (int i = 0; test_files[i] != NULL; i++) {
            printf("\n[Test %d] File: %s\n", i+1, test_files[i]);
            check_with_access(test_files[i]);
        }
    } else {
        // 命令行模式
        command_line_mode(argc, argv);
    }
    
    return 0;
}
```

### 2. Frida 拦截脚本

```javascript
// intercept_file_check.js
'use strict';

// 配置选项
var config = {
    // 目标文件名
    targetFile: "./hello.txt",
    
    // 返回结果模式
    // always_true: 总是返回存在
    // always_false: 总是返回不存在
    // random: 随机返回
    // toggle: 每次调用切换状态
    mode: "always_true",
    
    // 仅拦截特定函数
    targetFunctions: ["access", "stat", "fopen", "open"],
    
    // 详细日志
    verbose: true,
    
    // 保存原始函数指针
    originalFunctions: {}
};

// 统计信息
var stats = {
    intercepted: 0,
    access_calls: 0,
    stat_calls: 0,
    fopen_calls: 0,
    open_calls: 0,
    modified_results: 0
};

// 状态变量
var state = {
    toggleState: true  // 用于 toggle 模式
};

// 工具函数
function log(message) {
    if (config.verbose) {
        console.log("[*] " + message);
    }
}

function logCall(funcName, path, result) {
    console.log(`[${funcName}] ${path} -> ${result ? "EXISTS" : "NOT EXISTS"}`);
}

function shouldIntercept(funcName, path) {
    // 检查是否在目标函数列表中
    if (!config.targetFunctions.includes(funcName)) {
        return false;
    }
    
    // 检查路径是否匹配目标文件
    if (path && path.indexOf(config.targetFile) !== -1) {
        return true;
    }
    
    // 也可以匹配部分路径
    if (path && (path.endsWith("hello.txt") || 
                 path.indexOf("hello.txt") !== -1)) {
        return true;
    }
    
    return false;
}

function getFakeResult(funcName) {
    switch (config.mode) {
        case "always_true":
            return 0;  // 成功，表示文件存在
            
        case "always_false":
            return -1; // 失败，表示文件不存在
            
        case "random":
            return Math.random() > 0.5 ? 0 : -1;
            
        case "toggle":
            state.toggleState = !state.toggleState;
            return state.toggleState ? 0 : -1;
            
        case "sequence":
            // 按顺序：存在、不存在、存在、不存在...
            state.sequenceIndex = (state.sequenceIndex || 0) + 1;
            return state.sequenceIndex % 2 === 1 ? 0 : -1;
            
        default:
            return -1; // 默认返回不存在
    }
}

function setErrno(value) {
    // 设置 errno 值
    var errnoAddr = Module.findExportByName(null, "__errno_location");
    if (errnoAddr) {
        var errnoPtr = new NativePointer(errnoAddr);
        var errnoValue = errnoPtr.isNull() ? null : errnoPtr.readPointer();
        if (errnoValue) {
            errnoValue.writeInt(value);
        }
    }
}

// Hook access 函数
function hookAccess() {
    var accessAddr = Module.findExportByName(null, "access");
    if (!accessAddr) {
        console.log("[-] access function not found");
        return;
    }
    
    log(`access found at: ${accessAddr}`);
    config.originalFunctions.access = accessAddr;
    
    Interceptor.attach(accessAddr, {
        onEnter: function(args) {
            var path = Memory.readUtf8String(args[0]);
            var mode = args[1].toInt32();
            
            stats.access_calls++;
            
            if (shouldIntercept("access", path)) {
                this.shouldIntercept = true;
                this.path = path;
                this.mode = mode;
                stats.intercepted++;
                
                log(`Intercepting access: ${path} (mode: ${mode})`);
            }
        },
        onLeave: function(retval) {
            if (this.shouldIntercept) {
                var fakeResult = getFakeResult("access");
                
                // 设置返回值
                retval.replace(fakeResult);
                
                // 设置相应的 errno
                if (fakeResult === -1) {
                    setErrno(2);  // ENOENT: No such file or directory
                } else {
                    setErrno(0);  // Success
                }
                
                stats.modified_results++;
                logCall("access", this.path, fakeResult === 0);
            }
        }
    });
}

// Hook stat 函数
function hookStat() {
    // stat 有多个版本：stat, stat64, __xstat, etc.
    var statFunctions = ["stat", "stat64", "__xstat", "__xstat64"];
    
    statFunctions.forEach(function(funcName) {
        var funcAddr = Module.findExportByName(null, funcName);
        if (funcAddr) {
            log(`${funcName} found at: ${funcAddr}`);
            config.originalFunctions[funcName] = funcAddr;
            
            Interceptor.attach(funcAddr, {
                onEnter: function(args) {
                    var path = Memory.readUtf8String(args[0]);
                    
                    stats.stat_calls++;
                    
                    if (shouldIntercept("stat", path)) {
                        this.shouldIntercept = true;
                        this.path = path;
                        this.statBuffer = args[1];
                        stats.intercepted++;
                        
                        log(`Intercepting ${funcName}: ${path}`);
                    }
                },
                onLeave: function(retval) {
                    if (this.shouldIntercept) {
                        var fakeResult = getFakeResult("stat");
                        
                        // 设置返回值
                        retval.replace(fakeResult);
                        
                        // 如果模拟文件存在，填充 stat 结构体
                        if (fakeResult === 0 && this.statBuffer) {
                            try {
                                // 填充一些假数据
                                Memory.writeS64(this.statBuffer, 0x1000);        // st_dev
                                Memory.writeU64(this.statBuffer.add(8), 0x1001); // st_ino
                                Memory.writeU32(this.statBuffer.add(16), 0x81A4);// st_mode (regular file)
                                Memory.writeU32(this.statBuffer.add(20), 0x1);   // st_nlink
                                Memory.writeU32(this.statBuffer.add(24), 0);     // st_uid
                                Memory.writeU32(this.statBuffer.add(28), 0);     // st_gid
                                Memory.writeS64(this.statBuffer.add(32), 0);     // st_rdev
                                Memory.writeS64(this.statBuffer.add(40), 1024);  // st_size (1KB)
                                // ... 继续填充其他字段
                            } catch(e) {
                                log(`Error filling stat buffer: ${e}`);
                            }
                        } else if (fakeResult === -1) {
                            setErrno(2);  // ENOENT
                        }
                        
                        stats.modified_results++;
                        logCall("stat", this.path, fakeResult === 0);
                    }
                }
            });
        }
    });
}

// Hook fopen 函数
function hookFopen() {
    var fopenAddr = Module.findExportByName(null, "fopen");
    if (!fopenAddr) {
        console.log("[-] fopen function not found");
        return;
    }
    
    log(`fopen found at: ${fopenAddr}`);
    config.originalFunctions.fopen = fopenAddr;
    
    Interceptor.attach(fopenAddr, {
        onEnter: function(args) {
            var path = Memory.readUtf8String(args[0]);
            var mode = Memory.readUtf8String(args[1]);
            
            stats.fopen_calls++;
            
            if (shouldIntercept("fopen", path)) {
                this.shouldIntercept = true;
                this.path = path;
                this.mode = mode;
                stats.intercepted++;
                
                log(`Intercepting fopen: ${path} (mode: ${mode})`);
            }
        },
        onLeave: function(retval) {
            if (this.shouldIntercept) {
                var fakeResult = getFakeResult("fopen");
                
                if (fakeResult === 0) {
                    // 模拟成功 - 返回一个假的 FILE 指针
                    // 创建一个假的 FILE 结构体（简化版）
                    var fakeFilePtr = Memory.alloc(256);  // 分配一些内存作为假 FILE
                    Memory.writeU32(fakeFilePtr, 0x12345678);  // 一些假数据
                    retval.replace(fakeFilePtr);
                    
                    // 保存指针以便后续处理
                    this.fakeFilePtr = fakeFilePtr;
                } else {
                    // 模拟失败 - 返回 NULL
                    retval.replace(ptr(0));
                    setErrno(2);  // ENOENT
                }
                
                stats.modified_results++;
                logCall("fopen", this.path, fakeResult === 0);
            }
        }
    });
}

// Hook open 函数
function hookOpen() {
    var openAddr = Module.findExportByName(null, "open");
    if (!openAddr) {
        // 尝试 open64
        openAddr = Module.findExportByName(null, "open64");
    }
    
    if (!openAddr) {
        console.log("[-] open function not found");
        return;
    }
    
    log(`open found at: ${openAddr}`);
    config.originalFunctions.open = openAddr;
    
    Interceptor.attach(openAddr, {
        onEnter: function(args) {
            var path = Memory.readUtf8String(args[0]);
            var flags = args[1].toInt32();
            
            stats.open_calls++;
            
            if (shouldIntercept("open", path)) {
                this.shouldIntercept = true;
                this.path = path;
                this.flags = flags;
                stats.intercepted++;
                
                log(`Intercepting open: ${path} (flags: 0x${flags.toString(16)})`);
            }
        },
        onLeave: function(retval) {
            if (this.shouldIntercept) {
                var fakeResult = getFakeResult("open");
                
                if (fakeResult === 0) {
                    // 模拟成功 - 返回一个假的文件描述符
                    var fakeFd = 999;  // 一个高数值的假 fd
                    retval.replace(fakeFd);
                } else {
                    // 模拟失败 - 返回 -1
                    retval.replace(-1);
                    setErrno(2);  // ENOENT
                }
                
                stats.modified_results++;
                logCall("open", this.path, fakeResult === 0);
            }
        }
    });
}

// Hook __errno_location 来监控 errno
function hookErrnoLocation() {
    var errnoAddr = Module.findExportByName(null, "__errno_location");
    if (errnoAddr) {
        log(`__errno_location found at: ${errnoAddr}`);
        
        Interceptor.attach(errnoAddr, {
            onLeave: function(retval) {
                // 记录 errno 的变化
                this.errnoPtr = retval;
                // 可以在这里修改 errno 值
            }
        });
    }
}

// 动态配置函数
function configure(newConfig) {
    if (newConfig.mode) {
        config.mode = newConfig.mode;
        log(`Mode changed to: ${config.mode}`);
    }
    
    if (newConfig.targetFile) {
        config.targetFile = newConfig.targetFile;
        log(`Target file changed to: ${config.targetFile}`);
    }
    
    if (newConfig.verbose !== undefined) {
        config.verbose = newConfig.verbose;
    }
}

// RPC 接口 - 允许外部控制
rpc.exports = {
    // 获取当前配置
    getConfig: function() {
        return config;
    },
    
    // 更新配置
    setConfig: function(newConfig) {
        configure(newConfig);
        return "OK";
    },
    
    // 获取统计信息
    getStats: function() {
        return stats;
    },
    
    // 重置统计
    resetStats: function() {
        stats = {
            intercepted: 0,
            access_calls: 0,
            stat_calls: 0,
            fopen_calls: 0,
            open_calls: 0,
            modified_results: 0
        };
        return "OK";
    },
    
    // 设置特定模式
    setMode: function(mode) {
        config.mode = mode;
        return `Mode set to: ${mode}`;
    },
    
    // 设置目标文件
    setTarget: function(filepath) {
        config.targetFile = filepath;
        return `Target set to: ${filepath}`;
    },
    
    // 手动触发一次检查
    testHook: function() {
        log("Manual test triggered");
        return "Test completed";
    }
};

// 显示 banner
function showBanner() {
    console.log("\n" +
        "╔══════════════════════════════════════════════════╗\n" +
        "║        FILE CHECK INTERCEPTOR - Frida v1.0       ║\n" +
        "║                                                  ║\n" +
        "║  Target: " + config.targetFile.padEnd(35) + "║\n" +
        "║  Mode:   " + config.mode.padEnd(35) + "║\n" +
        "║  PID:    " + Process.id.toString().padEnd(35) + "║\n" +
        "╚══════════════════════════════════════════════════╝\n");
}

// 初始化函数
function init() {
    showBanner();
    
    log("Starting hooks installation...");
    
    // 安装所有钩子
    hookAccess();
    hookStat();
    hookFopen();
    hookOpen();
    hookErrnoLocation();
    
    log("All hooks installed successfully");
    log("Use rpc.exports to control the interceptor");
    
    // 定期显示状态
    setInterval(function() {
        if (config.verbose && stats.intercepted > 0) {
            console.log(`\n[Status] Intercepted: ${stats.intercepted}, Modified: ${stats.modified_results}`);
            console.log(`         Access: ${stats.access_calls}, Stat: ${stats.stat_calls}`);
            console.log(`         Fopen: ${stats.fopen_calls}, Open: ${stats.open_calls}`);
        }
    }, 5000);
}

// 启动
setTimeout(init, 100);
```

### 3. Python 控制脚本

```python
#!/usr/bin/env python3
# control_file_intercept.py

import frida
import sys
import time
import json
import threading
from cmd import Cmd

class FileInterceptorControl(Cmd):
    intro = "\nFile Interceptor Control Console\nType 'help' or '?' for commands\n"
    prompt = "interceptor> "
    
    def __init__(self, session):
        super().__init__()
        self.session = session
        self.script = None
        self.rpc = None
        
    def preloop(self):
        # 加载脚本
        with open("intercept_file_check.js", "r") as f:
            js_code = f.read()
        
        self.script = self.session.create_script(js_code)
        
        def on_message(message, data):
            if message['type'] == 'send':
                print(f"[*] {message['payload']}")
            elif message['type'] == 'error':
                print(f"[-] Error: {message['description']}")
        
        self.script.on('message', on_message)
        self.script.load()
        time.sleep(1)
        
        # 获取 RPC 接口
        self.rpc = self.script.exports
        
        # 显示初始状态
        config = self.rpc.get_config()
        print(f"\n[*] Connected to PID: {self.session.pid}")
        print(f"[*] Target file: {config['targetFile']}")
        print(f"[*] Mode: {config['mode']}")
    
    def do_config(self, arg):
        """显示当前配置"""
        config = self.rpc.get_config()
        print("\nCurrent configuration:")
        print(json.dumps(config, indent=2))
    
    def do_stats(self, arg):
        """显示统计信息"""
        stats = self.rpc.get_stats()
        print("\nStatistics:")
        print(json.dumps(stats, indent=2))
    
    def do_mode(self, arg):
        """设置模式: always_true, always_false, random, toggle"""
        if not arg:
            print("Usage: mode <always_true|always_false|random|toggle>")
            return
        
        result = self.rpc.set_mode(arg)
        print(f"[*] {result}")
    
    def do_target(self, arg):
        """设置目标文件路径"""
        if not arg:
            print("Usage: target <filepath>")
            return
        
        result = self.rpc.set_target(arg)
        print(f"[*] {result}")
    
    def do_reset(self, arg):
        """重置统计信息"""
        result = self.rpc.reset_stats()
        print(f"[*] {result}")
    
    def do_test(self, arg):
        """手动触发测试"""
        result = self.rpc.test_hook()
        print(f"[*] {result}")
    
    def do_verbose(self, arg):
        """切换详细日志模式"""
        config = self.rpc.get_config()
        new_verbose = not config['verbose']
        self.rpc.set_config({"verbose": new_verbose})
        print(f"[*] Verbose mode: {'ON' if new_verbose else 'OFF'}")
    
    def do_status(self, arg):
        """显示状态"""
        config = self.rpc.get_config()
        stats = self.rpc.get_stats()
        
        print("\n" + "="*50)
        print(f"PID: {self.session.pid}")
        print(f"Target: {config['targetFile']}")
        print(f"Mode: {config['mode']}")
        print(f"Verbose: {config['verbose']}")
        print("-"*50)
        print(f"Total intercepted: {stats['intercepted']}")
        print(f"Modified results: {stats['modified_results']}")
        print(f"Access calls: {stats['access_calls']}")
        print(f"Stat calls: {stats['stat_calls']}")
        print(f"Fopen calls: {stats['fopen_calls']}")
        print(f"Open calls: {stats['open_calls']}")
        print("="*50)
    
    def do_exit(self, arg):
        """退出控制台"""
        print("[*] Exiting...")
        return True
    
    def do_quit(self, arg):
        """退出控制台"""
        return self.do_exit(arg)

def main():
    if len(sys.argv) != 2:
        print("Usage: python3 control_file_intercept.py <pid>")
        print("Example: python3 control_file_intercept.py 12345")
        sys.exit(1)
    
    pid = int(sys.argv[1])
    
    try:
        # 连接到进程
        session = frida.attach(pid)
        print(f"[*] Attached to PID {pid}")
        
        # 启动控制台
        control = FileInterceptorControl(session)
        control.cmdloop()
        
    except Exception as e:
        print(f"[-] Error: {e}")
    finally:
        if 'session' in locals():
            session.detach()

if __name__ == "__main__":
    main()
```

### 4. 使用示例脚本

```javascript
// quick_intercept.js
// 快速使用脚本 - 一键拦截

Interceptor.attach(Module.findExportByName(null, "access"), {
    onEnter: function(args) {
        var path = Memory.readUtf8String(args[0]);
        if (path && (path.indexOf("hello.txt") !== -1 || path === "./hello.txt")) {
            console.log(`[!] Intercepting access: ${path}`);
            this.shouldHook = true;
            this.path = path;
        }
    },
    onLeave: function(retval) {
        if (this.shouldHook) {
            // 总是返回存在
            retval.replace(0);
            console.log(`[*] access(${this.path}) -> EXISTS`);
        }
    }
});

console.log("[READY] File access interceptor active!");
```

### 5. 编译和运行步骤

```bash
# 1. 编译 C 程序
gcc check_file.c -o check_file

# 2. 创建测试文件（可选）
echo "Hello" > hello.txt

# 3. 运行程序
./check_file &
# 记下 PID，例如 12345

# 或指定文件检查
./check_file ./hello.txt &
./check_file /nonexistent/file &

# 4. 使用 Frida 拦截
# 方法1: 使用完整脚本
frida -p 12345 -l intercept_file_check.js

# 方法2: 使用快速脚本
frida -p 12345 -l quick_intercept.js

# 方法3: 使用 Python 控制台
python3 control_file_intercept.py 12345
```

### 6. 测试不同场景

```bash
# 场景1: 文件存在，但拦截返回不存在
# 先创建文件
touch hello.txt
./check_file hello.txt &
# 然后用 Frida 设置为 always_false 模式

# 场景2: 文件不存在，但拦截返回存在
rm -f hello.txt
./check_file hello.txt &
# 然后用 Frida 设置为 always_true 模式

# 场景3: 随机结果
./check_file &
# 然后用 Frida 设置为 random 模式

# 场景4: 切换结果
./check_file &
# 然后用 Frida 设置为 toggle 模式
```

### 7. Python 自动化测试

```python
#!/usr/bin/env python3
# test_automation.py

import frida
import time
import subprocess
import sys

def test_scenario(name, target_file, create_file, frida_mode):
    print(f"\n{'='*60}")
    print(f"Test: {name}")
    print(f"File: {target_file} (create: {create_file})")
    print(f"Frida mode: {frida_mode}")
    print(f"{'='*60}")
    
    # 准备文件
    if create_file:
        with open(target_file, 'w') as f:
            f.write("test")
    else:
        try:
            import os
            os.remove(target_file)
        except:
            pass
    
    # 启动目标程序
    proc = subprocess.Popen(["./check_file", target_file], 
                           stdout=subprocess.PIPE, 
                           stderr=subprocess.PIPE,
                           text=True)
    time.sleep(1)
    
    try:
        # 连接 Frida
        session = frida.attach(proc.pid)
        
        # 加载拦截脚本
        with open("intercept_file_check.js", "r") as f:
            js_code = f.read()
        
        script = session.create_script(js_code)
        
        def on_message(message, data):
            if message['type'] == 'send':
                print(f"[Frida] {message['payload']}")
        
        script.on('message', on_message)
        script.load()
        
        # 设置模式
        script.exports.set_mode(frida_mode)
        
        # 让程序运行一会儿
        time.sleep(3)
        
        # 读取输出
        proc.terminate()
        stdout, stderr = proc.communicate()
        
        print("\nProgram output:")
        print(stdout)
        
        # 检查结果
        if frida_mode == "always_true":
            assert "✓" in stdout or "EXISTS" in stdout
            print("[PASS] File correctly reported as EXISTS")
        elif frida_mode == "always_false":
            assert "✗" in stdout or "NOT EXISTS" in stdout
            print("[PASS] File correctly reported as NOT EXISTS")
        
        session.detach()
        
    except Exception as e:
        print(f"[ERROR] {e}")
        proc.terminate()
    
    print(f"{'='*60}")

def main():
    # 编译程序
    subprocess.run(["gcc", "check_file.c", "-o", "check_file"], check=True)
    
    test_files = [
        ("./hello.txt", True, "File exists but report missing"),
        ("./hello.txt", False, "File missing but report exists"),
        ("/tmp/test.txt", True, "Random results"),
        ("/tmp/test.txt", False, "Toggle results")
    ]
    
    modes = ["always_true", "always_false", "random", "toggle"]
    
    for i, (target_file, create_file, description) in enumerate(test_files[:2]):
        for mode in modes[:2]:
            test_scenario(
                f"{description} ({mode})",
                target_file,
                create_file,
                mode
            )
            time.sleep(2)

if __name__ == "__main__":
    main()
```

### 8. 高级功能扩展

```javascript
// advanced_intercept.js
// 高级功能：根据调用者决定返回值

// 跟踪调用栈
function getCallerInfo() {
    var stack = Thread.backtrace(this.context, Backtracer.ACCURATE);
    if (stack.length > 1) {
        var caller = DebugSymbol.fromAddress(stack[1]);
        return caller.name || caller.address;
    }
    return "unknown";
}

// 根据调用者返回不同结果
Interceptor.attach(Module.findExportByName(null, "access"), {
    onEnter: function(args) {
        var path = Memory.readUtf8String(args[0]);
        if (path && path.indexOf("hello.txt") !== -1) {
            this.caller = getCallerInfo();
            console.log(`[Caller: ${this.caller}] access(${path})`);
        }
    },
    onLeave: function(retval) {
        if (this.caller) {
            // 如果是从 check_with_access 调用的，返回存在
            // 否则返回不存在
            if (this.caller.indexOf("check_with_access") !== -1) {
                retval.replace(0);  // EXISTS
                console.log(`[*] Returning EXISTS for ${this.caller}`);
            } else {
                retval.replace(-1); // NOT EXISTS
                console.log(`[*] Returning NOT EXISTS for ${this.caller}`);
            }
        }
    }
});
```

这个完整的示例展示了：
1. 如何编写检查文件存在的 C 程序
2. 如何用 Frida 拦截多种文件检查函数
3. 如何根据参数返回不同结果
4. 如何通过 Python 控制 Frida 脚本
5. 如何测试不同场景

你可以通过修改 `config.mode` 来控制返回结果：
- `always_true`: 总是返回文件存在
- `always_false`: 总是返回文件不存在  
- `random`: 随机返回
- `toggle`: 每次调用切换结果

## 拦截 C 语言的自定义方法

### 1. C 程序 - 自定义数组操作

```c
// custom_array.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// 自定义数组结构
typedef struct {
    int* data;
    int size;
    int capacity;
} IntArray;

// 自定义字符串数组
typedef struct {
    char** strings;
    int size;
    int capacity;
} StringArray;

// 初始化数组
IntArray* create_int_array(int initial_capacity) {
    IntArray* arr = (IntArray*)malloc(sizeof(IntArray));
    arr->data = (int*)malloc(initial_capacity * sizeof(int));
    arr->size = 0;
    arr->capacity = initial_capacity;
    return arr;
}

StringArray* create_string_array(int initial_capacity) {
    StringArray* arr = (StringArray*)malloc(sizeof(StringArray));
    arr->strings = (char**)malloc(initial_capacity * sizeof(char*));
    arr->size = 0;
    arr->capacity = initial_capacity;
    return arr;
}

// 添加元素
void int_array_append(IntArray* arr, int value) {
    if (arr->size >= arr->capacity) {
        arr->capacity *= 2;
        arr->data = (int*)realloc(arr->data, arr->capacity * sizeof(int));
    }
    arr->data[arr->size++] = value;
}

void string_array_append(StringArray* arr, const char* value) {
    if (arr->size >= arr->capacity) {
        arr->capacity *= 2;
        arr->strings = (char**)realloc(arr->strings, arr->capacity * sizeof(char*));
    }
    arr->strings[arr->size] = strdup(value);
    arr->size++;
}

// 自定义的 hello 方法 - 打印数组
void hello_print_int_array(IntArray* arr) {
    printf("[hello_int_array] Size: %d, Capacity: %d\n", arr->size, arr->capacity);
    printf("Elements: [");
    for (int i = 0; i < arr->size; i++) {
        printf("%d", arr->data[i]);
        if (i < arr->size - 1) printf(", ");
    }
    printf("]\n");
}

void hello_print_string_array(StringArray* arr) {
    printf("[hello_string_array] Size: %d, Capacity: %d\n", arr->size, arr->capacity);
    printf("Elements: [");
    for (int i = 0; i < arr->size; i++) {
        printf("\"%s\"", arr->strings[i]);
        if (i < arr->size - 1) printf(", ");
    }
    printf("]\n");
}

// 自定义的切片方法（原始实现）
IntArray* hello_slice_int(IntArray* arr, int start, int end) {
    printf("[original_slice] Slicing int array [%d:%d]\n", start, end);
    
    if (start < 0) start = 0;
    if (end > arr->size) end = arr->size;
    if (start >= end) return create_int_array(1);
    
    IntArray* sliced = create_int_array(end - start);
    for (int i = start; i < end; i++) {
        int_array_append(sliced, arr->data[i]);
    }
    return sliced;
}

StringArray* hello_slice_string(StringArray* arr, int start, int end) {
    printf("[original_slice] Slicing string array [%d:%d]\n", start, end);
    
    if (start < 0) start = 0;
    if (end > arr->size) end = arr->size;
    if (start >= end) return create_string_array(1);
    
    StringArray* sliced = create_string_array(end - start);
    for (int i = start; i < end; i++) {
        string_array_append(sliced, arr->strings[i]);
    }
    return sliced;
}

// 带步长的切片（复杂版本）
IntArray* hello_slice_with_step(IntArray* arr, int start, int end, int step) {
    printf("[original_slice_with_step] Slicing [%d:%d:%d]\n", start, end, step);
    
    if (step == 0) step = 1;
    if (step < 0) {
        // 负步长反向切片
        if (start < 0) start = arr->size + start;
        if (end < 0) end = arr->size + end;
        
        IntArray* sliced = create_int_array(1);
        for (int i = start; i > end; i += step) {
            int_array_append(sliced, arr->data[i]);
        }
        return sliced;
    } else {
        // 正步长
        if (start < 0) start = 0;
        if (end > arr->size) end = arr->size;
        
        IntArray* sliced = create_int_array(1);
        for (int i = start; i < end; i += step) {
            int_array_append(sliced, arr->data[i]);
        }
        return sliced;
    }
}

// 获取元素（带负数索引支持）
int hello_get_int(IntArray* arr, int index) {
    if (index < 0) index = arr->size + index;
    if (index < 0 || index >= arr->size) {
        printf("[ERROR] Index %d out of bounds (size: %d)\n", index, arr->size);
        return 0;
    }
    return arr->data[index];
}

char* hello_get_string(StringArray* arr, int index) {
    if (index < 0) index = arr->size + index;
    if (index < 0 || index >= arr->size) {
        printf("[ERROR] Index %d out of bounds (size: %d)\n", index, arr->size);
        return NULL;
    }
    return arr->strings[index];
}

// 释放内存
void free_int_array(IntArray* arr) {
    if (arr) {
        free(arr->data);
        free(arr);
    }
}

void free_string_array(StringArray* arr) {
    if (arr) {
        for (int i = 0; i < arr->size; i++) {
            free(arr->strings[i]);
        }
        free(arr->strings);
        free(arr);
    }
}

// 演示函数
void demo_int_array() {
    printf("\n=== Int Array Demo ===\n");
    
    IntArray* arr = create_int_array(5);
    for (int i = 0; i < 10; i++) {
        int_array_append(arr, i * 10);
    }
    
    hello_print_int_array(arr);
    
    // 测试各种切片
    IntArray* slice1 = hello_slice_int(arr, 2, 5);
    hello_print_int_array(slice1);
    free_int_array(slice1);
    
    IntArray* slice2 = hello_slice_with_step(arr, 1, 9, 2);
    hello_print_int_array(slice2);
    free_int_array(slice2);
    
    // 测试负数索引
    printf("arr[-1] = %d\n", hello_get_int(arr, -1));
    printf("arr[-3] = %d\n", hello_get_int(arr, -3));
    
    free_int_array(arr);
}

void demo_string_array() {
    printf("\n=== String Array Demo ===\n");
    
    StringArray* arr = create_string_array(5);
    string_array_append(arr, "apple");
    string_array_append(arr, "banana");
    string_array_append(arr, "cherry");
    string_array_append(arr, "date");
    string_array_append(arr, "elderberry");
    string_array_append(arr, "fig");
    string_array_append(arr, "grape");
    
    hello_print_string_array(arr);
    
    StringArray* slice1 = hello_slice_string(arr, 1, 4);
    hello_print_string_array(slice1);
    free_string_array(slice1);
    
    printf("arr[-2] = %s\n", hello_get_string(arr, -2));
    printf("arr[2] = %s\n", hello_get_string(arr, 2));
    
    free_string_array(arr);
}

// 交互式测试
void interactive_test() {
    printf("=== Interactive Array Slice Test ===\n");
    printf("PID: %d\n", getpid());
    
    IntArray* arr = create_int_array(10);
    for (int i = 0; i < 20; i++) {
        int_array_append(arr, i * 5);
    }
    
    while (1) {
        printf("\nCurrent array: ");
        hello_print_int_array(arr);
        
        printf("\nOptions:\n");
        printf("1. Get element by index\n");
        printf("2. Slice array [start:end]\n");
        printf("3. Slice with step [start:end:step]\n");
        printf("4. Exit\n");
        printf("Choice: ");
        
        char input[100];
        fgets(input, sizeof(input), stdin);
        
        switch (input[0]) {
            case '1': {
                printf("Enter index: ");
                fgets(input, sizeof(input), stdin);
                int idx = atoi(input);
                int value = hello_get_int(arr, idx);
                printf("arr[%d] = %d\n", idx, value);
                break;
            }
            case '2': {
                printf("Enter start end: ");
                fgets(input, sizeof(input), stdin);
                int start, end;
                sscanf(input, "%d %d", &start, &end);
                IntArray* slice = hello_slice_int(arr, start, end);
                hello_print_int_array(slice);
                free_int_array(slice);
                break;
            }
            case '3': {
                printf("Enter start end step: ");
                fgets(input, sizeof(input), stdin);
                int start, end, step;
                sscanf(input, "%d %d %d", &start, &end, &step);
                IntArray* slice = hello_slice_with_step(arr, start, end, step);
                hello_print_int_array(slice);
                free_int_array(slice);
                break;
            }
            case '4':
                free_int_array(arr);
                return;
            default:
                printf("Invalid option\n");
        }
        
        sleep(1); // 方便 Frida 拦截
    }
}

int main(int argc, char* argv[]) {
    printf("=== Custom Array Slice Demo ===\n");
    
    if (argc > 1 && strcmp(argv[1], "--demo") == 0) {
        demo_int_array();
        demo_string_array();
    } else if (argc > 1 && strcmp(argv[1], "--test") == 0) {
        // 自动化测试
        IntArray* test_arr = create_int_array(5);
        for (int i = 0; i < 8; i++) {
            int_array_append(test_arr, i);
        }
        
        printf("Test array created: ");
        hello_print_int_array(test_arr);
        
        // 测试各种切片
        printf("\nTesting slices:\n");
        
        IntArray* slices[] = {
            hello_slice_int(test_arr, 2, 5),      // [2:5]
            hello_slice_int(test_arr, -3, -1),    // [-3:-1]
            hello_slice_with_step(test_arr, 0, 8, 2),  // [0:8:2]
            hello_slice_with_step(test_arr, 7, 0, -1), // [7:0:-1]
            NULL
        };
        
        for (int i = 0; slices[i] != NULL; i++) {
            hello_print_int_array(slices[i]);
            free_int_array(slices[i]);
        }
        
        free_int_array(test_arr);
    } else {
        interactive_test();
    }
    
    return 0;
}
```

### 2. Frida 拦截脚本 - 实现高级切片功能

```javascript
// intercept_custom_slice.js
'use strict';

// 配置
var config = {
    // 切片行为配置
    sliceBehavior: {
        // 是否支持负数索引
        supportNegativeIndex: true,
        // 是否支持超出范围的索引（自动截断）
        autoTruncate: true,
        // 默认步长
        defaultStep: 1,
        // 是否启用高级切片功能
        enableAdvancedSlice: true,
        // 切片结果模式: normal, reverse, random, double
        sliceMode: "normal"
    },
    
    // 要拦截的函数
    targetFunctions: [
        "hello_slice_int",
        "hello_slice_string",
        "hello_slice_with_step",
        "hello_get_int",
        "hello_get_string"
    ],
    
    // 调试选项
    debug: true,
    logArguments: true,
    logResults: true
};

// 统计
var stats = {
    calls: 0,
    slices: 0,
    gets: 0,
    modified: 0,
    errors: 0
};

// 工具函数
function log(message) {
    if (config.debug) {
        console.log(`[SLICE] ${message}`);
    }
}

function hexDump(ptr, size) {
    if (config.debug) {
        console.log(hexdump(ptr, { offset: 0, length: size, header: true, ansi: true }));
    }
}

// 解析 IntArray 结构体
function readIntArray(ptr) {
    try {
        // 假设结构体: data*, size, capacity
        var dataPtr = ptr.readPointer();
        var size = ptr.add(Process.pointerSize).readInt();
        var capacity = ptr.add(Process.pointerSize * 2).readInt();
        
        return {
            dataPtr: dataPtr,
            size: size,
            capacity: capacity,
            readData: function() {
                var data = [];
                for (var i = 0; i < this.size; i++) {
                    data.push(this.dataPtr.add(i * 4).readInt());
                }
                return data;
            }
        };
    } catch(e) {
        log(`Error reading IntArray: ${e}`);
        return null;
    }
}

// 解析 StringArray 结构体
function readStringArray(ptr) {
    try {
        var stringsPtr = ptr.readPointer();
        var size = ptr.add(Process.pointerSize).readInt();
        var capacity = ptr.add(Process.pointerSize * 2).readInt();
        
        return {
            stringsPtr: stringsPtr,
            size: size,
            capacity: capacity,
            readStrings: function() {
                var strings = [];
                for (var i = 0; i < this.size; i++) {
                    var strPtr = this.stringsPtr.add(i * Process.pointerSize).readPointer();
                    strings.push(strPtr.readUtf8String());
                }
                return strings;
            }
        };
    } catch(e) {
        log(`Error reading StringArray: ${e}`);
        return null;
    }
}

// 创建假的 IntArray
function createFakeIntArray(data) {
    // 分配内存: IntArray 结构体 + 数据
    var arrSize = Process.pointerSize * 3; // data*, size, capacity
    var dataSize = data.length * 4;
    
    var arrPtr = Memory.alloc(arrSize);
    var dataPtr = Memory.alloc(dataSize);
    
    // 写入数据
    for (var i = 0; i < data.length; i++) {
        dataPtr.add(i * 4).writeInt(data[i]);
    }
    
    // 设置结构体字段
    arrPtr.writePointer(dataPtr);
    arrPtr.add(Process.pointerSize).writeInt(data.length);
    arrPtr.add(Process.pointerSize * 2).writeInt(data.length);
    
    return arrPtr;
}

// 创建假的 StringArray
function createFakeStringArray(strings) {
    var arrPtr = Memory.alloc(Process.pointerSize * 3);
    var stringsPtr = Memory.alloc(strings.length * Process.pointerSize);
    
    // 分配并存储字符串
    for (var i = 0; i < strings.length; i++) {
        var strPtr = Memory.allocUtf8String(strings[i]);
        stringsPtr.add(i * Process.pointerSize).writePointer(strPtr);
    }
    
    arrPtr.writePointer(stringsPtr);
    arrPtr.add(Process.pointerSize).writeInt(strings.length);
    arrPtr.add(Process.pointerSize * 2).writeInt(strings.length);
    
    return arrPtr;
}

// Python 风格的切片计算
function calculateSliceIndices(size, start, end, step) {
    // 处理负数索引
    if (start < 0) start = size + start;
    if (end < 0) end = size + end;
    
    // 自动截断
    if (config.sliceBehavior.autoTruncate) {
        if (start < 0) start = 0;
        if (end > size) end = size;
        if (start > size) start = size;
        if (end < 0) end = 0;
    }
    
    // 处理步长
    if (step === undefined) step = config.sliceBehavior.defaultStep;
    
    // 确保 start 和 end 在步长方向上合理
    if (step > 0) {
        if (start < 0) start = 0;
        if (end > size) end = size;
        if (start > end) return { indices: [], length: 0 };
    } else if (step < 0) {
        if (start >= size) start = size - 1;
        if (end < -1) end = -1;
        if (start < end) return { indices: [], length: 0 };
    }
    
    // 计算实际索引
    var indices = [];
    if (step > 0) {
        for (var i = start; i < end; i += step) {
            indices.push(i);
        }
    } else {
        for (var i = start; i > end; i += step) {
            indices.push(i);
        }
    }
    
    return {
        indices: indices,
        length: indices.length,
        start: start,
        end: end,
        step: step
    };
}

// 高级切片算法
function advancedSlice(data, sliceInfo, mode) {
    var indices = sliceInfo.indices;
    var result = [];
    
    switch (mode) {
        case "normal":
            // 正常切片
            for (var i = 0; i < indices.length; i++) {
                if (indices[i] >= 0 && indices[i] < data.length) {
                    result.push(data[indices[i]]);
                }
            }
            break;
            
        case "reverse":
            // 反转切片
            for (var i = 0; i < indices.length; i++) {
                if (indices[i] >= 0 && indices[i] < data.length) {
                    result.push(data[indices[i]]);
                }
            }
            result.reverse();
            break;
            
        case "random":
            // 随机元素（保持顺序）
            for (var i = 0; i < indices.length; i++) {
                if (indices[i] >= 0 && indices[i] < data.length) {
                    // 50% 概率替换为随机值
                    if (Math.random() > 0.5) {
                        result.push(Math.floor(Math.random() * 100));
                    } else {
                        result.push(data[indices[i]]);
                    }
                }
            }
            break;
            
        case "double":
            // 每个元素重复一次
            for (var i = 0; i < indices.length; i++) {
                if (indices[i] >= 0 && indices[i] < data.length) {
                    result.push(data[indices[i]]);
                    result.push(data[indices[i]] * 2);
                }
            }
            break;
            
        case "fibonacci":
            // 用斐波那契数列替换
            var fib = [0, 1];
            for (var i = 0; i < indices.length; i++) {
                if (i >= fib.length) {
                    fib.push(fib[i-1] + fib[i-2]);
                }
                result.push(fib[i]);
            }
            break;
            
        default:
            // 默认正常切片
            for (var i = 0; i < indices.length; i++) {
                if (indices[i] >= 0 && indices[i] < data.length) {
                    result.push(data[indices[i]]);
                }
            }
    }
    
    return result;
}

// Hook hello_slice_int
function hookHelloSliceInt() {
    var symbol = Module.findExportByName(null, "hello_slice_int");
    if (!symbol) {
        log("hello_slice_int not found, trying to find by pattern...");
        // 尝试通过模式查找
        var matches = [];
        Module.enumerateExports("").forEach(function(exp) {
            if (exp.name.indexOf("slice") !== -1 || exp.name.indexOf("hello") !== -1) {
                matches.push(exp);
            }
        });
        
        if (matches.length > 0) {
            symbol = matches[0].address;
            log(`Found similar function: ${matches[0].name} at ${symbol}`);
        }
    }
    
    if (symbol) {
        log(`Hooking hello_slice_int at ${symbol}`);
        
        Interceptor.attach(symbol, {
            onEnter: function(args) {
                stats.calls++;
                stats.slices++;
                
                this.originalArrPtr = args[0];
                this.start = args[1].toInt32();
                this.end = args[2].toInt32();
                
                // 读取原始数组数据
                var arr = readIntArray(this.originalArrPtr);
                if (arr) {
                    this.originalData = arr.readData();
                    
                    if (config.logArguments) {
                        log(`hello_slice_int called:`);
                        log(`  Array: [${this.originalData.join(', ')}]`);
                        log(`  Slice: [${this.start}:${this.end}]`);
                    }
                    
                    // 计算 Python 风格的切片
                    var sliceInfo = calculateSliceIndices(
                        arr.size, 
                        this.start, 
                        this.end
                    );
                    
                    // 应用高级切片
                    var slicedData = advancedSlice(
                        this.originalData, 
                        sliceInfo, 
                        config.sliceBehavior.sliceMode
                    );
                    
                    this.shouldIntercept = true;
                    this.slicedData = slicedData;
                    this.sliceInfo = sliceInfo;
                    
                    log(`  Calculated indices: [${sliceInfo.indices.join(', ')}]`);
                    log(`  Result data: [${slicedData.join(', ')}]`);
                }
            },
            onLeave: function(retval) {
                if (this.shouldIntercept && this.slicedData) {
                    // 创建并返回假的切片数组
                    var fakeSlicePtr = createFakeIntArray(this.slicedData);
                    retval.replace(fakeSlicePtr);
                    
                    stats.modified++;
                    log(`  Returned modified slice with ${this.slicedData.length} elements`);
                }
            }
        });
    } else {
        log("Could not find hello_slice_int");
    }
}

// Hook hello_slice_with_step
function hookHelloSliceWithStep() {
    var symbol = Module.findExportByName(null, "hello_slice_with_step");
    if (!symbol) return;
    
    log(`Hooking hello_slice_with_step at ${symbol}`);
    
    Interceptor.attach(symbol, {
        onEnter: function(args) {
            stats.calls++;
            stats.slices++;
            
            this.originalArrPtr = args[0];
            this.start = args[1].toInt32();
            this.end = args[2].toInt32();
            this.step = args[3].toInt32();
            
            var arr = readIntArray(this.originalArrPtr);
            if (arr) {
                this.originalData = arr.readData();
                
                if (config.logArguments) {
                    log(`hello_slice_with_step called:`);
                    log(`  Array: [${this.originalData.join(', ')}]`);
                    log(`  Slice: [${this.start}:${this.end}:${this.step}]`);
                }
                
                // 计算切片
                var sliceInfo = calculateSliceIndices(
                    arr.size, 
                    this.start, 
                    this.end,
                    this.step
                );
                
                // 应用高级切片
                var slicedData = advancedSlice(
                    this.originalData, 
                    sliceInfo, 
                    config.sliceBehavior.sliceMode
                );
                
                this.shouldIntercept = true;
                this.slicedData = slicedData;
                
                log(`  Result data: [${slicedData.join(', ')}]`);
            }
        },
        onLeave: function(retval) {
            if (this.shouldIntercept && this.slicedData) {
                var fakeSlicePtr = createFakeIntArray(this.slicedData);
                retval.replace(fakeSlicePtr);
                stats.modified++;
            }
        }
    });
}

// Hook hello_get_int (支持高级索引)
function hookHelloGetInt() {
    var symbol = Module.findExportByName(null, "hello_get_int");
    if (!symbol) return;
    
    log(`Hooking hello_get_int at ${symbol}`);
    
    Interceptor.attach(symbol, {
        onEnter: function(args) {
            stats.calls++;
            stats.gets++;
            
            this.arrPtr = args[0];
            this.index = args[1].toInt32();
            
            var arr = readIntArray(this.arrPtr);
            if (arr) {
                this.originalData = arr.readData();
                
                // 处理负数索引
                var actualIndex = this.index;
                if (config.sliceBehavior.supportNegativeIndex && actualIndex < 0) {
                    actualIndex = arr.size + actualIndex;
                }
                
                // 边界检查
                if (actualIndex < 0 || actualIndex >= arr.size) {
                    log(`Index ${this.index} (actual: ${actualIndex}) out of bounds`);
                    this.outOfBounds = true;
                } else {
                    this.originalValue = this.originalData[actualIndex];
                    log(`hello_get_int: arr[${this.index}] = ${this.originalValue}`);
                }
            }
        },
        onLeave: function(retval) {
            if (this.outOfBounds) {
                // 返回默认值而不是崩溃
                retval.replace(999); // 特殊错误值
                stats.modified++;
            } else if (this.originalValue !== undefined) {
                // 可以在这里修改返回值
                if (config.sliceBehavior.sliceMode === "random") {
                    var modifiedValue = this.originalValue * (Math.random() * 2 + 1);
                    retval.replace(Math.floor(modifiedValue));
                    stats.modified++;
                    log(`  Modified to: ${Math.floor(modifiedValue)}`);
                }
            }
        }
    });
}

// Hook hello_slice_string
function hookHelloSliceString() {
    var symbol = Module.findExportByName(null, "hello_slice_string");
    if (!symbol) return;
    
    log(`Hooking hello_slice_string at ${symbol}`);
    
    Interceptor.attach(symbol, {
        onEnter: function(args) {
            stats.calls++;
            stats.slices++;
            
            this.arrPtr = args[0];
            this.start = args[1].toInt32();
            this.end = args[2].toInt32();
            
            var arr = readStringArray(this.arrPtr);
            if (arr) {
                this.originalStrings = arr.readStrings();
                
                if (config.logArguments) {
                    log(`hello_slice_string called:`);
                    log(`  Array: [${this.originalStrings.map(s => `"${s}"`).join(', ')}]`);
                    log(`  Slice: [${this.start}:${this.end}]`);
                }
                
                // 计算切片
                var sliceInfo = calculateSliceIndices(
                    arr.size, 
                    this.start, 
                    this.end
                );
                
                // 收集切片结果
                this.slicedStrings = [];
                for (var i = 0; i < sliceInfo.indices.length; i++) {
                    var idx = sliceInfo.indices[i];
                    if (idx >= 0 && idx < arr.size) {
                        var str = this.originalStrings[idx];
                        
                        // 应用字符串变换
                        switch (config.sliceBehavior.sliceMode) {
                            case "reverse":
                                str = str.split('').reverse().join('');
                                break;
                            case "uppercase":
                                str = str.toUpperCase();
                                break;
                            case "duplicate":
                                str = str + "_" + str;
                                break;
                        }
                        
                        this.slicedStrings.push(str);
                    }
                }
                
                this.shouldIntercept = true;
                log(`  Result strings: [${this.slicedStrings.map(s => `"${s}"`).join(', ')}]`);
            }
        },
        onLeave: function(retval) {
            if (this.shouldIntercept && this.slicedStrings) {
                var fakeSlicePtr = createFakeStringArray(this.slicedStrings);
                retval.replace(fakeSlicePtr);
                stats.modified++;
            }
        }
    });
}

// 动态配置接口
rpc.exports = {
    getConfig: function() {
        return config;
    },
    
    setConfig: function(newConfig) {
        for (var key in newConfig) {
            if (key in config) {
                if (typeof config[key] === 'object' && config[key] !== null) {
                    Object.assign(config[key], newConfig[key]);
                } else {
                    config[key] = newConfig[key];
                }
            }
        }
        return "Config updated";
    },
    
    getStats: function() {
        return stats;
    },
    
    setSliceMode: function(mode) {
        config.sliceBehavior.sliceMode = mode;
        return `Slice mode set to: ${mode}`;
    },
    
    testSlice: function(arrData, start, end, step) {
        // 直接在 JavaScript 中测试切片算法
        var sliceInfo = calculateSliceIndices(arrData.length, start, end, step);
        var result = advancedSlice(arrData, sliceInfo, config.sliceBehavior.sliceMode);
        
        return {
            indices: sliceInfo.indices,
            result: result,
            info: sliceInfo
        };
    },
    
    // 直接操作内存创建数组
    createTestArray: function(data) {
        return createFakeIntArray(data).toString();
    },
    
    // 读取内存中的数组
    readArrayAt: function(address) {
        var ptr = new NativePointer(address);
        var arr = readIntArray(ptr);
        if (arr) {
            return {
                data: arr.readData(),
                size: arr.size,
                capacity: arr.capacity
            };
        }
        return null;
    }
};

// 安装所有钩子
function installHooks() {
    console.log("\n" + "=".repeat(60));
    console.log("CUSTOM ARRAY SLICE INTERCEPTOR");
    console.log("=".repeat(60));
    console.log(`PID: ${Process.id}`);
    console.log(`Debug: ${config.debug}`);
    console.log(`Slice Mode: ${config.sliceBehavior.sliceMode}`);
    console.log("=".repeat(60) + "\n");
    
    hookHelloSliceInt();
    hookHelloSliceWithStep();
    hookHelloGetInt();
    hookHelloSliceString();
    
    log("All hooks installed successfully");
    
    // 显示控制提示
    console.log("\n[CONTROL] Use rpc.exports to control the interceptor:");
    console.log("  - setSliceMode('normal|reverse|random|double|fibonacci')");
    console.log("  - setConfig({sliceBehavior: {autoTruncate: false}})");
    console.log("  - getStats() to see interception statistics");
    
    // 定期显示状态
    setInterval(function() {
        if (stats.calls > 0) {
            log(`[STATS] Calls: ${stats.calls}, Modified: ${stats.modified}`);
        }
    }, 5000);
}

// 延迟启动，确保目标函数已加载
setTimeout(installHooks, 100);
```

### 3. Python 控制脚本

```python
#!/usr/bin/env python3
# control_slice_intercept.py

import frida
import sys
import json
import time
from cmd import Cmd

class SliceInterceptorControl(Cmd):
    intro = "\nCustom Slice Interceptor Control Console\nType 'help' or '?' for commands\n"
    prompt = "slice> "
    
    def __init__(self, session):
        super().__init__()
        self.session = session
        self.script = None
        self.rpc = None
        
    def preloop(self):
        # 加载 Frida 脚本
        with open("intercept_custom_slice.js", "r") as f:
            js_code = f.read()
        
        self.script = self.session.create_script(js_code)
        
        def on_message(message, data):
            if message['type'] == 'send':
                print(f"[*] {message['payload']}")
            elif message['type'] == 'error':
                print(f"[-] Error: {message['description']}")
        
        self.script.on('message', on_message)
        self.script.load()
        time.sleep(1)
        
        # 获取 RPC 接口
        self.rpc = self.script.exports
        
        # 显示初始状态
        config = self.rpc.get_config()
        print(f"\n[*] Connected to PID: {self.session.pid}")
        print(f"[*] Slice mode: {config['sliceBehavior']['sliceMode']}")
        print(f"[*] Debug: {config['debug']}")
    
    def do_config(self, arg):
        """显示当前配置"""
        config = self.rpc.get_config()
        print("\n当前配置:")
        print(json.dumps(config, indent=2))
    
    def do_stats(self, arg):
        """显示统计信息"""
        stats = self.rpc.get_stats()
        print("\n拦截统计:")
        for key, value in stats.items():
            print(f"  {key}: {value}")
    
    def do_mode(self, arg):
        """设置切片模式: normal, reverse, random, double, fibonacci"""
        if not arg:
            print("可用模式: normal, reverse, random, double, fibonacci")
            return
        
        result = self.rpc.set_slice_mode(arg)
        print(f"[*] {result}")
    
    def do_test(self, arg):
        """测试切片算法"""
        try:
            if not arg:
                print("用法: test <数组数据> <start> <end> [step]")
                print("示例: test 1,2,3,4,5 1 4")
                return
            
            parts = arg.split()
            if len(parts) < 3:
                print("参数不足")
                return
            
            # 解析数组数据
            arr_data = [int(x) for x in parts[0].split(',')]
            start = int(parts[1])
            end = int(parts[2])
            step = int(parts[3]) if len(parts) > 3 else 1
            
            result = self.rpc.test_slice(arr_data, start, end, step)
            
            print(f"\n测试切片:")
            print(f"  数组: {arr_data}")
            print(f"  切片: [{start}:{end}:{step}]")
            print(f"  索引: {result['indices']}")
            print(f"  结果: {result['result']}")
            
        except Exception as e:
            print(f"错误: {e}")
    
    def do_read(self, arg):
        """读取内存中的数组 (需要地址)"""
        if not arg:
            print("用法: read <内存地址>")
            print("示例: read 0x7fff12345678")
            return
        
        try:
            addr = int(arg, 16) if arg.startswith('0x') else int(arg)
            result = self.rpc.read_array_at(addr)
            
            if result:
                print(f"\n数组信息:")
                print(f"  大小: {result['size']}")
                print(f"  容量: {result['capacity']}")
                print(f"  数据: {result['data']}")
            else:
                print("无法读取数组")
                
        except Exception as e:
            print(f"错误: {e}")
    
    def do_create(self, arg):
        """创建测试数组"""
        if not arg:
            print("用法: create <数据>")
            print("示例: create 10,20,30,40")
            return
        
        try:
            arr_data = [int(x) for x in arg.split(',')]
            addr = self.rpc.create_test_array(arr_data)
            print(f"[*] 数组创建在地址: {addr}")
            print(f"    数据: {arr_data}")
            
        except Exception as e:
            print(f"错误: {e}")
    
    def do_advanced(self, arg):
        """高级配置"""
        print("\n高级配置选项:")
        print("  1. 启用/禁用负数索引支持")
        print("  2. 启用/禁用自动截断")
        print("  3. 设置默认步长")
        print("  4. 启用/禁用调试日志")
        
        choice = input("选择 (1-4): ").strip()
        
        try:
            if choice == '1':
                current = self.rpc.get_config()['sliceBehavior']['supportNegativeIndex']
                new_value = not current
                self.rpc.set_config({
                    'sliceBehavior': {'supportNegativeIndex': new_value}
                })
                print(f"[*] 负数索引支持: {'启用' if new_value else '禁用'}")
                
            elif choice == '2':
                current = self.rpc.get_config()['sliceBehavior']['autoTruncate']
                new_value = not current
                self.rpc.set_config({
                    'sliceBehavior': {'autoTruncate': new_value}
                })
                print(f"[*] 自动截断: {'启用' if new_value else '禁用'}")
                
            elif choice == '3':
                step = input("输入默认步长: ").strip()
                step = int(step)
                self.rpc.set_config({
                    'sliceBehavior': {'defaultStep': step}
                })
                print(f"[*] 默认步长设置为: {step}")
                
            elif choice == '4':
                current = self.rpc.get_config()['debug']
                new_value = not current
                self.rpc.set_config({
                    'debug': new_value
                })
                print(f"[*] 调试日志: {'启用' if new_value else '禁用'}")
                
        except Exception as e:
            print(f"错误: {e}")
    
    def do_status(self, arg):
        """显示完整状态"""
        config = self.rpc.get_config()
        stats = self.rpc.get_stats()
        
        print("\n" + "="*60)
        print("切片拦截器状态")
        print("="*60)
        print(f"PID: {self.session.pid}")
        print(f"切片模式: {config['sliceBehavior']['sliceMode']}")
        print(f"负数索引: {config['sliceBehavior']['supportNegativeIndex']}")
        print(f"自动截断: {config['sliceBehavior']['autoTruncate']}")
        print("-"*60)
        print(f"总调用: {stats['calls']}")
        print(f"切片操作: {stats['slices']}")
        print(f"获取操作: {stats['gets']}")
        print(f"修改次数: {stats['modified']}")
        print("="*60)
    
    def do_exit(self, arg):
        """退出控制台"""
        print("[*] 退出...")
        return True
    
    def do_quit(self, arg):
        """退出控制台"""
        return self.do_exit(arg)

def main():
    if len(sys.argv) != 2:
        print("用法: python3 control_slice_intercept.py <pid>")
        print("示例: python3 control_slice_intercept.py 12345")
        sys.exit(1)
    
    pid = int(sys.argv[1])
    
    try:
        # 连接到进程
        session = frida.attach(pid)
        print(f"[*] 连接到 PID {pid}")
        
        # 启动控制台
        control = SliceInterceptorControl(session)
        control.cmdloop()
        
    except Exception as e:
        print(f"[-] 错误: {e}")
    finally:
        if 'session' in locals():
            session.detach()

if __name__ == "__main__":
    main()
```

### 4. 快速使用脚本

```javascript
// quick_slice_intercept.js
// 快速拦截脚本 - 简单版本

Interceptor.attach(Module.findExportByName(null, "hello_slice_int"), {
    onEnter: function(args) {
        console.log("\n[+] hello_slice_int intercepted!");
        console.log(`    Array ptr: ${args[0]}`);
        console.log(`    Start: ${args[1].toInt32()}`);
        console.log(`    End: ${args[2].toInt32()}`);
        
        // 修改切片参数
        var newStart = args[1].toInt32() + 1;
        var newEnd = args[2].toInt32() - 1;
        
        console.log(`    Modified to: [${newStart}:${newEnd}]`);
        
        args[1] = ptr(newStart);
        args[2] = ptr(newEnd);
    }
});

// 拦截获取元素
Interceptor.attach(Module.findExportByName(null, "hello_get_int"), {
    onEnter: function(args) {
        var idx = args[1].toInt32();
        console.log(`[+] hello_get_int: index ${idx}`);
        
        // 总是返回索引值的平方
        this.modifiedValue = idx * idx;
    },
    onLeave: function(retval) {
        retval.replace(this.modifiedValue);
        console.log(`    Returned: ${this.modifiedValue} (modified)`);
    }
});

console.log("[READY] Slice interceptor active!");
```

### 5. 编译和运行

```bash
# 1. 编译 C 程序
gcc custom_array.c -o custom_array -no-pie

# 2. 运行程序
./custom_array &
# 记下 PID，例如 12345

# 或使用测试模式
./custom_array --test &
./custom_array --demo &

# 3. 使用 Frida 拦截
# 方法1: 使用完整脚本
frida -p 12345 -l intercept_custom_slice.js

# 方法2: 使用快速脚本
frida -p 12345 -l quick_slice_intercept.js

# 方法3: 使用 Python 控制台
python3 control_slice_intercept.py 12345
```

### 6. 演示不同切片模式

在 Frida 控制台中尝试：

```javascript
// 1. 正常模式
rpc.exports.setSliceMode("normal");
// 输出: [0, 1, 2, 3] -> [1, 2] (正常切片)

// 2. 反转模式
rpc.exports.setSliceMode("reverse");
// 输出: [0, 1, 2, 3] -> [2, 1] (反转切片)

// 3. 随机模式
rpc.exports.setSliceMode("random");
// 输出: [0, 1, 2, 3] -> [随机值, 随机值]

// 4. 双倍模式
rpc.exports.setSliceMode("double");
// 输出: [0, 1, 2, 3] -> [0, 0, 1, 2, 2, 4]

// 5. 斐波那契模式
rpc.exports.setSliceMode("fibonacci");
// 输出: [0, 1, 2, 3] -> [0, 1, 1, 2]
```

### 7. 高级特性

这个实现展示了：

1. **结构体解析**：读取 C 语言的自定义结构体
2. **Python风格切片**：实现类似 Python 的切片语法
3. **负数索引支持**：支持 `arr[-1]` 这样的索引
4. **多种切片模式**：正常、反转、随机、双倍等
5. **内存操作**：创建假的数组结构体
6. **错误处理**：安全地处理边界情况
7. **动态配置**：运行时修改切片行为

### 8. 实际应用场景

这种技术可以用于：
- **测试边界情况**：测试代码对异常输入的响应
- **API 模拟**：模拟外部库的行为
- **安全测试**：测试程序对恶意输入的防御
- **性能优化**：替换低效算法为高效算法
- **调试辅助**：动态修改算法行为进行调试

通过 Frida 拦截自定义的 C 函数，你可以：
- 修改函数的输入参数
- 改变函数的返回值
- 实现完全不同的算法
- 收集函数调用的统计信息
- 动态启用/禁用功能

这种动态代码修改能力是 Frida 最强大的特性之一！

## 原理

# Frida 原理详解

## 1. 整体架构

Frida 是一个动态二进制插桩框架，其核心原理可以概括为：

```
应用进程 ←[注入]→ Frida Agent (JavaScript 运行时) ←[通信]→ Frida 控制端
```

### 1.1 核心组件

```python
# Frida 架构简图
# ===========================================
# 控制端 (Client)         # 被控端 (Target)
# ===========================================
# frida-python            frida-server (Android)
# frida-tools             frida-gadget (.so)
# frida-cli               frida-agent (.js)
# 自定义脚本               injected code
# ===========================================
```

## 2. 注入原理

### 2.1 注入方式对比

| 方式 | 原理 | 平台 | 特点 |
|------|------|------|------|
| **ptrace** | Linux 调试接口 | Linux/Android | 最常用，功能强大 |
| **posix_spawn** | 进程创建时注入 | iOS/macOS | iOS 主要方式 |
| **LD_PRELOAD** | 动态库预加载 | Linux | 简单但有限制 |
| **Xposed** | 修改 ART/Dalvik | Android | Android Java 层 |
| **Cydia Substrate** | MSHookFunction | iOS | iOS 早期方案 |

### 2.2 ptrace 注入流程（Android/Linux）

```c
// 简化版的 ptrace 注入原理
int inject_process(pid_t pid, const char* library_path) {
    // 1. 附加到目标进程
    ptrace(PTRACE_ATTACH, pid, NULL, NULL);
    waitpid(pid, NULL, WUNTRACED);
    
    // 2. 获取寄存器状态
    struct user_regs_struct regs;
    ptrace(PTRACE_GETREGS, pid, NULL, &regs);
    
    // 3. 备份原始指令
    long backup = ptrace(PTRACE_PEEKTEXT, pid, regs.rip, NULL);
    
    // 4. 写入 shellcode (调用 dlopen 加载库)
    unsigned char shellcode[] = {
        // mov rdi, library_path_address
        // mov rax, dlopen_address
        // call rax
        // 恢复原始指令
        // 跳回原位置
    };
    
    // 5. 执行 shellcode
    for (int i = 0; i < sizeof(shellcode); i += sizeof(long)) {
        ptrace(PTRACE_POKETEXT, pid, regs.rip + i, 
               *(long*)(shellcode + i));
    }
    
    // 6. 单步执行并恢复
    ptrace(PTRACE_SINGLESTEP, pid, NULL, NULL);
    waitpid(pid, NULL, WUNTRACED);
    
    // 7. 恢复原始代码
    ptrace(PTRACE_POKETEXT, pid, regs.rip, backup);
    
    // 8. 恢复寄存器并分离
    ptrace(PTRACE_SETREGS, pid, NULL, &regs);
    ptrace(PTRACE_DETACH, pid, NULL, NULL);
    
    return 0;
}
```

### 2.3 Frida 的实际注入流程

```python
# frida-core/injector.c 简化逻辑
def frida_inject(pid, agent_code):
    # 1. 创建 Agent 脚本文件
    agent_so = compile_agent_to_so(agent_code)
    
    # 2. 在目标进程分配内存
    remote_mem = allocate_remote_memory(pid, agent_so.size)
    
    # 3. 写入 Agent 代码
    write_remote_memory(pid, remote_mem, agent_so.code)
    
    # 4. 查找/计算函数地址
    dlopen_addr = find_dlopen_address(pid)
    dlsym_addr = find_dlsym_address(pid)
    
    # 5. 创建远程线程执行
    create_remote_thread(pid, dlopen_addr, [remote_mem, RTLD_LAZY])
    
    # 6. 等待 Agent 初始化
    wait_for_agent_ready(pid)
    
    return remote_mem
```

## 3. Agent 工作原理

### 3.1 Agent 架构

```c
// frida-gum 核心架构
typedef struct {
    GumInterceptor* interceptor;    // 函数拦截器
    GumStalker* stalker;            // 指令追踪器
    GumScriptBackend* backend;      // JS 运行时后端
    GumMemoryAccessMonitor* monitor; // 内存访问监视器
    GHashTable* hooks;              // 已安装的钩子
} FridaAgent;
```

### 3.2 JavaScript 运行时集成

```javascript
// Frida 的 JavaScript 运行时引擎选择
const engines = {
    // V8: 默认引擎，高性能
    v8: {
        type: "v8",
        features: ["JIT", "ES6+", "async/await"],
        memory: "独立堆"
    },
    
    // Duktape: 轻量级替代
    duktape: {
        type: "interpreter",
        features: ["ES5", "小巧"],
        memory: "共享堆"
    }
};

// Frida Agent 初始化流程
function initializeAgent() {
    // 1. 初始化 Gum (动态插桩框架)
    gum_init();
    
    // 2. 创建 JS 运行时
    const runtime = new JavaScriptRuntime({
        memory_domain: "default",
        scheduler: new MicrotaskQueue()
    });
    
    // 3. 暴露 Native API
    runtime.expose({
        'Process': ProcessAPI,
        'Module': ModuleAPI,
        'Memory': MemoryAPI,
        'Interceptor': InterceptorAPI,
        'Stalker': StalkerAPI
    });
    
    // 4. 加载用户脚本
    const script = runtime.compile(userJavaScript);
    runtime.execute(script);
    
    // 5. 建立通信通道
    setupIpcChannel(runtime);
}
```

## 4. 函数拦截原理

### 4.1 函数拦截技术对比

| 技术 | 原理 | 优点 | 缺点 |
|------|------|------|------|
| **inline hook** | 修改函数开头指令 | 高效、通用 | 复杂、可能崩溃 |
| **导入表 hook** | 修改 IAT/GOT | 稳定 | 只针对动态链接 |
| **trampoline** | 跳板函数 | 可恢复 | 需要额外内存 |
| **breakpoint** | 软中断 | 简单 | 性能差 |

### 4.2 Frida 的 Interceptor 实现

```c
// gum/interceptor-x86.c 简化版
typedef struct _GumInvocationListener {
    void (*on_enter)(GumInvocationContext* context);
    void (*on_leave)(GumInvocationContext* context);
} GumInvocationListener;

// 拦截器核心逻辑
gboolean gum_interceptor_attach(void* function_address,
                               GumInvocationListener* listener,
                               void* user_data) {
    // 1. 备份原始指令
    uint8_t* target = (uint8_t*)function_address;
    uint8_t original[PROLOGUE_SIZE];
    memcpy(original, target, PROLOGUE_SIZE);
    
    // 2. 生成跳转指令 (x86-64 示例)
    // jmp relative_address
    uint8_t jump[] = { 0xE9, 0x00, 0x00, 0x00, 0x00 };
    int32_t offset = (int32_t)(trampoline - target - 5);
    memcpy(&jump[1], &offset, 4);
    
    // 3. 写入跳转指令
    mprotect_region(target, PROLOGUE_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC);
    memcpy(target, jump, sizeof(jump));
    mprotect_region(target, PROLOGUE_SIZE, PROT_READ | PROT_EXEC);
    
    // 4. 创建蹦床函数
    uint8_t* trampoline = allocate_executable_memory();
    // 复制原始指令
    memcpy(trampoline, original, PROLOGUE_SIZE);
    // 添加跳回指令
    uint8_t return_jump[] = { 0xE9, 0x00, 0x00, 0x00, 0x00 };
    offset = (int32_t)(target + PROLOGUE_SIZE - trampoline - PROLOGUE_SIZE - 5);
    memcpy(&return_jump[1], &offset, 4);
    memcpy(trampoline + PROLOGUE_SIZE, return_jump, sizeof(return_jump));
    
    // 5. 设置回调
    gum_invocation_context_init(&context, listener, user_data);
    
    return TRUE;
}
```

### 4.3 JavaScript 拦截器接口

```javascript
// frida-gumjs/bindings/interceptor.js
class Interceptor {
    static attach(target, callbacks) {
        // 转换为 NativePointer
        const targetPtr = ptr(target);
        
        // 创建 Native 回调
        const nativeCallbacks = new NativeCallback({
            onEnter: function(context) {
                // 调用 JavaScript 回调
                const jsContext = new InvocationContext(context);
                if (callbacks.onEnter) {
                    callbacks.onEnter(jsContext);
                }
            },
            onLeave: function(context) {
                // 调用 JavaScript 回调
                const jsContext = new InvocationContext(context);
                if (callbacks.onLeave) {
                    callbacks.onLeave(jsContext);
                }
            }
        }, 'void', ['pointer']);
        
        // 调用 Native 层
        _gum_interceptor_attach(targetPtr, nativeCallbacks, null);
    }
    
    static detach(listener) {
        _gum_interceptor_detach(listener);
    }
}
```

## 5. 内存操作原理

### 5.1 内存访问抽象层

```c
// gum/memory.c - 内存操作抽象
typedef struct {
    uint8_t* (*read)(GumAddress address, gsize len);
    gboolean (*write)(GumAddress address, const uint8_t* bytes, gsize len);
    gboolean (*protect)(GumAddress address, gsize len, GumPageProtection prot);
    GArray* (*enumerate_ranges)(GumPageProtection prot);
} GumMemoryApi;

// 不同平台的实现
#ifdef __linux__
    #include "memory-linux.c"
#elif defined(__APPLE__)
    #include "memory-darwin.c"
#elif defined(_WIN32)
    #include "memory-windows.c"
#endif
```

### 5.2 内存扫描算法

```javascript
// Process.enumerateRanges 实现原理
function enumerateRanges(protection) {
    // 1. 读取 /proc/<pid>/maps (Linux)
    // 或调用 VirtualQueryEx (Windows)
    // 或调用 vm_region (macOS)
    
    const ranges = [];
    
    // 2. 解析内存映射
    // 示例: 7f1234560000-7f1234567890 r-xp 00000000 08:01 123456 /lib/libc.so.6
    const maps = readProcessMaps(pid);
    
    for (const line of maps) {
        const [range, perms, offset, device, inode, pathname] = line.split(/\s+/);
        const [startHex, endHex] = range.split('-');
        const start = ptr(startHex);
        const end = ptr(endHex);
        
        // 3. 检查保护标志
        if (protection === undefined || 
            matchProtection(perms, protection)) {
            ranges.push({
                base: start,
                size: end.sub(start),
                protection: permsToProtection(perms)
            });
        }
    }
    
    return ranges;
}
```

### 5.3 内存扫描优化

```c
// 内存扫描的 SIMD 优化 (x86 SSE/AVX)
void memory_scan_simd(const uint8_t* data, size_t size, 
                      const uint8_t* pattern, size_t pattern_len,
                      void (*on_match)(void* address, void* user_data),
                      void* user_data) {
    // 使用 SIMD 指令加速搜索
#ifdef __SSE2__
    __m128i pattern_vec = _mm_loadu_si128((__m128i*)pattern);
    
    for (size_t i = 0; i <= size - 16; i += 16) {
        __m128i data_vec = _mm_loadu_si128((__m128i*)(data + i));
        __m128i cmp = _mm_cmpeq_epi8(data_vec, pattern_vec);
        int mask = _mm_movemask_epi8(cmp);
        
        if (mask == 0xFFFF) {  // 完全匹配
            on_match((void*)(data + i), user_data);
        }
    }
#endif
}
```

## 6. 指令追踪原理

### 6.1 Stalker 工作原理

```c
// gum/stalker.c - 指令追踪器
typedef struct {
    GumCpuContext saved_context;      // 保存的 CPU 上下文
    uint8_t* code_cache;              // 代码缓存
    GumEventSink* event_sink;         // 事件回调
    GHashTable* block_cache;          // 已翻译的块缓存
} GumStalker;

// 指令翻译流程
void stalker_follow(GumStalker* self, GumThreadId thread_id) {
    // 1. 保存原始上下文
    gum_thread_suspend(thread_id);
    gum_cpu_context_copy(&self->saved_context, get_thread_context(thread_id));
    
    // 2. 开始翻译循环
    while (stalker_active) {
        // 获取当前指令地址
        GumAddress pc = get_program_counter();
        
        // 3. 检查是否已翻译
        GumStalkerBlock* block = g_hash_table_lookup(self->block_cache, pc);
        if (block == NULL) {
            // 4. 翻译指令块
            block = translate_block(pc);
            g_hash_table_insert(self->block_cache, pc, block);
        }
        
        // 5. 执行翻译后的代码
        execute_translated_block(block);
        
        // 6. 触发事件回调
        if (self->event_sink != NULL) {
            self->event_sink->on_call(pc, ...);
        }
    }
}
```

### 6.2 指令重写示例

```assembly
; 原始指令
original:
    push rbp
    mov rbp, rsp
    sub rsp, 0x20
    mov [rbp-0x10], rdi
    call some_function
    add rsp, 0x20
    pop rbp
    ret

; Stalker 翻译后的指令
translated:
    push rbp
    mov rbp, rsp
    ; 插入事件触发代码
    push rax
    push rdi
    mov rdi, [event_callback]
    call rdi
    pop rdi
    pop rax
    ; 继续原始指令
    sub rsp, 0x20
    mov [rbp-0x10], rdi
    call some_function
    ; 插入返回事件
    push rax
    mov rdi, [return_callback]
    call rdi
    pop rax
    add rsp, 0x20
    pop rbp
    ret
```

## 7. 通信机制

### 7.1 进程间通信架构

```
┌─────────────┐    USB/网络    ┌─────────────┐
│  控制端脚本   │◄─────────────►│  frida-server │
│ (Python/JS) │               │   (设备端)   │
└─────────────┘               └─────────────┘
                                      │
                                      ▼
                               ┌─────────────┐
                               │ Agent (.so) │
                               │ 在目标进程   │
                               └─────────────┘
```

### 7.2 通信协议

```c
// frida-core/ipc-protocol.h
typedef struct {
    uint32_t length;      // 消息长度
    uint32_t type;        // 消息类型
    uint8_t payload[];    // 消息体
} FridaIpcMessage;

// 消息类型枚举
enum {
    FRIDA_IPC_HELLO = 1,
    FRIDA_IPC_SCRIPT_CREATE,
    FRIDA_IPC_SCRIPT_DESTROY,
    FRIDA_IPC_SCRIPT_LOAD,
    FRIDA_IPC_SCRIPT_UNLOAD,
    FRIDA_IPC_SCRIPT_POST_MESSAGE,
    FRIDA_IPC_SCRIPT_LOG,
    FRIDA_IPC_SCRIPT_ERROR
};
```

### 7.3 数据传输优化

```javascript
// 使用共享内存传输大数据
class SharedMemoryTransport {
    constructor(size) {
        // 1. 创建共享内存区域
        this.shm_fd = shm_open("/frida_shm_" + uuid(), O_RDWR | O_CREAT, 0666);
        ftruncate(this.shm_fd, size);
        
        // 2. 内存映射
        this.buffer = mmap(NULL, size, PROT_READ | PROT_WRITE, 
                          MAP_SHARED, this.shm_fd, 0);
        
        // 3. 在进程间传递文件描述符
        send_fd_over_socket(this.shm_fd);
    }
    
    write(data) {
        // 写入共享内存
        memcpy(this.buffer, data, data.length);
        
        // 只发送元数据
        send_metadata({
            offset: 0,
            length: data.length,
            shm_id: this.shm_id
        });
    }
}
```

## 8. 安全性考虑

### 8.1 反检测技术

```c
// Frida 的反检测机制
void anti_detection_init() {
    // 1. 隐藏线程
    pthread_setname_np("android.bg");  // 伪装成系统线程
    
    // 2. 隐藏内存映射
    hide_memory_maps("/proc/self/maps");
    
    // 3. 隐藏文件描述符
    hide_file_descriptor(agent_fd);
    
    // 4. 随机化 Agent 特征
    randomize_agent_signature();
    
    // 5. Hook 检测函数
    hook_detection_functions();
}

// 隐藏内存映射
void hide_memory_maps(const char* maps_path) {
    // Hook read() 系统调用
    interceptor_attach("read", {
        onEnter: function(args) {
            const fd = args[0].toInt32();
            const buf = args[1];
            const count = args[2].toInt32();
            
            // 检查是否在读取 /proc/self/maps
            if (is_maps_file(fd)) {
                this.is_reading_maps = true;
                this.original_buf = buf;
                
                // 重定向到临时缓冲区
                this.temp_buf = Memory.alloc(count);
                args[1] = this.temp_buf;
            }
        },
        onLeave: function(retval) {
            if (this.is_reading_maps) {
                // 过滤掉包含 frida 的行
                const data = Memory.readByteArray(this.temp_buf, retval.toInt32());
                const filtered = filter_frida_entries(data);
                
                // 写回原始缓冲区
                Memory.writeByteArray(this.original_buf, filtered);
            }
        }
    });
}
```

### 8.2 稳定性保证

```c
// 异常处理机制
void safe_hook_implementation(void* target, void* replacement) {
    // 1. 验证目标地址
    if (!is_executable_address(target)) {
        return ERROR_INVALID_ADDRESS;
    }
    
    // 2. 创建安全区 (安全页)
    uint8_t* safe_page = mmap(NULL, PAGE_SIZE,
                             PROT_READ | PROT_WRITE | PROT_EXEC,
                             MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    
    // 3. 备份原始指令
    memcpy(safe_page, target, HOOK_SIZE);
    
    // 4. 原子性修改
    atomic_hook_install(target, replacement, safe_page);
    
    // 5. 设置信号处理器处理崩溃
    struct sigaction sa;
    sa.sa_sigaction = crash_handler;
    sa.sa_flags = SA_SIGINFO;
    sigaction(SIGSEGV, &sa, NULL);
    
    // 6. 完整性验证
    if (!verify_hook_integrity(target, replacement)) {
        // 自动恢复
        atomic_hook_remove(target, safe_page);
        return ERROR_INTEGRITY_CHECK_FAILED;
    }
}
```

## 9. 性能优化

### 9.1 代码缓存机制

```c
// 翻译块缓存
typedef struct _GumStalkerBlockCache {
    GHashTable* hash_table;          // 地址 -> 翻译块
    GQueue* lru_queue;               // LRU 队列
    size_t max_size;                 // 最大缓存大小
    size_t hit_count;                // 命中统计
    size_t miss_count;               // 未命中统计
} GumStalkerBlockCache;

// 缓存查找优化
GumStalkerBlock* block_cache_lookup(GumStalkerBlockCache* cache, 
                                   GumAddress address) {
    // 1. 一级缓存：哈希表
    GumStalkerBlock* block = g_hash_table_lookup(cache->hash_table, 
                                                 GUINT_TO_POINTER(address));
    if (block != NULL) {
        // 更新 LRU
        g_queue_remove(cache->lru_queue, GUINT_TO_POINTER(address));
        g_queue_push_head(cache->lru_queue, GUINT_TO_POINTER(address));
        cache->hit_count++;
        return block;
    }
    
    cache->miss_count++;
    
    // 2. 缓存未命中，执行翻译
    block = translate_address(address);
    
    // 3. 插入缓存
    g_hash_table_insert(cache->hash_table, GUINT_TO_POINTER(address), block);
    g_queue_push_head(cache->lru_queue, GUINT_TO_POINTER(address));
    
    // 4. 检查缓存大小，必要时淘汰
    if (g_queue_get_length(cache->lru_queue) > cache->max_size) {
        gpointer old_key = g_queue_pop_tail(cache->lru_queue);
        GumStalkerBlock* old_block = g_hash_table_lookup(cache->hash_table, 
                                                        old_key);
        free_translated_block(old_block);
        g_hash_table_remove(cache->hash_table, old_key);
    }
    
    return block;
}
```

### 9.2 JIT 优化

```javascript
// JavaScript 到 Native 的 JIT 编译
class FridaJITCompiler {
    compile(jsFunction) {
        // 1. 分析 JavaScript 函数
        const ast = parseJavaScript(jsFunction.toString());
        
        // 2. 转换为中间表示 (IR)
        const ir = generateIR(ast);
        
        // 3. 优化 IR
        optimizeIR(ir);
        
        // 4. 生成机器码
        const machineCode = generateMachineCode(ir, {
            architecture: Process.arch,
            abi: Process.abi
        });
        
        // 5. 分配可执行内存
        const executableMemory = Memory.alloc(machineCode.length, {
            protection: 'rwx'
        });
        
        // 6. 写入机器码
        Memory.writeByteArray(executableMemory, machineCode);
        
        return executableMemory;
    }
}
```

## 10. 跨平台实现

### 10.1 平台抽象层

```c
// gum/platform.h - 平台抽象接口
typedef struct {
    // 进程操作
    GumProcessId (*get_current_process_id)(void);
    GumThreadId (*get_current_thread_id)(void);
    
    // 内存操作
    gboolean (*memory_query)(GumAddress address, GumPageProtection* prot);
    gpointer (*memory_alloc)(gsize size, GumPageProtection prot);
    gboolean (*memory_free)(gpointer mem, gsize size);
    
    // 线程操作
    GumThreadId (*thread_create)(GumThreadFunc func, gpointer data);
    void (*thread_suspend)(GumThreadId thread_id);
    void (*thread_resume)(GumThreadId thread_id);
    
    // 代码生成
    gpointer (*code_alloc)(gsize size);
    void (*code_free)(gpointer code, gsize size);
} GumPlatformApi;

// 各平台实现
#ifdef HAVE_LINUX
    #include "platform-linux.c"
#elif defined(HAVE_DARWIN)
    #include "platform-darwin.c"
#elif defined(HAVE_WINDOWS)
    #include "platform-windows.c"
#elif defined(HAVE_QNX)
    #include "platform-qnx.c"
#endif
```

### 10.2 架构支持

```c
// CPU 架构抽象
typedef struct {
    const char* name;
    int pointer_size;
    
    // 寄存器定义
    const char** register_names;
    int register_count;
    
    // 指令集特性
    GumCpuFeature features;
    
    // 函数调用约定
    GumCallConvention call_convention;
    
    // 代码生成器
    GumCodeGenerator* codegen;
} GumCpuDescription;

// 支持的架构
static GumCpuDescription cpus[] = {
    {
        .name = "x86",
        .pointer_size = 4,
        .register_names = {"eax", "ebx", "ecx", "edx", ...},
        .features = CPU_FEATURE_CMOV | CPU_FEATURE_SSE,
        .call_convention = CALL_CONVENTION_CDECL
    },
    {
        .name = "x64",
        .pointer_size = 8,
        .register_names = {"rax", "rbx", "rcx", "rdx", ...},
        .features = CPU_FEATURE_SSE2 | CPU_FEATURE_AVX,
        .call_convention = CALL_CONVENTION_SYSTEM_V
    },
    {
        .name = "arm",
        .pointer_size = 4,
        .register_names = {"r0", "r1", "r2", "r3", ...},
        .features = CPU_FEATURE_THUMB | CPU_FEATURE_VFP,
        .call_convention = CALL_CONVENTION_ARM
    },
    {
        .name = "arm64",
        .pointer_size = 8,
        .register_names = {"x0", "x1", "x2", "x3", ...},
        .features = CPU_FEATURE_ARM64 | CPU_FEATURE_NEON,
        .call_convention = CALL_CONVENTION_ARM64
    },
    // MIPS, PPC, etc.
};
```

## 11. 实际工作流程示例

### 11.1 完整拦截流程

```python
# Python 脚本执行时发生的所有事情
def frida_instrumentation_workflow():
    # 阶段1: 准备阶段 (控制端)
    # ---------------------------------
    # 1.1 解析用户脚本
    js_code = read_user_script()
    
    # 1.2 编译为 Agent 代码
    agent_binary = compile_to_agent(js_code)
    
    # 1.3 建立设备连接
    device = connect_to_device("USB")
    
    # 阶段2: 注入阶段 (目标设备)
    # ---------------------------------
    # 2.1 附加到目标进程
    session = device.attach(pid)
    
    # 2.2 创建 Agent
    # 实际执行 ptrace/dlsym 等系统调用
    agent = create_agent_in_process(session, agent_binary)
    
    # 阶段3: 运行阶段 (目标进程)
    # ---------------------------------
    # 3.1 Agent 初始化
    # - 加载 frida-gum
    # - 初始化 JavaScript 运行时
    # - 暴露 Native API
    agent.initialize()
    
    # 3.2 执行用户脚本
    # - 解析 JavaScript
    # - 安装钩子
    # - 设置回调
    agent.execute_script(js_code)
    
    # 3.3 建立通信通道
    # - 创建消息队列
    # - 设置信号处理器
    # - 启动事件循环
    agent.start_message_loop()
    
    # 阶段4: 交互阶段 (双向通信)
    # ---------------------------------
    # 4.1 用户调用 API
    # Python: script.exports.my_function()
    #    ↓ 序列化为消息
    #    ↓ 通过 USB/网络发送
    #    ↓ Agent 接收并反序列化
    #    ↓ 调用对应的 JavaScript 函数
    #    ↓ 返回结果，反向传输
    
    # 4.2 事件回调
    # Agent: 检测到函数调用
    #    ↓ 调用 JavaScript 回调
    #    ↓ 产生消息发送到控制端
    #    ↓ Python 的 on_message 被调用
    
    # 阶段5: 清理阶段
    # ---------------------------------
    # 5.1 用户停止脚本
    # 5.2 移除所有钩子
    # 5.3 释放内存
    # 5.4 从进程分离
```

## 12. 高级技术深度解析

### 12.1 动态代码生成

```c
// gum/arch-x86/relocator.c - 指令重定位
void relocator_relocate(GumRelocator* self, 
                       const uint8_t* input_code,
                       uint8_t* output_code) {
    // 指令解码器
    ZydisDecoder decoder;
    ZydisDecodedInstruction instruction;
    
    while (ZYAN_SUCCESS(ZydisDecoderDecodeBuffer(&decoder, 
                                                 input_code,
                                                 MAX_INSTRUCTION_LENGTH,
                                                 &instruction))) {
        // 分析指令类型
        switch (instruction.mnemonic) {
            case ZYDIS_MNEMONIC_CALL:
            case ZYDIS_MNEMONIC_JMP:
                // 处理相对跳转/调用
                relocate_relative_branch(&instruction, 
                                        input_code,
                                        output_code,
                                        self->base_difference);
                break;
                
            case ZYDIS_MNEMONIC_MOV:
                // 处理 RIP 相对寻址
                if (instruction.attributes & ZYDIS_ATTRIB_IS_RELATIVE) {
                    relocate_rip_relative(&instruction,
                                         input_code,
                                         output_code,
                                         self->base_difference);
                }
                break;
                
            default:
                // 直接复制指令
                memcpy(output_code, input_code, instruction.length);
                break;
        }
        
        input_code += instruction.length;
        output_code += instruction.length;
    }
}
```

### 12.2 信号处理与异常恢复

```c
// 信号处理器确保稳定性
static void signal_handler(int signo, siginfo_t* info, void* context) {
    GumExceptionDetails details;
    
    // 1. 收集异常信息
    details.type = signo;
    details.address = info->si_addr;
    details.memory_operation = get_memory_operation(info);
    
    // 2. 检查是否是 Frida 引起的异常
    if (is_frida_related_exception(&details)) {
        // 3. 恢复执行上下文
        ucontext_t* uc = (ucontext_t*)context;
        
        // 4. 如果是无效内存访问，模拟成功
        if (signo == SIGSEGV || signo == SIGBUS) {
            // 模拟内存读取返回0
            uc->uc_mcontext.gregs[REG_RAX] = 0;
            
            // 跳过故障指令
            uc->uc_mcontext.gregs[REG_RIP] += get_instruction_length(
                uc->uc_mcontext.gregs[REG_RIP]);
            
            // 继续执行
            return;
        }
    }
    
    // 5. 不是 Frida 引起的异常，传递给原来的处理器
    original_signal_handler(signo, info, context);
}
```

## 13. 总结

Frida 的核心原理可以概括为：

1. **注入技术**：使用平台特定的注入机制将 Agent 加载到目标进程
2. **动态插桩**：通过修改指令实现函数拦截和指令追踪
3. **脚本运行时**：嵌入 JavaScript 引擎提供灵活的脚本能力
4. **进程间通信**：建立控制端和目标端的双向通信通道
5. **跨平台抽象**：通过抽象层支持多种操作系统和 CPU 架构

其技术栈包括：
- **底层**：ptrace, dlopen, mach_vm, VirtualAllocEx
- **中间层**：指令解码、代码生成、内存管理
- **上层**：JavaScript API、RPC、工具链
