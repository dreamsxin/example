## Programming Ground Up 深入理解程序设计使用Linux汇编语言

- https://github.com/IammyselfYBX/GAS-Programming_from_the_Ground-Up
- https://github.com/foomur/programming-ground-up
- https://github.com/swseverance/programming-from-the-ground-up
- https://github.com/physcat/asm

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
