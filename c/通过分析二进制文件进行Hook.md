# 通过分析HelloWorld二进制进行Hook

## 一、分析HelloWorld二进制

### 1.1 编译并分析二进制文件

```bash
# 编译测试程序（禁用PIE，便于分析）
gcc -o hello hello.c -no-pie -g

# 查看二进制信息
file hello
readelf -h hello

# 查看函数地址
nm hello | grep -E "print_message|add_numbers|main"
objdump -t hello | grep -E "print_message|add_numbers|main"
```

### 1.2 获取函数地址
使用以下命令获取函数地址：
```bash
# 方法1: 使用nm
nm -n hello | grep -E "T print_message$|T add_numbers$|T main$"

# 方法2: 使用objdump
objdump -d hello | grep -A 10 "<print_message>:"
objdump -d hello | grep -A 10 "<add_numbers>:"

# 方法3: 使用readelf
readelf -s hello | grep -E "print_message|add_numbers|main"
```

## 二、基于二进制分析的Hook工具

### 2.1 完整的二进制分析Hook工具

```c
// binary_hook.c - 基于二进制分析的Hook工具
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <elf.h>
#include <sys/mman.h>

#define MAX_PATH 256
#define MAX_SYMBOLS 1000

// 符号表条目
typedef struct {
    char name[256];
    Elf64_Addr value;
    Elf64_Xword size;
    unsigned char type;
    unsigned char bind;
} SymbolEntry;

// ELF解析上下文
typedef struct {
    int fd;
    Elf64_Ehdr ehdr;
    Elf64_Shdr *shdrs;
    char *shstrtab;
    char *strtab;
    SymbolEntry *symbols;
    int symbol_count;
} ELFContext;

// 函数地址信息
typedef struct {
    char name[256];
    unsigned long address;
    unsigned char original_bytes[16];
    int hooked;
} FunctionInfo;

// 全局变量
ELFContext elf_ctx;
FunctionInfo functions[100];
int function_count = 0;

// 解析ELF文件
int parse_elf(const char *path, ELFContext *ctx) {
    struct stat st;
    
    // 打开文件
    ctx->fd = open(path, O_RDONLY);
    if (ctx->fd < 0) {
        perror("open");
        return -1;
    }
    
    // 获取文件大小
    if (fstat(ctx->fd, &st) < 0) {
        perror("fstat");
        close(ctx->fd);
        return -1;
    }
    
    // 映射文件到内存
    void *map = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, ctx->fd, 0);
    if (map == MAP_FAILED) {
        perror("mmap");
        close(ctx->fd);
        return -1;
    }
    
    // 解析ELF头
    memcpy(&ctx->ehdr, map, sizeof(Elf64_Ehdr));
    
    // 验证ELF魔数
    if (memcmp(ctx->ehdr.e_ident, ELFMAG, SELFMAG) != 0) {
        fprintf(stderr, "Not an ELF file\n");
        munmap(map, st.st_size);
        close(ctx->fd);
        return -1;
    }
    
    // 读取节头表
    ctx->shdrs = (Elf64_Shdr *)((char *)map + ctx->ehdr.e_shoff);
    
    // 读取节头字符串表
    Elf64_Shdr shstrtab_hdr = ctx->shdrs[ctx->ehdr.e_shstrndx];
    ctx->shstrtab = (char *)map + shstrtab_hdr.sh_offset;
    
    // 查找符号表和字符串表
    Elf64_Shdr symtab_hdr, strtab_hdr;
    int found_symtab = 0, found_strtab = 0;
    
    for (int i = 0; i < ctx->ehdr.e_shnum; i++) {
        char *name = ctx->shstrtab + ctx->shdrs[i].sh_name;
        
        if (ctx->shdrs[i].sh_type == SHT_SYMTAB) {
            symtab_hdr = ctx->shdrs[i];
            found_symtab = 1;
        } else if (ctx->shdrs[i].sh_type == SHT_STRTAB && 
                   strcmp(name, ".strtab") == 0) {
            strtab_hdr = ctx->shdrs[i];
            ctx->strtab = (char *)map + strtab_hdr.sh_offset;
            found_strtab = 1;
        }
    }
    
    if (!found_symtab || !found_strtab) {
        fprintf(stderr, "Symbol table or string table not found\n");
        munmap(map, st.st_size);
        close(ctx->fd);
        return -1;
    }
    
    // 读取符号表
    int num_symbols = symtab_hdr.sh_size / sizeof(Elf64_Sym);
    Elf64_Sym *symtab = (Elf64_Sym *)((char *)map + symtab_hdr.sh_offset);
    
    ctx->symbols = malloc(num_symbols * sizeof(SymbolEntry));
    ctx->symbol_count = 0;
    
    for (int i = 0; i < num_symbols; i++) {
        Elf64_Sym *sym = &symtab[i];
        unsigned char type = ELF64_ST_TYPE(sym->st_info);
        unsigned char bind = ELF64_ST_BIND(sym->st_info);
        
        // 只关心函数和全局/弱符号
        if (sym->st_name != 0 && (type == STT_FUNC || type == STT_NOTYPE) &&
            (bind == STB_GLOBAL || bind == STB_WEAK) && sym->st_size > 0) {
            
            SymbolEntry *entry = &ctx->symbols[ctx->symbol_count];
            strncpy(entry->name, ctx->strtab + sym->st_name, sizeof(entry->name)-1);
            entry->name[sizeof(entry->name)-1] = '\0';
            entry->value = sym->st_value;
            entry->size = sym->st_size;
            entry->type = type;
            entry->bind = bind;
            
            ctx->symbol_count++;
        }
    }
    
    munmap(map, st.st_size);
    return 0;
}

// 释放ELF解析上下文
void free_elf_context(ELFContext *ctx) {
    if (ctx->symbols) {
        free(ctx->symbols);
        ctx->symbols = NULL;
    }
    if (ctx->fd >= 0) {
        close(ctx->fd);
        ctx->fd = -1;
    }
}

// 查找函数地址（考虑ASLR）
unsigned long find_function_address(pid_t pid, const char *func_name, 
                                   ELFContext *ctx) {
    // 首先从ELF中获取函数偏移
    unsigned long offset = 0;
    
    for (int i = 0; i < ctx->symbol_count; i++) {
        if (strcmp(ctx->symbols[i].name, func_name) == 0) {
            offset = ctx->symbols[i].value;
            printf("Found %s at offset: 0x%lx\n", func_name, offset);
            break;
        }
    }
    
    if (offset == 0) {
        fprintf(stderr, "Function %s not found in symbol table\n", func_name);
        return 0;
    }
    
    // 获取进程的内存映射以找到基地址
    char maps_path[MAX_PATH];
    sprintf(maps_path, "/proc/%d/maps", pid);
    
    FILE *maps = fopen(maps_path, "r");
    if (!maps) {
        perror("fopen maps");
        return 0;
    }
    
    char line[512];
    unsigned long base_address = 0;
    
    while (fgets(line, sizeof(line), maps)) {
        if (strstr(line, "hello") || strstr(line, "/home/")) {
            unsigned long start, end;
            char perms[5];
            
            if (sscanf(line, "%lx-%lx %4s", &start, &end, perms) == 3) {
                if (strstr(perms, "x")) {  // 可执行段
                    base_address = start;
                    printf("Found executable segment at: 0x%lx\n", base_address);
                    break;
                }
            }
        }
    }
    
    fclose(maps);
    
    if (base_address == 0) {
        fprintf(stderr, "Could not find base address\n");
        return 0;
    }
    
    unsigned long actual_address = base_address + offset;
    printf("Calculated address for %s: 0x%lx (base: 0x%lx + offset: 0x%lx)\n", 
           func_name, actual_address, base_address, offset);
    
    return actual_address;
}

// 读取进程内存
long read_process_memory(pid_t pid, unsigned long addr, void *buffer, size_t size) {
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

// 写入进程内存
long write_process_memory(pid_t pid, unsigned long addr, void *buffer, size_t size) {
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

// 安装Hook（使用跳转指令）
int install_hook(pid_t pid, unsigned long target_addr, const char *func_name) {
    // 保存原始字节
    unsigned char original[16];
    if (read_process_memory(pid, target_addr, original, sizeof(original)) == -1) {
        fprintf(stderr, "Failed to read original bytes\n");
        return -1;
    }
    
    // 保存到函数信息
    FunctionInfo *func = &functions[function_count];
    strncpy(func->name, func_name, sizeof(func->name)-1);
    func->address = target_addr;
    memcpy(func->original_bytes, original, sizeof(original));
    func->hooked = 1;
    function_count++;
    
    printf("Original bytes at 0x%lx (%s): ", target_addr, func_name);
    for (int i = 0; i < 8; i++) {
        printf("%02x ", original[i]);
    }
    printf("\n");
    
    // 创建跳转指令（相对跳转）
    // jmpq *$rip + offset (FF 25 00 00 00 00 + 8字节地址)
    unsigned char jump_code[] = {
        0xFF, 0x25, 0x00, 0x00, 0x00, 0x00,  // jmpq *0x0(%rip)
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  // 目标地址
    };
    
    // 由于我们不能在目标进程中分配内存，这里使用一个简单的trampoline
    // 对于演示，我们使用一个更简单的方法：直接修改为非法指令触发信号
    unsigned char trap_code[] = {0xCC};  // int3
    
    printf("Installing int3 breakpoint at 0x%lx\n", target_addr);
    
    // 写入断点
    if (write_process_memory(pid, target_addr, trap_code, sizeof(trap_code)) == -1) {
        fprintf(stderr, "Failed to install hook\n");
        return -1;
    }
    
    return 0;
}

// 处理断点
void handle_breakpoint(pid_t pid, unsigned long addr, struct user_regs_struct *regs) {
    printf("[BREAKPOINT] Hit at 0x%lx\n", addr);
    
    // 查找哪个函数被触发
    for (int i = 0; i < function_count; i++) {
        if (functions[i].address == addr) {
            printf("[HOOK] Function %s called!\n", functions[i].name);
            
            // 获取寄存器值
            printf("  RDI: 0x%lx\n", regs->rdi);
            printf("  RSI: 0x%lx\n", regs->rsi);
            printf("  RDX: 0x%lx\n", regs->rdx);
            printf("  RCX: 0x%lx\n", regs->rcx);
            
            // 如果是print_message，读取字符串参数
            if (strstr(functions[i].name, "print_message")) {
                char buffer[256] = {0};
                if (regs->rdi != 0) {
                    read_process_memory(pid, regs->rdi, buffer, sizeof(buffer)-1);
                    printf("  String argument: %s\n", buffer);
                    
                    // 修改字符串（可选）
                    char new_str[] = "[HOOKED] Hello from hook!";
                    write_process_memory(pid, regs->rdi, new_str, strlen(new_str) + 1);
                    printf("  Modified string to: %s\n", new_str);
                }
            }
            break;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <pid> <binary_path> [function1] [function2] ...\n", argv[0]);
        printf("Example: %s $(pidof hello) ./hello print_message add_numbers\n", argv[0]);
        return 1;
    }
    
    pid_t target_pid = atoi(argv[1]);
    const char *binary_path = argv[2];
    
    // 解析ELF文件
    printf("Parsing ELF file: %s\n", binary_path);
    if (parse_elf(binary_path, &elf_ctx) == -1) {
        fprintf(stderr, "Failed to parse ELF file\n");
        return 1;
    }
    
    printf("Found %d symbols\n", elf_ctx.symbol_count);
    
    // 附加到目标进程
    if (ptrace(PTRACE_ATTACH, target_pid, NULL, NULL) == -1) {
        perror("ptrace ATTACH");
        free_elf_context(&elf_ctx);
        return 1;
    }
    
    // 等待进程停止
    int status;
    waitpid(target_pid, &status, 0);
    printf("Attached to process %d\n", target_pid);
    
    // 安装指定函数的Hook
    for (int i = 3; i < argc; i++) {
        const char *func_name = argv[i];
        unsigned long addr = find_function_address(target_pid, func_name, &elf_ctx);
        
        if (addr != 0) {
            if (install_hook(target_pid, addr, func_name) == 0) {
                printf("Successfully hooked %s at 0x%lx\n", func_name, addr);
            }
        }
    }
    
    // 如果没有指定函数，Hook所有找到的函数
    if (argc == 3) {
        printf("No functions specified, hooking main functions...\n");
        
        // Hook一些常见的函数
        const char *default_funcs[] = {"main", "print_message", "add_numbers", NULL};
        
        for (int i = 0; default_funcs[i] != NULL; i++) {
            unsigned long addr = find_function_address(target_pid, default_funcs[i], &elf_ctx);
            if (addr != 0) {
                install_hook(target_pid, addr, default_funcs[i]);
            }
        }
    }
    
    // 继续执行并等待断点
    ptrace(PTRACE_CONT, target_pid, NULL, NULL);
    
    while (1) {
        // 等待信号
        waitpid(target_pid, &status, 0);
        
        if (WIFEXITED(status)) {
            printf("Target process exited\n");
            break;
        }
        
        if (WIFSTOPPED(status)) {
            int sig = WSTOPSIG(status);
            
            if (sig == SIGTRAP) {
                // 获取寄存器状态
                struct user_regs_struct regs;
                ptrace(PTRACE_GETREGS, target_pid, NULL, &regs);
                
                // 断点地址是RIP-1
                unsigned long break_addr = regs.rip - 1;
                
                // 处理断点
                handle_breakpoint(target_pid, break_addr, &regs);
                
                // 恢复原始指令
                for (int i = 0; i < function_count; i++) {
                    if (functions[i].address == break_addr) {
                        // 恢复原始字节
                        write_process_memory(target_pid, break_addr, 
                                           functions[i].original_bytes, 1);
                        
                        // 设置RIP回到原指令
                        regs.rip = break_addr;
                        ptrace(PTRACE_SETREGS, target_pid, NULL, &regs);
                        
                        // 单步执行
                        ptrace(PTRACE_SINGLESTEP, target_pid, NULL, NULL);
                        waitpid(target_pid, &status, 0);
                        
                        // 重新设置断点
                        unsigned char trap = 0xCC;
                        write_process_memory(target_pid, break_addr, &trap, 1);
                        
                        break;
                    }
                }
            } else if (sig == SIGSTOP) {
                // 初始停止信号
                printf("Process stopped by SIGSTOP\n");
            } else {
                printf("Received signal: %d\n", sig);
            }
            
            // 继续执行
            ptrace(PTRACE_CONT, target_pid, NULL, NULL);
        }
    }
    
    // 清理
    ptrace(PTRACE_DETACH, target_pid, NULL, NULL);
    free_elf_context(&elf_ctx);
    
    printf("Detached from process\n");
    
    return 0;
}
```

### 2.2 编译和使用

```bash
# 编译Hook工具
gcc -o binary_hook binary_hook.c

# 运行测试程序（在终端1）
./hello

# 在终端2中，查找进程ID并运行Hook工具
PID=$(pidof hello)
./binary_hook $PID ./hello print_message add_numbers

# 或者Hook所有函数
./binary_hook $PID ./hello
```

## 三、更高级的Hook引擎

### 3.1 支持指令重定位的Hook引擎

```c
// advanced_hook.c - 高级Hook引擎
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/mman.h>
#include <elf.h>
#include <capstone/capstone.h>

// Capstone反汇编引擎
csh handle;

// 重定位的指令块
typedef struct {
    unsigned long address;      // 原始地址
    unsigned char *original;    // 原始指令
    unsigned char *relocated;   // 重定位后的指令
    size_t size;                // 指令块大小
    unsigned char *trampoline;  // 跳板代码
    size_t trampoline_size;     // 跳板大小
} RelocatedBlock;

// 安装支持指令重定位的Hook
RelocatedBlock *install_advanced_hook(pid_t pid, unsigned long target_addr, 
                                     unsigned long hook_func_addr) {
    // 初始化Capstone
    if (cs_open(CS_ARCH_X86, CS_MODE_64, &handle) != CS_ERR_OK) {
        fprintf(stderr, "Failed to initialize Capstone\n");
        return NULL;
    }
    
    cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON);
    
    // 读取原始指令
    unsigned char code[32];
    if (read_process_memory(pid, target_addr, code, sizeof(code)) == -1) {
        fprintf(stderr, "Failed to read code\n");
        cs_close(&handle);
        return NULL;
    }
    
    // 反汇编指令，找到需要重定位的指令
    cs_insn *insn;
    size_t count = cs_disasm(handle, code, sizeof(code), target_addr, 0, &insn);
    
    if (count == 0) {
        fprintf(stderr, "Failed to disassemble code\n");
        cs_close(&handle);
        return NULL;
    }
    
    // 计算需要重定位的指令大小（至少5字节，因为跳转指令需要5字节）
    size_t relocated_size = 0;
    size_t i;
    for (i = 0; i < count && relocated_size < 5; i++) {
        relocated_size += insn[i].size;
    }
    
    // 分配重定位块
    RelocatedBlock *block = malloc(sizeof(RelocatedBlock));
    block->address = target_addr;
    block->size = relocated_size;
    
    // 保存原始指令
    block->original = malloc(relocated_size);
    memcpy(block->original, code, relocated_size);
    
    // 创建跳板代码（原始指令 + 跳回原函数）
    block->trampoline_size = relocated_size + 14;  // 原始指令 + 跳转
    block->trampoline = malloc(block->trampoline_size);
    
    // 复制原始指令
    memcpy(block->trampoline, code, relocated_size);
    
    // 添加跳转回原函数继续执行的代码
    // jmp [rip]
    // .quad target_addr + relocated_size
    unsigned char jmp_back[] = {
        0xFF, 0x25, 0x00, 0x00, 0x00, 0x00,  // jmp [rip]
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  // 目标地址
    };
    
    unsigned long continue_addr = target_addr + relocated_size;
    memcpy(jmp_back + 6, &continue_addr, sizeof(continue_addr));
    memcpy(block->trampoline + relocated_size, jmp_back, sizeof(jmp_back));
    
    // 在目标进程中分配内存用于跳板
    // 使用mmap系统调用
    struct user_regs_struct regs;
    ptrace(PTRACE_GETREGS, pid, NULL, &regs);
    
    // 保存原始寄存器
    struct user_regs_struct saved_regs = regs;
    
    // 调用mmap分配内存
    regs.rax = 9;  // mmap系统调用号
    regs.rdi = 0;  // 地址（NULL表示由系统选择）
    regs.rsi = block->trampoline_size;
    regs.rdx = PROT_READ | PROT_WRITE | PROT_EXEC;
    regs.r10 = MAP_PRIVATE | MAP_ANONYMOUS;
    regs.r8 = -1;  // fd
    regs.r9 = 0;   // offset
    
    ptrace(PTRACE_SETREGS, pid, NULL, &regs);
    ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
    waitpid(pid, NULL, 0);
    
    // 获取mmap返回值
    ptrace(PTRACE_GETREGS, pid, NULL, &regs);
    unsigned long trampoline_addr = regs.rax;
    
    // 恢复寄存器
    ptrace(PTRACE_SETREGS, pid, NULL, &saved_regs);
    
    // 写入跳板代码到分配的内存
    for (size_t j = 0; j < block->trampoline_size; j += sizeof(long)) {
        size_t chunk_size = (block->trampoline_size - j < sizeof(long)) ? 
                           block->trampoline_size - j : sizeof(long);
        
        long data = 0;
        memcpy(&data, block->trampoline + j, chunk_size);
        ptrace(PTRACE_POKEDATA, pid, trampoline_addr + j, data);
    }
    
    // 创建跳转到Hook函数的指令
    unsigned char hook_jmp[] = {
        0xFF, 0x25, 0x00, 0x00, 0x00, 0x00,  // jmp [rip]
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  // hook函数地址
    };
    
    memcpy(hook_jmp + 6, &hook_func_addr, sizeof(hook_func_addr));
    
    // 写入跳转指令到原函数
    for (size_t j = 0; j < sizeof(hook_jmp); j += sizeof(long)) {
        size_t chunk_size = (sizeof(hook_jmp) - j < sizeof(long)) ? 
                           sizeof(hook_jmp) - j : sizeof(long);
        
        long data = 0;
        memcpy(&data, hook_jmp + j, chunk_size);
        ptrace(PTRACE_POKEDATA, pid, target_addr + j, data);
    }
    
    // 用NOP填充剩余空间
    if (sizeof(hook_jmp) < relocated_size) {
        unsigned char nop = 0x90;
        for (size_t j = sizeof(hook_jmp); j < relocated_size; j++) {
            ptrace(PTRACE_POKEDATA, pid, target_addr + j, nop);
        }
    }
    
    cs_free(insn, count);
    cs_close(&handle);
    
    printf("Advanced hook installed:\n");
    printf("  Target: 0x%lx\n", target_addr);
    printf("  Trampoline: 0x%lx\n", trampoline_addr);
    printf("  Hook function: 0x%lx\n", hook_func_addr);
    
    return block;
}
```

## 四、自动化分析脚本

### 4.1 Python分析脚本

```python
#!/usr/bin/env python3
# analyze_binary.py - 自动化分析二进制文件

import subprocess
import re
import struct
import sys

def get_function_addresses(binary_path):
    """使用nm获取函数地址"""
    result = {}
    
    try:
        # 运行nm命令
        nm_output = subprocess.check_output(['nm', '-n', binary_path]).decode('utf-8')
        
        for line in nm_output.split('\n'):
            if ' T ' in line or ' t ' in line:
                parts = line.strip().split()
                if len(parts) >= 3:
                    addr = int(parts[0], 16)
                    name = parts[-1]
                    
                    # 过滤掉编译器生成的和系统函数
                    if (not name.startswith('_') and 
                        not name.startswith('.') and
                        'plt' not in name and
                        'got' not in name):
                        result[name] = addr
    except Exception as e:
        print(f"Error running nm: {e}")
    
    return result

def get_disassembly(binary_path, function_name, address):
    """获取函数反汇编"""
    try:
        # 使用objdump反汇编
        cmd = ['objdump', '-d', '--disassemble=' + function_name, binary_path]
        disasm = subprocess.check_output(cmd).decode('utf-8')
        
        # 提取前几行指令
        lines = disasm.split('\n')
        instructions = []
        
        for line in lines[7:]:  # 跳过头部
            if line.strip() and ':' in line:
                # 提取指令字节
                match = re.search(r':\s+([0-9a-f\s]+)\s+', line)
                if match:
                    hex_bytes = match.group(1).replace(' ', '')
                    instructions.append(hex_bytes)
            
            if len(instructions) >= 10:  # 只取前10条指令
                break
        
        return instructions
    except Exception as e:
        print(f"Error disassembling: {e}")
        return []

def analyze_function(binary_path, function_name):
    """分析函数"""
    print(f"\nAnalyzing function: {function_name}")
    print("=" * 50)
    
    # 获取地址
    funcs = get_function_addresses(binary_path)
    if function_name in funcs:
        addr = funcs[function_name]
        print(f"Address: 0x{addr:x}")
        
        # 获取反汇编
        instructions = get_disassembly(binary_path, function_name, addr)
        print(f"First {len(instructions)} instructions:")
        
        for i, inst in enumerate(instructions):
            print(f"  {i:2d}: {inst}")
            
            # 分析指令类型
            if inst.startswith('55'):
                print("        -> push rbp (function prologue)")
            elif inst.startswith('4889e5'):
                print("        -> mov rbp, rsp (function prologue)")
            elif inst.startswith('c3'):
                print("        -> ret (function return)")
            elif inst.startswith('e8'):
                print("        -> call (relative call)")
            elif inst.startswith('ff'):
                print("        -> jmp/call (indirect)")
    else:
        print(f"Function {function_name} not found")
    
    return funcs.get(function_name, 0)

def find_string_references(binary_path, target_string):
    """查找字符串引用"""
    print(f"\nSearching for references to string: '{target_string}'")
    print("=" * 50)
    
    try:
        # 使用strings和grep
        strings_cmd = ['strings', binary_path]
        strings_output = subprocess.check_output(strings_cmd).decode('utf-8')
        
        for line in strings_output.split('\n'):
            if target_string in line:
                print(f"Found string: {line}")
                
                # 尝试查找引用地址
                grep_cmd = f"objdump -s {binary_path} | grep -B2 -A2 '{target_string[:20]}'"
                result = subprocess.run(grep_cmd, shell=True, capture_output=True, text=True)
                if result.stdout:
                    print("Possible references:")
                    print(result.stdout)
    except Exception as e:
        print(f"Error finding string references: {e}")

def main():
    if len(sys.argv) < 2:
        print("Usage: python analyze_binary.py <binary_path> [function_name]")
        sys.exit(1)
    
    binary_path = sys.argv[1]
    
    print(f"Analyzing binary: {binary_path}")
    print("=" * 60)
    
    # 获取所有函数
    all_funcs = get_function_addresses(binary_path)
    print(f"Found {len(all_funcs)} functions")
    
    # 显示前20个函数
    print("\nTop 20 functions:")
    for i, (name, addr) in enumerate(list(all_funcs.items())[:20]):
        print(f"  {i:2d}: 0x{addr:012x} {name}")
    
    # 分析特定函数
    if len(sys.argv) > 2:
        for func_name in sys.argv[2:]:
            analyze_function(binary_path, func_name)
    
    # 查找字符串引用
    find_string_references(binary_path, "Hello, World!")

if __name__ == "__main__":
    main()
```

### 4.2 使用脚本

```bash
# 安装依赖
python3 -m pip install capstone pyelftools

# 运行分析
python3 analyze_binary.py ./hello
python3 analyze_binary.py ./hello print_message add_numbers

# 生成Hook脚本
python3 analyze_binary.py ./hello > hook_plan.txt
```

## 五、使用示例和测试

### 5.1 完整测试流程

```bash
# 1. 编译测试程序
gcc -o hello hello.c -no-pie -g

# 2. 编译Hook工具
gcc -o binary_hook binary_hook.c

# 3. 在终端1运行测试程序
./hello

# 4. 在终端2分析二进制
python3 analyze_binary.py ./hello

# 5. 在终端3运行Hook
PID=$(pidof hello)
./binary_hook $PID ./hello print_message

# 6. 观察输出
# 终端1的输出会显示被Hook修改的字符串
```

### 5.2 预期输出

当Hook成功后，`hello`程序的输出会变为：
```
Message: [HOOKED] Hello from hook!
10 + 20 = 30
Sum: 30
Message: [HOOKED] Hello from hook!
10 + 20 = 30
Sum: 30
...
```

而Hook工具的输出会显示：
```
[BREAKPOINT] Hit at 0x4005a6
[HOOK] Function print_message called!
RDI: 0x7ffd12345678
String argument: Hello, World!
Modified string to: [HOOKED] Hello from hook!
```

## 六、总结

通过分析二进制进行Hook的关键步骤：

1. **ELF文件分析**：解析二进制文件的符号表、节头表等
2. **地址计算**：考虑ASLR，计算函数在内存中的实际地址
3. **指令分析**：反汇编目标函数，确定Hook位置
4. **指令重定位**：保存并重定位被覆盖的指令
5. **跳板代码**：创建执行原始指令并跳回的代码
6. **Hook安装**：写入跳转指令并设置断点处理

这种方法比简单的ptrace断点更强大，可以实现：
- 函数参数的读取和修改
- 返回值的拦截和修改
- 多次调用处理
- 线程安全（需要额外处理）
- 支持复杂的指令重定位

实际应用中，还需要考虑：
- 多线程同步
- 信号处理
- 异常处理
- 性能优化
- 兼容性（不同架构、编译器）
