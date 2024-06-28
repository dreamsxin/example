#PURPOSE: 		This program will calculate x^2
# 				
# 				
#
#			Everything in the main program is stored in registers,
#			so the data section doesn’t have anything.
.code32
.section .data
.section .text
.globl _start
_start:
	pushl $8 		# the number you want to square, x=?, push second argument
	pushl $2 		# push first argument
	call square 		# call the function
	pushl %eax 		# save the first answer before
	popl %ebx
	movl $1, %eax		#exit(%ebx is returned)
int $0x80
	
#PURPOSE: 		This function is used to compute
# 			the value of a square, that is x^

#INPUT: 		First argument - the base number
# 			Second argument - the power to raise it to
# 				
#
#OUTPUT: 		Will give the result as a return value
#
#NOTES: 		The power must be 1 or greater
#
#VARIABLES:
# 				%ebx - holds the base number
# 				%ecx - holds the power
#
# 				-4(%ebp) - holds the current result
#
# 				%eax is used for temporary storage
#

.type square, @function
square:
	pushl %ebp 			#save old base pointer
	movl %esp, %ebp 		#make stack pointer the base pointer
	subl $4, %esp 			#get room for our local storage
	movl 8(%ebp), %ebx 		#put first argument in %eax
	movl 12(%ebp), %ecx 		#put second argument in %ecx
	movl %ebx, -4(%ebp) 		#store current result
	
square_loop_start:
	
	cmpl $1, %ecx 			#if the power is 1, we are done
	je end_square
	movl -4(%ebp), %eax 		#move the current result into %eax
	imull %ebx, %eax 		#multiply the current result by
					#the base number
	movl %eax, -4(%ebp) 		#store the current result
	decl %ecx 			#decrease the power
	jmp square_loop_start 		#run for the next power
	
end_square:
	movl -4(%ebp), %eax 		#return value goes in %eax
	movl %ebp, %esp 		#restore the stack pointer
	popl %ebp 			#restore the base pointer
ret



#  To run, from the command line
#  			as --32 square.s -o square.o
#			ld -melf_i386 square.o -o square
#			./square
#			echo $?
