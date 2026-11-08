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
.Ljoin.2:
	movl	-12(%rbp), %eax
	movl	-8(%rbp), %ecx
	cmpl	%ecx, %eax
	jge	.Ljoin.3
	movl	-4(%rbp), %eax
	cmpl	$0, %eax
	je	.Ljoin.4
	movl	-12(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -12(%rbp)
	jmp	.Ljoin.5
.Ljoin.4:
	movl	-12(%rbp), %eax
	addl	$2, %eax
	movl	%eax, -12(%rbp)
.Ljoin.5:
	jmp	.Ljoin.2
.Ljoin.3:
	movl	-12(%rbp), %eax
	jmp	.Ljoin.1
.Ljoin.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	join, .-join
	.section	.note.GNU-stack,"",@progbits
