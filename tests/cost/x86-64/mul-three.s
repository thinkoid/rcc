	.file	"mul-three.c"
	.text
	.globl	mul_three
	.type	mul_three, @function
mul_three:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	imull	$3, %eax, %eax
	jmp	.Lmul_three.1
.Lmul_three.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	mul_three, .-mul_three
	.section	.note.GNU-stack,"",@progbits
