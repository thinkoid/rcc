	.file	"logical.c"
	.text
	.globl	both
	.type	both, @function
both:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	cmpl	$0, %eax
	je	.L2
	movl	-8(%rbp), %eax
	cmpl	$0, %eax
	je	.L2
	movl	$1, %eax
	jmp	.L3
.L2:
	movl	$0, %eax
.L3:
	jmp	.L1
.L1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	both, .-both
	.globl	either
	.type	either, @function
either:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	cmpl	$0, %eax
	jne	.L7
	movl	-8(%rbp), %eax
	cmpl	$0, %eax
	je	.L5
.L7:
	movl	$1, %eax
	jmp	.L6
.L5:
	movl	$0, %eax
.L6:
	jmp	.L4
.L4:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	either, .-either
	.section	.note.GNU-stack,"",@progbits
