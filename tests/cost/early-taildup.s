	.file	"early-taildup.c"
	.text
	.globl	join
	.type	join, @function
join:
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
	movl	-4(%rbp), %eax
	cmpl	$0, %eax
	je	.L4
	movl	-12(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -12(%rbp)
	jmp	.L5
.L4:
	movl	-12(%rbp), %eax
	addl	$2, %eax
	movl	%eax, -12(%rbp)
.L5:
	jmp	.L2
.L3:
	movl	-12(%rbp), %eax
	jmp	.L1
.L1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	join, .-join
	.section	.note.GNU-stack,"",@progbits
