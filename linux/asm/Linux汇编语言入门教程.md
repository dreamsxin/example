# Linux汇编语言入门教程（以C语言为对比）

## 第一部分：环境准备与基础知识

### 1.1 环境配置

```bash
# 安装必要的工具
sudo apt-get update
sudo apt-get install build-essential gcc gdb nasm

# 验证安装
gcc --version
nasm --version
```

### 1.2 基础概念对比

| C语言概念 | 汇编对应 | 说明 |
|-----------|----------|------|
| 变量 | 寄存器/内存位置 | 汇编中直接操作寄存器和内存 |
| 函数 | 标签(label) + 调用指令 | 汇编使用标签表示代码位置 |
| 控制结构 | 跳转指令 | if/for/while用条件跳转实现 |
| 指针 | 内存地址 | 汇编直接使用内存地址 |

## 第二部分：第一个程序对比

### C语言版本
```c
// hello.c
#include <stdio.h>

int main() {
    printf("Hello, World!\n");
    return 0;
}
```

### 汇编版本（x86_64 Linux）
```nasm
; hello.asm - NASM语法
section .data
    msg db 'Hello, World!', 0xa   ; 字符串+换行
    len equ $ - msg               ; 计算字符串长度

section .text
    global _start                 ; 入口点

_start:
    ; write(1, msg, len)
    mov rax, 1          ; 系统调用号 1 = write
    mov rdi, 1          ; 文件描述符 1 = stdout
    mov rsi, msg        ; 字符串地址
    mov rdx, len        ; 字符串长度
    syscall             ; 调用内核

    ; exit(0)
    mov rax, 60         ; 系统调用号 60 = exit
    mov rdi, 0          ; 退出码 0
    syscall
```

### 编译运行
```bash
# C语言编译运行
gcc hello.c -o hello_c
./hello_c

# 汇编编译运行
nasm -f elf64 hello.asm -o hello.o
ld hello.o -o hello_asm
./hello_asm
```

## 第三部分：变量与数据类型对比

### C语言示例
```c
#include <stdio.h>

int main() {
    // 变量声明与初始化
    int a = 10;
    int b = 20;
    int sum = a + b;
    
    // 数组
    int arr[5] = {1, 2, 3, 4, 5};
    
    // 字符串
    char str[] = "Hello";
    
    printf("Sum: %d\n", sum);
    return 0;
}
```

### 汇编版本
```nasm
section .data
    ; 初始化数据
    a dd 10            ; 32位整数
    b dd 20
    sum dd 0
    
    ; 数组
    arr dd 1, 2, 3, 4, 5
    
    ; 字符串
    str db 'Hello', 0
    
    ; 格式化字符串
    fmt db 'Sum: %d', 0xa, 0

section .bss
    ; 未初始化数据
    buffer resb 100    ; 保留100字节空间

section .text
    global main
    extern printf      ; 使用C标准库函数

main:
    push rbp
    mov rbp, rsp
    
    ; 加载变量到寄存器
    mov eax, [a]       ; 将a的值加载到eax
    add eax, [b]       ; 加上b的值
    mov [sum], eax     ; 保存结果到sum
    
    ; 调用printf
    mov rdi, fmt       ; 第一个参数：格式字符串
    mov esi, [sum]     ; 第二个参数：sum的值
    xor eax, eax       ; 对于整数参数，清空eax
    call printf
    
    pop rbp
    ret
```

## 第四部分：控制结构对比

### C语言控制结构
```c
#include <stdio.h>

int main() {
    int i;
    
    // if-else 示例
    int x = 10;
    if (x > 5) {
        printf("x > 5\n");
    } else {
        printf("x <= 5\n");
    }
    
    // for 循环示例
    for (i = 0; i < 5; i++) {
        printf("i = %d\n", i);
    }
    
    // while 循环示例
    i = 0;
    while (i < 3) {
        printf("While loop: %d\n", i);
        i++;
    }
    
    return 0;
}
```

### 汇编控制结构实现
```nasm
section .data
    x dd 10
    msg1 db 'x > 5', 0xa, 0
    msg2 db 'x <= 5', 0xa, 0
    fmt db 'i = %d', 0xa, 0
    fmt2 db 'While loop: %d', 0xa, 0

section .text
    global main
    extern printf

main:
    push rbp
    mov rbp, rsp
    
    ; ====== if-else 实现 ======
    mov eax, [x]
    cmp eax, 5          ; 比较 x 和 5
    jle else_label      ; 如果小于等于，跳转到else
    
    ; if 部分
    mov rdi, msg1
    call printf
    jmp if_end
    
else_label:
    ; else 部分
    mov rdi, msg2
    call printf
    
if_end:
    
    ; ====== for 循环实现 ======
    mov ebx, 0          ; i = 0
for_loop:
    cmp ebx, 5          ; 比较 i 和 5
    jge for_end         ; 如果 i >= 5，跳出循环
    
    ; 循环体
    mov rdi, fmt
    mov esi, ebx
    xor eax, eax
    call printf
    
    inc ebx             ; i++
    jmp for_loop        ; 跳回循环开始
    
for_end:
    
    ; ====== while 循环实现 ======
    mov ebx, 0          ; i = 0
while_loop:
    cmp ebx, 3          ; 比较 i 和 3
    jge while_end       ; 如果 i >= 3，结束循环
    
    ; 循环体
    mov rdi, fmt2
    mov esi, ebx
    xor eax, eax
    call printf
    
    inc ebx             ; i++
    jmp while_loop      ; 继续循环
    
while_end:
    
    pop rbp
    ret
```

## 第五部分：函数调用对比

### C语言函数
```c
#include <stdio.h>

// 函数声明
int add(int a, int b);
int factorial(int n);

int main() {
    // 调用add函数
    int result = add(5, 3);
    printf("5 + 3 = %d\n", result);
    
    // 调用递归函数
    int fact = factorial(5);
    printf("5! = %d\n", fact);
    
    return 0;
}

// 简单加法函数
int add(int a, int b) {
    return a + b;
}

// 递归计算阶乘
int factorial(int n) {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}
```

### 汇编函数实现
```nasm
section .data
    fmt_add db '%d + %d = %d', 0xa, 0
    fmt_fact db '%d! = %d', 0xa, 0

section .text
    global main
    extern printf

; ====== 加法函数 ======
; 参数：edi = a, esi = b
; 返回值：eax = a + b
add:
    mov eax, edi
    add eax, esi
    ret

; ====== 递归阶乘函数 ======
; 参数：edi = n
; 返回值：eax = n!
factorial:
    push rbp
    mov rbp, rsp
    
    cmp edi, 1          ; 比较 n 和 1
    jg .recursive       ; 如果 n > 1，递归
    
    ; 基本情况：n <= 1
    mov eax, 1          ; 返回 1
    jmp .end
    
.recursive:
    ; 保存 n 和返回地址
    push rdi
    
    ; 计算 factorial(n-1)
    dec edi             ; n = n-1
    call factorial      ; 递归调用
    
    ; 恢复 n 并计算 n * factorial(n-1)
    pop rdi             ; 恢复原始 n
    imul eax, edi       ; eax = n * factorial(n-1)
    
.end:
    pop rbp
    ret

; ====== 主函数 ======
main:
    push rbp
    mov rbp, rsp
    
    ; 调用 add 函数
    mov edi, 5          ; 第一个参数
    mov esi, 3          ; 第二个参数
    call add            ; 调用函数
    
    ; 打印结果
    mov r8d, eax        ; 保存返回值
    mov rdi, fmt_add
    mov esi, 5
    mov edx, 3
    mov ecx, r8d
    xor eax, eax
    call printf
    
    ; 调用 factorial 函数
    mov edi, 5
    call factorial
    
    ; 打印结果
    mov r8d, eax        ; 保存返回值
    mov rdi, fmt_fact
    mov esi, 5
    mov edx, r8d
    xor eax, eax
    call printf
    
    pop rbp
    ret
```

## 第六部分：内存操作与指针

### C语言指针示例
```c
#include <stdio.h>

int main() {
    int arr[5] = {10, 20, 30, 40, 50};
    int *ptr = arr;
    
    // 通过指针访问数组
    for (int i = 0; i < 5; i++) {
        printf("arr[%d] = %d\n", i, *(ptr + i));
    }
    
    // 修改数组元素
    ptr[2] = 100;
    printf("修改后: arr[2] = %d\n", arr[2]);
    
    return 0;
}
```

### 汇编内存操作
```nasm
section .data
    arr dd 10, 20, 30, 40, 50
    fmt db 'arr[%d] = %d', 0xa, 0
    fmt2 db '修改后: arr[2] = %d', 0xa, 0

section .bss
    ptr resd 1          ; 为指针保留空间

section .text
    global main
    extern printf

main:
    push rbp
    mov rbp, rsp
    
    ; 设置指针
    mov QWORD [ptr], arr
    
    ; 循环访问数组
    mov ebx, 0          ; i = 0
loop_start:
    cmp ebx, 5
    jge loop_end
    
    ; 计算 arr[i] 的地址
    mov rcx, [ptr]      ; 获取数组起始地址
    mov edx, ebx        ; i
    imul rdx, rdx, 4    ; 索引 * 4 (int大小)
    add rcx, rdx        ; arr + i*4
    
    ; 获取值并打印
    mov esi, ebx        ; 第一个参数：i
    mov edx, [rcx]      ; 第二个参数：arr[i]
    mov rdi, fmt
    xor eax, eax
    call printf
    
    inc ebx
    jmp loop_start
    
loop_end:
    
    ; 修改 arr[2]
    mov rcx, [ptr]      ; 数组起始地址
    add rcx, 8          ; arr + 2*4 = arr + 8
    mov DWORD [rcx], 100 ; arr[2] = 100
    
    ; 打印修改后的值
    mov esi, [rcx]
    mov rdi, fmt2
    xor eax, eax
    call printf
    
    pop rbp
    ret
```

## 第七部分：混合编程（C调用汇编）

### C主程序调用汇编函数
```c
// main.c
#include <stdio.h>

// 声明汇编函数
extern int asm_add(int a, int b);
extern void asm_strcpy(char *dest, const char *src);

int main() {
    // 调用汇编加法函数
    int sum = asm_add(10, 20);
    printf("10 + 20 = %d\n", sum);
    
    // 调用汇编字符串复制函数
    char src[] = "Hello from Assembly!";
    char dest[50];
    asm_strcpy(dest, src);
    printf("Copied string: %s\n", dest);
    
    return 0;
}
```

### 汇编函数实现
```nasm
; asm_functions.asm
section .text
    global asm_add
    global asm_strcpy

; 加法函数
; C原型: int asm_add(int a, int b)
asm_add:
    push rbp
    mov rbp, rsp
    
    mov eax, edi        ; 第一个参数在edi
    add eax, esi        ; 第二个参数在esi
    
    pop rbp
    ret

; 字符串复制函数
; C原型: void asm_strcpy(char *dest, const char *src)
asm_strcpy:
    push rbp
    mov rbp, rsp
    
    mov rsi, rdi        ; rdi = dest, rsi = src
    mov rdi, rsi
    
.copy_loop:
    mov al, [rsi]       ; 加载源字符
    mov [rdi], al       ; 存储到目标
    test al, al         ; 检查是否为0
    jz .copy_done       ; 如果是0，结束
    inc rsi             ; 源指针++
    inc rdi             ; 目标指针++
    jmp .copy_loop
    
.copy_done:
    pop rbp
    ret
```

### 编译链接
```bash
# 编译汇编文件
nasm -f elf64 asm_functions.asm -o asm_functions.o

# 编译C文件
gcc -c main.c -o main.o

# 链接并运行
gcc main.o asm_functions.o -o mixed_program
./mixed_program
```

## 第八部分：常用系统调用参考

| 系统调用 | 调用号 | 参数 | C语言对应 |
|---------|--------|------|----------|
| read | 0 | rdi=fd, rsi=buf, rdx=count | read() |
| write | 1 | rdi=fd, rsi=buf, rdx=count | write() |
| open | 2 | rdi=filename, rsi=flags, rdx=mode | open() |
| close | 3 | rdi=fd | close() |
| exit | 60 | rdi=exit_code | exit() |
| brk | 12 | rdi=address | brk() |

### 系统调用示例
```nasm
section .data
    filename db 'test.txt', 0
    content db 'Hello, File!', 0xa
    content_len equ $ - content

section .bss
    fd resd 1

section .text
    global _start

_start:
    ; 打开文件
    mov rax, 2          ; open系统调用
    mov rdi, filename
    mov rsi, 0102o      ; O_CREAT | O_WRONLY
    mov rdx, 0644o      ; 权限
    syscall
    
    mov [fd], eax       ; 保存文件描述符
    
    ; 写入文件
    mov rax, 1          ; write系统调用
    mov rdi, [fd]
    mov rsi, content
    mov rdx, content_len
    syscall
    
    ; 关闭文件
    mov rax, 3          ; close系统调用
    mov rdi, [fd]
    syscall
    
    ; 退出
    mov rax, 60
    mov rdi, 0
    syscall
```

## 第九部分：调试技巧

### 使用GDB调试汇编程序
```bash
# 编译时添加调试信息
nasm -f elf64 -g program.asm -o program.o
ld -g program.o -o program

# 启动GDB
gdb ./program

# 常用GDB命令
(gdb) break _start      # 在入口点设置断点
(gdb) run              # 运行程序
(gdb) info registers   # 查看寄存器
(gdb) x/10x $rsp       # 查看栈内容
(gdb) stepi           # 单步执行一条指令
(gdb) continue        # 继续执行
(gdb) quit            # 退出
```

### 调试示例
```nasm
section .data
    debug_msg db 'Debug point reached!', 0xa
    debug_len equ $ - debug_msg

section .text
    global _start

_start:
    ; 调试点1
    mov rax, 1
    mov rdi, 1
    mov rsi, debug_msg
    mov rdx, debug_len
    syscall            ; 输出调试信息
    
    ; ... 其他代码 ...
```

## 第十部分：优化技巧和最佳实践

1. **寄存器使用优先级**
   - 优先使用易失性寄存器（rax, rcx, rdx, rsi, rdi, r8-r11）
   - 非易失性寄存器（rbx, rbp, r12-r15）需要在保存后使用

2. **减少内存访问**
   ```nasm
   ; 不好的做法：频繁访问内存
   add eax, [var1]
   add eax, [var2]
   
   ; 好的做法：加载到寄存器
   mov ebx, [var1]
   mov ecx, [var2]
   add eax, ebx
   add eax, ecx
   ```

3. **对齐数据**
   ```nasm
   section .data
       align 16        ; 16字节对齐
       big_array dd 1000 DUP(?)
   ```

4. **使用适当的指令**
   - 小数据用`mov`，大数据用`rep movsb`
   - 乘除用`imul`/`idiv`，移位用`shl`/`shr`

## 总结练习：计算斐波那契数列

### C语言版本
```c
#include <stdio.h>

int fibonacci(int n) {
    if (n <= 1) return n;
    return fibonacci(n-1) + fibonacci(n-2);
}

int main() {
    for (int i = 0; i < 10; i++) {
        printf("fib(%d) = %d\n", i, fibonacci(i));
    }
    return 0;
}
```

### 汇编优化版本（迭代）
```nasm
section .data
    fmt db 'fib(%d) = %d', 0xa, 0

section .text
    global main
    extern printf

; 迭代计算斐波那契数
; 参数：edi = n
; 返回值：eax = fib(n)
fibonacci:
    cmp edi, 1
    jle .base_case
    
    mov eax, 0          ; fib(0)
    mov ebx, 1          ; fib(1)
    mov ecx, 1          ; 当前索引
    
.loop:
    cmp ecx, edi
    jge .done
    
    ; 计算下一个斐波那契数
    mov edx, eax
    add edx, ebx        ; fib(n) = fib(n-1) + fib(n-2)
    mov eax, ebx
    mov ebx, edx
    
    inc ecx
    jmp .loop
    
.base_case:
    mov eax, edi
    ret
    
.done:
    mov eax, ebx
    ret

main:
    push rbp
    mov rbp, rsp
    
    mov ebx, 0          ; i = 0
.print_loop:
    cmp ebx, 10
    jge .end
    
    ; 计算fib(i)
    mov edi, ebx
    call fibonacci
    
    ; 打印结果
    mov rdi, fmt
    mov esi, ebx
    mov edx, eax
    xor eax, eax
    call printf
    
    inc ebx
    jmp .print_loop
    
.end:
    pop rbp
    ret
```
