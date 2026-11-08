	.file	"block-order.c"
	.text
	.globl	block_order
	.type	block_order, @function
block_order:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	$0, %eax
	movl	%eax, -12(%rbp)
.L2:
	movl	-12(%rbp), %eax
	movl	-8(%rbp), %ecx
	cmpl	%ecx, %eax
	jge	.L3
	movl	-12(%rbp), %eax
	movl	-4(%rbp), %ecx
	cmpl	%ecx, %eax
	jne	.L4
	movl	$1, %eax
	jmp	.L1
.L4:
	movl	-12(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -12(%rbp)
	jmp	.L2
.L3:
	movl	$0, %eax
	jmp	.L1
.L1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	block_order, .-block_order
	.section	.note.GNU-stack,"",@progbits
