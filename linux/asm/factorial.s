# as -32 -g -o factorial.o factorial.s && ld -m elf_i386 -o factorial factorial.o && ./factorial

# 目的：给定一个数字，本程序将计算其阶乘
#

# 本程序展示了如何递归调用一个函数

# 注意：在调用函数时，需要将参数和返回地址压入栈中
# 注意：在函数调用完毕后，需要将栈指针恢复到调用函数之前的状态
.code32

.section .data # 本程序无全局数据

.equ SYS_exit, 60 # 赋值伪操作给表达式赋予一个名字

.section .text

.global _start
.global factorial

# calculates 4!
_start:
    pushl $4                 # 64位指令 push $4
    call factorial
    addl $4, %esp            # 64位指令 add $8, %rsp
    movl %eax, %edx          # 64位指令 mov %rax, %rdi
    movl $1, %eax            # 64位指令 movl $SYS_exit, %rax
    int $0x80                # syscall

# 实际的函数定义
# 注意：函数的返回值放在%eax中
.type factorial, @function
factorial:
    pushl %ebp                # 64位指令 push %rbp
    movl %esp, %ebp           # 64位指令 mov %rsp, %rbp

    movl 8(%ebp), %eax        # 64位指令 mov 16(%rbp), %rax
    cmpl $1, %eax             # 64位指令 cmp $1, %rax
    je end_factorial
    decl %eax                 # 64位指令 dec %rax
    pushl %eax                # 64位指令 push %rax
    call factorial
    movl 8(%ebp), %eax
    imull %ebx, %eax

end_factorial:
    mov %ebp, %esp
    pop %ebp
    ret
