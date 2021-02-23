main:
		pushl	%ebp
		movl	%esp, %ebp
		movl	$1, -4(%ebp)
		movl	$2, -8(%ebp)
		movl	$3, -12(%ebp)
		pushl	-4(%ebp)
		pushl	-8(%ebp)
		pushl	-12(%ebp)
		call	print
		addl	$12, %ebp
main.exit:
		movl	%ebp, %esp
		popl	%ebp
		ret
.globl main
