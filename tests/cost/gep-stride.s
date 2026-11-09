	.file	"gep-stride.c"
	.text
	.globl	row
	.type	row, @function
row:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	leaq	-28(%rbp), %rax
	movl	-4(%rbp), %ecx
	movslq	%ecx, %rcx
	imulq	$12, %rcx, %rcx
	addq	%rcx, %rax
	movq	%rax, -40(%rbp)
	movq	-40(%rbp), %rax
	jmp	.Lrow.1
.Lrow.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	row, .-row
	.section	.note.GNU-stack,"",@progbits
