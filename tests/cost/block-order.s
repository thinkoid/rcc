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
.Lblock_order.2:
	movl	-12(%rbp), %eax
	movl	-8(%rbp), %ecx
	cmpl	%ecx, %eax
	jge	.Lblock_order.3
	movl	-12(%rbp), %eax
	movl	-4(%rbp), %ecx
	cmpl	%ecx, %eax
	jne	.Lblock_order.4
	movl	$1, %eax
	jmp	.Lblock_order.1
.Lblock_order.4:
	movl	-12(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -12(%rbp)
	jmp	.Lblock_order.2
.Lblock_order.3:
	movl	$0, %eax
	jmp	.Lblock_order.1
.Lblock_order.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	block_order, .-block_order
	.section	.note.GNU-stack,"",@progbits
