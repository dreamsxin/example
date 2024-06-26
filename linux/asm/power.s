# 目的:	展示函数如何工作的程序
#		本程序将计算	2^3 + 5^2
#

# 主程序所有内容都存储在寄存器中，因此数据段不含任何内容

.section	.data

.section	.text

.globl	_start

_start:
	pushl	$3		#压入第二个参数，%esp-4
	pushl	$2		#压入第一个参数，%esp-4
	call	power           #压入第一个参数，%esp-4
	addl	$8, %esp	#恢复栈指针
	pushl	%eax		#在调用下一个函数前，保存第一个答案

	pushl	$2
	pushl	$5
	call	power       #jmp 区别，将返回地址入栈
	addl	$8, %esp
	
	popl	%ebx
	addl	%eax, %ebx

	movl	$1, %eax
	int		$0x80

#目的：	用于计算一个数的幂
#
#输入：	第一个参数	底数
#		第二个参数	底数的指数
#
#输出：	以返回值的形式给出结果
#
#注意：	直属必须大于等于1
#
#变量：
#		%ebx保存底数
#		%ecx保存指数
#			-4(%ebp)保存当前结果
#			%eax用于暂时存储

.type	power, @function
power:
	pushl	%ebp            #压入栈基，%esp-4
	movl	%esp, %ebp      #将此时的栈顶地址赋值给栈基
	subl	$4, %esp		#为本地存储保留空间
	movl	8(%ebp), %ebx	#第一个参数放入%eax
	movl	12(%ebp), %ecx	#第二个参数放入%ecx
	movl	%ebx, -4(%ebp)	#存储当前结果
power_loop_start:
	cmpl	$1, %ecx	#如果是1次方，就已经获得结果
	je		end_power
	movl	-4(%ebp), %eax	#将当前结果放入%eax
	imull	%ebx, %eax		#将当前结果与底数相乘
	movl	%eax, -4(%ebp)	#保存当前结果
	decl	%ecx
	jmp		power_loop_start
end_power:
	movl	-4(%ebp), %eax	#返回值移入%eax
	movl	%ebp,	%esp
	popl	%ebp
	ret
