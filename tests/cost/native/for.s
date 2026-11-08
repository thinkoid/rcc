	.file	"for.c"
	.text
	.globl	sum
	.type	sum, @function
sum:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	$0, %eax
	movl	%eax, -8(%rbp)
	movl	$0, %eax
	movl	%eax, -12(%rbp)
.L2:
	movl	-12(%rbp), %eax
	movl	-4(%rbp), %ecx
	cmpl	%ecx, %eax
	jge	.L4
	movl	-8(%rbp), %eax
	movl	-12(%rbp), %ecx
	addl	%ecx, %eax
	movl	%eax, -8(%rbp)
.L3:
	movl	-12(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -12(%rbp)
	jmp	.L2
.L4:
	movl	-8(%rbp), %eax
	jmp	.L1
.L1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	sum, .-sum
	.section	.note.GNU-stack,"",@progbits
