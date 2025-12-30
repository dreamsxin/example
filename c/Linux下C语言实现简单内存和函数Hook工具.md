# Linux下C语言实现简单内存和函数Hook工具

## 一、目标程序：hello.c

```c
// hello.c - 测试程序
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void print_message(const char* msg) {
    printf("Message: %s\n", msg);
}

int add_numbers(int a, int b) {
    int result = a + b;
    printf("%d + %d = %d\n", a, b, result);
    return result;
}

void secret_function() {
    printf("This is a secret function!\n");
}

int main() {
    char buffer[64] = "Hello, World!";
    
    while (1) {
        print_message(buffer);
        int sum = add_numbers(10, 20);
        printf("Sum: %d\n", sum);
        sleep(2);
    }
    
    return 0;
}
```

编译测试程序：
```bash
gcc -o hello hello.c -no-pie  # 禁用PIE便于演示
```

## 二、内存Hook工具：memory_hook.c

```c
// memory_hook.c - 内存读写和修改工具
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <elf.h>
#include <dlfcn.h>

#define MAX_PATH 256

// 进程信息结构
typedef struct {
    pid_t pid;
    unsigned long base_addr;
    char maps_path[MAX_PATH];
} ProcessInfo;

// 内存区域信息
typedef struct {
    unsigned long start;
    unsigned long end;
    char perms[5];
    unsigned long offset;
    char dev[32];
    unsigned long inode;
    char pathname[MAX_PATH];
} MemoryRegion;

// 读取目标进程内存
long read_memory(pid_t pid, unsigned long addr, void *buffer, size_t size) {
    long ret = 0;
    unsigned long *ptr = (unsigned long *)buffer;
    size_t words = size / sizeof(long);
    
    for (size_t i = 0; i < words; i++) {
        long data = ptrace(PTRACE_PEEKDATA, pid, addr + i * sizeof(long), NULL);
        if (data == -1 && errno) {
            perror("ptrace PEEKDATA");
            return -1;
        }
        ptr[i] = data;
    }
    
    // 处理剩余字节
    size_t remaining = size % sizeof(long);
    if (remaining > 0) {
        long data = ptrace(PTRACE_PEEKDATA, pid, 
                          addr + words * sizeof(long), NULL);
        if (data == -1 && errno) {
            perror("ptrace PEEKDATA");
            return -1;
        }
        memcpy(ptr + words, &data, remaining);
    }
    
    return 0;
}

// 写入目标进程内存
long write_memory(pid_t pid, unsigned long addr, void *buffer, size_t size) {
    unsigned long *ptr = (unsigned long *)buffer;
    size_t words = size / sizeof(long);
    
    for (size_t i = 0; i < words; i++) {
        if (ptrace(PTRACE_POKEDATA, pid, addr + i * sizeof(long), ptr[i]) == -1) {
            perror("ptrace POKEDATA");
            return -1;
        }
    }
    
    // 处理剩余字节
    size_t remaining = size % sizeof(long);
    if (remaining > 0) {
        // 读取原始数据
        long original = ptrace(PTRACE_PEEKDATA, pid, 
                              addr + words * sizeof(long), NULL);
        if (original == -1 && errno) {
            perror("ptrace PEEKDATA");
            return -1;
        }
        
        // 合并数据
        memcpy(&original, ptr + words, remaining);
        
        // 写回
        if (ptrace(PTRACE_POKEDATA, pid, 
                  addr + words * sizeof(long), original) == -1) {
            perror("ptrace POKEDATA");
            return -1;
        }
    }
    
    return 0;
}

// 查找字符串内存
unsigned long find_string_in_memory(pid_t pid, const char *str) {
    char maps_path[MAX_PATH];
    sprintf(maps_path, "/proc/%d/maps", pid);
    
    FILE *maps = fopen(maps_path, "r");
    if (!maps) {
        perror("fopen maps");
        return 0;
    }
    
    char line[512];
    unsigned long found_addr = 0;
    
    while (fgets(line, sizeof(line), maps)) {
        unsigned long start, end;
        char perms[5], dev[32], pathname[MAX_PATH];
        unsigned long offset, inode;
        
        sscanf(line, "%lx-%lx %4s %lx %31s %lu %s", 
               &start, &end, perms, &offset, dev, &inode, pathname);
        
        // 只搜索可读内存区域
        if (strchr(perms, 'r')) {
            size_t str_len = strlen(str);
            char *buffer = malloc(end - start);
            
            if (read_memory(pid, start, buffer, end - start) == 0) {
                for (unsigned long i = 0; i < (end - start - str_len); i++) {
                    if (memcmp(buffer + i, str, str_len) == 0) {
                        found_addr = start + i;
                        free(buffer);
                        fclose(maps);
                        return found_addr;
                    }
                }
            }
            free(buffer);
        }
    }
    
    fclose(maps);
    return 0;
}

// 修改字符串内容
void modify_string(pid_t pid, unsigned long addr, const char *new_str) {
    printf("Modifying string at 0x%lx: '%s'\n", addr, new_str);
    write_memory(pid, addr, (void *)new_str, strlen(new_str) + 1);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <pid>\n", argv[0]);
        printf("Example: %s $(pidof hello)\n", argv[0]);
        return 1;
    }
    
    pid_t target_pid = atoi(argv[1]);
    
    // 附加到目标进程
    if (ptrace(PTRACE_ATTACH, target_pid, NULL, NULL) == -1) {
        perror("ptrace ATTACH");
        return 1;
    }
    
    // 等待进程停止
    waitpid(target_pid, NULL, 0);
    printf("Attached to process %d\n", target_pid);
    
    // 查找并修改"Hello, World!"字符串
    unsigned long hello_addr = find_string_in_memory(target_pid, "Hello, World!");
    if (hello_addr) {
        printf("Found 'Hello, World!' at address: 0x%lx\n", hello_addr);
        
        // 修改为新的字符串
        modify_string(target_pid, hello_addr, "Hooked by Memory Tool!");
        
        // 验证修改
        char buffer[64];
        read_memory(target_pid, hello_addr, buffer, sizeof(buffer));
        printf("Modified string: %s\n", buffer);
    } else {
        printf("String not found\n");
    }
    
    // 分离进程
    ptrace(PTRACE_DETACH, target_pid, NULL, NULL);
    printf("Detached from process\n");
    
    return 0;
}
```

编译内存Hook工具：
```bash
gcc -o memory_hook memory_hook.c
```

## 三、函数Hook工具：function_hook.c

```c
// function_hook.c - 函数Hook工具
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <elf.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include <capstone/capstone.h>

#define MAX_PATH 256
#define SHELLCODE_SIZE 32

// 汇编代码：跳转到我们的函数
unsigned char jump_code[] = {
    0x68, 0x00, 0x00, 0x00, 0x00,    // push <hook_function_addr>
    0xC3                             // ret
};

// 保存原始字节
unsigned char original_bytes[16];
unsigned long original_addr = 0;

// Hook函数（用C内联汇编实现）
__attribute__((naked)) void hook_function() {
    __asm__ volatile(
        // 保存寄存器
        "push %rax\n"
        "push %rbx\n"
        "push %rcx\n"
        "push %rdx\n"
        "push %rsi\n"
        "push %rdi\n"
        
        // 调用我们的处理函数
        "mov %rsp, %rdi\n"           // 传递栈指针给C函数
        "call handle_hook\n"
        
        // 恢复寄存器
        "pop %rdi\n"
        "pop %rsi\n"
        "pop %rdx\n"
        "pop %rcx\n"
        "pop %rbx\n"
        "pop %rax\n"
        
        // 跳回原函数（跳过我们插入的hook代码）
        "push %rax\n"
        "mov $0x%lx, %rax\n"         // 原函数地址+被覆盖的字节数
        "xchg (%rsp), %rax\n"
        "ret\n"
        : : "i"(original_addr + sizeof(jump_code))
    );
}

// Hook处理函数
void handle_hook(struct user_regs_struct *regs) {
    printf("[HOOK] Function called!\n");
    printf("[HOOK] RDI: 0x%lx\n", regs->rdi);
    printf("[HOOK] RSI: 0x%lx\n", regs->rsi);
    
    // 修改参数（示例：修改第一个参数）
    if (regs->rdi != 0) {
        // 读取原始字符串
        char buffer[128];
        read_memory(getpid(), regs->rdi, buffer, sizeof(buffer));
        printf("[HOOK] Original string: %s\n", buffer);
        
        // 可以在这里修改参数
    }
}

// 读取进程内存（复用前面的函数）
long read_memory(pid_t pid, unsigned long addr, void *buffer, size_t size);

// 查找函数地址（通过符号表）
unsigned long find_function_address(pid_t pid, const char *func_name) {
    char maps_path[MAX_PATH];
    sprintf(maps_path, "/proc/%d/maps", pid);
    
    FILE *maps = fopen(maps_path, "r");
    if (!maps) {
        perror("fopen maps");
        return 0;
    }
    
    char line[512];
    unsigned long func_addr = 0;
    
    while (fgets(line, sizeof(line), maps)) {
        if (strstr(line, "hello")) {  // 查找我们的目标程序
            unsigned long start, end;
            char perms[5];
            sscanf(line, "%lx-%lx %4s", &start, &end, perms);
            
            // 读取ELF头
            Elf64_Ehdr ehdr;
            read_memory(pid, start, &ehdr, sizeof(ehdr));
            
            if (memcmp(ehdr.e_ident, ELFMAG, SELFMAG) == 0) {
                // 读取节头表
                Elf64_Shdr *shdr = malloc(ehdr.e_shentsize * ehdr.e_shnum);
                read_memory(pid, start + ehdr.e_shoff, shdr, 
                           ehdr.e_shentsize * ehdr.e_shnum);
                
                // 查找符号表
                for (int i = 0; i < ehdr.e_shnum; i++) {
                    if (shdr[i].sh_type == SHT_SYMTAB) {
                        Elf64_Sym *symtab = malloc(shdr[i].sh_size);
                        read_memory(pid, start + shdr[i].sh_offset, 
                                   symtab, shdr[i].sh_size);
                        
                        // 读取字符串表
                        Elf64_Shdr strtab_hdr = shdr[shdr[i].sh_link];
                        char *strtab = malloc(strtab_hdr.sh_size);
                        read_memory(pid, start + strtab_hdr.sh_offset, 
                                   strtab, strtab_hdr.sh_size);
                        
                        // 查找函数符号
                        int num_syms = shdr[i].sh_size / sizeof(Elf64_Sym);
                        for (int j = 0; j < num_syms; j++) {
                            if (ELF64_ST_TYPE(symtab[j].st_info) == STT_FUNC &&
                                strcmp(strtab + symtab[j].st_name, func_name) == 0) {
                                func_addr = start + symtab[j].st_value;
                                break;
                            }
                        }
                        
                        free(symtab);
                        free(strtab);
                        break;
                    }
                }
                free(shdr);
            }
        }
    }
    
    fclose(maps);
    return func_addr;
}

// 安装函数Hook
int install_hook(pid_t pid, unsigned long target_addr) {
    printf("Installing hook at address: 0x%lx\n", target_addr);
    
    // 保存原始代码
    if (read_memory(pid, target_addr, original_bytes, sizeof(original_bytes)) == -1) {
        return -1;
    }
    
    original_addr = target_addr;
    
    // 在目标进程中分配内存用于我们的hook函数
    struct user_regs_struct regs;
    
    // 获取当前寄存器状态
    ptrace(PTRACE_GETREGS, pid, NULL, &regs);
    
    // 使用mmap系统调用分配内存
    unsigned long mmap_addr = 0x10000000;  // 建议的地址
    
    // 设置mmap参数
    regs.rax = 9;  // mmap系统调用号
    regs.rdi = mmap_addr;  // 地址
    regs.rsi = 4096;  // 大小
    regs.rdx = PROT_READ | PROT_WRITE | PROT_EXEC;  // 权限
    regs.r10 = MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED;  // 标志
    regs.r8 = -1;  // 文件描述符
    regs.r9 = 0;  // 偏移
    
    ptrace(PTRACE_SETREGS, pid, NULL, &regs);
    ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
    waitpid(pid, NULL, 0);
    
    // 获取mmap返回值
    ptrace(PTRACE_GETREGS, pid, NULL, &regs);
    unsigned long allocated_addr = regs.rax;
    
    printf("Allocated memory at: 0x%lx\n", allocated_addr);
    
    // 准备hook代码
    unsigned long hook_func_addr = allocated_addr + 0x100;
    *(unsigned long*)(jump_code + 1) = hook_func_addr;
    
    // 写入hook代码到目标进程
    write_memory(pid, target_addr, jump_code, sizeof(jump_code));
    
    return 0;
}

// 主函数
int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <pid> <function_name>\n", argv[0]);
        printf("Example: %s $(pidof hello) print_message\n", argv[0]);
        return 1;
    }
    
    pid_t target_pid = atoi(argv[1]);
    const char *func_name = argv[2];
    
    // 附加到目标进程
    if (ptrace(PTRACE_ATTACH, target_pid, NULL, NULL) == -1) {
        perror("ptrace ATTACH");
        return 1;
    }
    
    waitpid(target_pid, NULL, 0);
    printf("Attached to process %d\n", target_pid);
    
    // 查找函数地址
    unsigned long func_addr = find_function_address(target_pid, func_name);
    if (func_addr == 0) {
        // 尝试通过其他方式查找
        char cmd[256];
        sprintf(cmd, "nm hello | grep ' T %s$'", func_name);
        FILE *fp = popen(cmd, "r");
        if (fp) {
            char line[256];
            if (fgets(line, sizeof(line), fp)) {
                sscanf(line, "%lx", &func_addr);
            }
            pclose(fp);
        }
    }
    
    if (func_addr == 0) {
        printf("Function %s not found\n", func_name);
        ptrace(PTRACE_DETACH, target_pid, NULL, NULL);
        return 1;
    }
    
    printf("Function %s found at address: 0x%lx\n", func_name, func_addr);
    
    // 安装Hook
    if (install_hook(target_pid, func_addr) == 0) {
        printf("Hook installed successfully\n");
    } else {
        printf("Failed to install hook\n");
    }
    
    // 分离进程
    ptrace(PTRACE_DETACH, target_pid, NULL, NULL);
    printf("Detached from process\n");
    
    return 0;
}
```

编译函数Hook工具（需要安装Capstone）：
```bash
# 安装Capstone
sudo apt-get install libcapstone-dev

# 编译
gcc -o function_hook function_hook.c -lcapstone
```

## 四、简化版Hook工具：simple_hook.c

```c
// simple_hook.c - 简化版Hook工具
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <errno.h>
#include <unistd.h>

// 简单的int3断点Hook
int install_breakpoint(pid_t pid, unsigned long addr) {
    // 保存原始字节
    long original = ptrace(PTRACE_PEEKDATA, pid, addr, NULL);
    if (original == -1 && errno) {
        perror("ptrace PEEKDATA");
        return -1;
    }
    
    printf("Original instruction at 0x%lx: 0x%lx\n", addr, original);
    
    // 写入int3断点 (0xCC)
    long modified = (original & ~0xFF) | 0xCC;
    if (ptrace(PTRACE_POKEDATA, pid, addr, modified) == -1) {
        perror("ptrace POKEDATA");
        return -1;
    }
    
    printf("Breakpoint installed at 0x%lx\n", addr);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <pid> [address]\n", argv[0]);
        return 1;
    }
    
    pid_t target_pid = atoi(argv[1]);
    unsigned long target_addr = 0;
    
    if (argc > 2) {
        target_addr = strtoul(argv[2], NULL, 16);
    } else {
        // 如果没有指定地址，使用print_message函数的常见地址
        target_addr = 0x4005a6;  // 需要根据实际调整
    }
    
    // 附加到进程
    if (ptrace(PTRACE_ATTACH, target_pid, NULL, NULL) == -1) {
        perror("ptrace ATTACH");
        return 1;
    }
    
    waitpid(target_pid, NULL, 0);
    
    // 安装断点
    install_breakpoint(target_pid, target_addr);
    
    // 继续执行
    ptrace(PTRACE_CONT, target_pid, NULL, NULL);
    
    // 等待断点触发
    int status;
    waitpid(target_pid, &status, 0);
    
    if (WIFSTOPPED(status) && WSTOPSIG(status) == SIGTRAP) {
        printf("Breakpoint hit!\n");
        
        // 获取寄存器状态
        struct user_regs_struct regs;
        ptrace(PTRACE_GETREGS, target_pid, NULL, &regs);
        
        printf("RIP: 0x%lx\n", regs.rip);
        printf("RDI: 0x%lx\n", regs.rdi);  // 第一个参数
        
        // 读取字符串参数
        if (regs.rdi != 0) {
            char buffer[128] = {0};
            for (int i = 0; i < sizeof(buffer) - 1; i++) {
                long word = ptrace(PTRACE_PEEKDATA, target_pid, regs.rdi + i, NULL);
                buffer[i] = word & 0xFF;
                if (buffer[i] == '\0') break;
            }
            printf("String argument: %s\n", buffer);
        }
        
        // 恢复原始指令
        regs.rip -= 1;  // 回退RIP
        ptrace(PTRACE_SETREGS, target_pid, NULL, &regs);
        
        // 恢复原始字节
        long original = ptrace(PTRACE_PEEKDATA, pid, target_addr, NULL);
        original = (original & ~0xFF) | (original_data & 0xFF);
        ptrace(PTRACE_POKEDATA, target_pid, target_addr, original);
    }
    
    // 分离
    ptrace(PTRACE_DETACH, target_pid, NULL, NULL);
    
    return 0;
}
```

编译简化版：
```bash
gcc -o simple_hook simple_hook.c
```

## 五、使用说明

### 1. 编译所有程序
```bash
# 编译测试程序
gcc -o hello hello.c -no-pie

# 编译内存Hook工具
gcc -o memory_hook memory_hook.c

# 编译函数Hook工具（需要capstone）
gcc -o function_hook function_hook.c -lcapstone

# 编译简化版Hook工具
gcc -o simple_hook simple_hook.c
```

### 2. 运行测试
```bash
# 终端1：运行测试程序
./hello

# 终端2：查找进程ID
pidof hello

# 使用内存Hook工具修改字符串
sudo ./memory_hook $(pidof hello)

# 使用简化版Hook工具
sudo ./simple_hook $(pidof hello) 0x4005a6
```

## 六、原理说明

### 1. 内存Hook原理
- 使用`ptrace`系统调用附加到目标进程
- 通过`PTRACE_PEEKDATA`和`PTRACE_POKEDATA`读写内存
- 扫描`/proc/<pid>/maps`查找内存区域
- 在可读内存区域搜索目标字符串

### 2. 函数Hook原理
- **断点Hook**：使用`int3`指令（0xCC）插入断点
- **跳转Hook**：修改函数开头为跳转指令
- **PLT/GOT Hook**：修改动态链接表（更高级）
- **Inline Hook**：直接修改函数代码

### 3. 关键技术点
- `ptrace`系统调用：进程跟踪和控制
- `/proc/<pid>/maps`：进程内存映射信息
- ELF格式解析：查找函数地址
- 信号处理：处理断点触发的SIGTRAP信号

## 七、注意事项

1. **需要root权限**：`ptrace`需要特权
2. **进程状态**：确保目标进程已停止
3. **并发安全**：多线程程序需要特殊处理
4. **指令长度**：Hook时要考虑指令对齐和长度
5. **恢复机制**：记得恢复原始代码
