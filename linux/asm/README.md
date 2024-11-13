## Programming Ground Up 深入理解程序设计使用Linux汇编语言

- https://github.com/IammyselfYBX/GAS-Programming_from_the_Ground-Up
- https://github.com/foomur/programming-ground-up
- https://github.com/swseverance/programming-from-the-ground-up
- https://github.com/physcat/asm

## 寻址方式

### 指令寻址
- 顺序寻址
可通过程序计数器（PC）加1，自动形成下一条指令的地址。
- 跳跃寻址
通过 转移类指令 实现。跳跃寻址是指下一条指令的地址码不是由 PC 给出的，而是本条指令给出的。

### 数据寻址

- 立即寻址
`#` 表示立即寻址特征，指令的地址字段指出的不是操作数的地址，而是操作数本身，称为 立即寻址，又称为 立即数寻址。 数据采用补码形式存放。
- 直接寻址
指令格式的地址字段中直接指出操作数在内存中的地址，就称为直接寻址。
- 间接寻址
相对于直接寻址而言的，是指指令的地址字段给出的形式地址不是操作数的真正地址，而是操作数地址所在的存储单元地址；也就是 “地址的地址”
- 寄存器寻址
指令在执行时所需的操作数来自寄存器，运算结果也写回寄存器；地址码字段直接指出了寄存器的编号。
- 寄存器间接寻址
如果寄存器中不是直接给出操作数，而是操作数的内存地址，那么就称为寄存器间接寻址。
- 隐含寻址
是指指令字中不直接给出操作数的地址，而是隐含在某个寄存器中（通过操作码表示）。
- 基址寻址
系统设定，基址寻址是一种偏移寻址的方式，需要设有基址寄存器 BR ；将基址寄存器 BR 的内容（基地址）加上指令中的形式地址，就可以形成操作数的有效地址。
- 变址寻址
用户设定，和基址寻址极为相似，需要设有变址寄存器 IX；将变址寄存器 IX 的内容加上指令中的形式地址，就可以形成操作数的有效地址。

## .section
该指令将程序分成几个不同部分。

### .section .data

数据段，列出程序所需的内存空间。

###  .section .text

文本段，存储程序的指令。


## 64-bit和32-bit的差别

1. pushl和popl适用在32bit
2. push和pop适用在642bit，寄存器只能使用 r 开头
   
### invalid instruction suffix for push

`as -32 power.s -g -o power.o && ld -m elf_i386 power.o -o power`

如果使用pushl和popl， 会出现 “invalid instruction suffix for push”
这是因为pushl和popl都是适用在32bit系统上的，因为64-bit 的系统，不接受32-bit的数值，所以用pushl 将立即数(immediate)入栈，会引发错误。32bit只接受16-bit和32-bit，64bit只接受16-bit和64-bit。
解决方法可以是：`as -32 power.s -g -o power.o` 这样，加上–32选项，表示强制按照32-bit mode来编译。但是，ld环境必须是32-bit mode的，不然会出现 ld: i386 architecture of input filepower.o’ is incompatible with i386:x86-64 output` 错误。
解决上述错误的方法： `ld -m elf_i386 power.o -o power`

如果使用push和pop，会出现错误operand type mismatch for 'push' 在语句 push %eax中。
这也是64-bit和32-bit的差别。在64-bit mode下面，push和栈进行交互的时候，不能使用%eax，而要使用%rax.其他命令亦如此。

如果想要在64bit下完整的使用32bit mode。那么按照下面三个方法改善：

- Add .code32 to the top of assembly code
- Assemble with the -32 flag
- Link with the -m elf_i386 flag
