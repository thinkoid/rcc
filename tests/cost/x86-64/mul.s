	.file	"mul.c"
	.text
	.globl	mul
	.type	mul, @function
mul:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	movl	-8(%rbp), %ecx
	imull	%ecx, %eax
	jmp	.Lmul.1
.Lmul.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	mul, .-mul
	.section	.note.GNU-stack,"",@progbits
