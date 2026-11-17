	.file	"register-blocks.c"
	.text
	.globl	blocks
	.type	blocks, @function
blocks:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	pushq	%r12
	.cfi_offset %r12, -32
	pushq	%r13
	.cfi_offset %r13, -40
	pushq	%r14
	.cfi_offset %r14, -48
	movl	%edi, %ebx
	movl	$0, %r12d
	movl	%ebx, %r13d
	imull	$2, %r13d, %r13d
	addl	%r13d, %r12d
	movl	%ebx, %r13d
	imull	$3, %r13d, %r13d
	addl	%r13d, %r12d
	movl	%r12d, %r14d
	addl	$1, %r14d
	movl	%r14d, %eax
	jmp	.Lblocks.1
.Lblocks.1:
	popq	%r14
	popq	%r13
	popq	%r12
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	blocks, .-blocks
	.section	.note.GNU-stack,"",@progbits
