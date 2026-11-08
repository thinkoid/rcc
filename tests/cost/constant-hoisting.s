	.file	"constant-hoisting.c"
	.text
	.globl	two_constants
	.type	two_constants, @function
two_constants:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rax
	movabsq	$81985529216486895, %rcx
	addq	%rcx, %rax
	movq	-16(%rbp), %rcx
	addq	%rcx, %rax
	movabsq	$81985529216486903, %rcx
	addq	%rcx, %rax
	jmp	.L1
.L1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	two_constants, .-two_constants
	.section	.note.GNU-stack,"",@progbits
