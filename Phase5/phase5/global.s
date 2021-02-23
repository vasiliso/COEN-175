_foo:
		pushl	%ebp
		movl	%esp, %ebp
		movl	$1, _x
		movl	$2, _y
		movl	$3, _z
_foo.exit:
		movl	%ebp, %esp
		popl	%ebp
		ret
.globl _foo
.comm x, 4
.comm y, 4
.comm z, 4
