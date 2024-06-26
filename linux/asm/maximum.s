# as -g -o maximum.o maximum.s && ld -o maximum maximum.o && ./maximum

# 目的：本程序寻找一组数据项中的最大值
#
# 变量：寄存器有一下用途
#
# %edi - 保存正在检测的数据项索引
# %ebx - 当前已经找到的最大数据项
# %eax - 当前数据项数
#
# 使用一下内存位置
#
# data_items - 包含数据项的数组
#              0 表示数据结束

.section .data

data_items: 
    .long 3,67,34,222,45,75,54,34,44,33,22,11,66,0

.section .text

.globl _start
_start:
    movl $0, %edi # 数据项索引
    movl data_items(,%edi,4), %eax # 加载数据
    movl %eax, %ebx # 当前最大数据项

start_loop:
    cmpl $0, %eax # 检查是否已经到达数据项数组的末尾
    je loop_exit # 如果是，程序结束
    incl %edi # 下一个数据项
    movl data_items(,%edi,4), %eax # 加载数据
    cmpl %ebx, %eax # 比较当前最大数据项和新的数据项
    jle start_loop # 新的数据项小于等于当前最大数据项，继续循环
    movl %eax, %ebx # 新的数据项大于当前最大数据项，更新当前最大数据项
    jmp start_loop # 继续循环

loop_exit:
    movl $1, %eax # exit系统调用号
    int $0x80 # 调用系统调用
