# as -g -o maximum.o maximum.s && ld -o maximum maximum.o && ./maximum

.section .data

data_items:
    .quad 3, 67, 34, 222, 45, 75, 54, 34, 44, 33, 22, 11, 66, 0

.equ SYS_exit, 60

.section .text

.global _start

# rdi - index
# rbx - maximum
# rax - current item
_start:
    mov $0, %rdi
    mov data_items(,%rdi,8), %rax   # mov first item to rax
    mov %rax, %rbx

start_loop:
    cmp $0, %rax    # check to see if we've hit the end of the list
    je loop_exit
    inc %rdi
    mov data_items(,%rdi,8), %rax
    cmp %rbx, %rax  # compare current item with maximum so far
    jle start_loop
    mov %rax, %rbx
    jmp start_loop

loop_exit:
    mov $SYS_exit, %rax
    mov %rbx, %rdi  # so that largest item can be viewed via `echo $?`
    syscall
