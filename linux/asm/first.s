# as exit.s -o exit.o && ld exit.o -o exit

# 目的：退出并向linux内核返回一个状态码
#

# 输入：无
#

# 输出：返回一个状态码，在运行程序后可通过输入 echo $? 来读取状态码
#

# 变量：
#    %eax 保存系统调用号
#    %ebx 保存状态码
.section .data # 数据段开始，任何以小数点符号开头的指令都由汇编程序处理，被称为汇编指令或伪操作

.section .text # 文本段开始，存放程序指令的部分，以下都属于文本段内容
.globl _start  # .globl 表示汇编之后不应该废弃此符号，_start是程序的入口点，是一个特殊符号，由链接器使用
_start:
    # 调用系统调用 exit，参数为 1
    # 系统调用号存放在 %eax 中，参数存放在 %ebx 中
    movl $1, %eax
    movl $0, %ebx
    int $0x80      # int 代表中断，控制权转交给系统，系统读取 %eax 中的系统调用号，并执行相应的操作
