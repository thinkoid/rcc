	.file	"break-continue.c"
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
	movl	$0, -8(%rbp)
	movl	$0, -12(%rbp)
	.p2align 5
.Lsum.2:
	movl	-12(%rbp), %eax
	movl	-4(%rbp), %ecx
	cmpl	%ecx, %eax
	jge	.Lsum.4
	movl	-12(%rbp), %eax
	cmpl	$5, %eax
	jne	.Lsum.5
	jmp	.Lsum.3
.Lsum.5:
	movl	-8(%rbp), %eax
	cmpl	$100, %eax
	jle	.Lsum.6
	jmp	.Lsum.4
.Lsum.6:
	movl	-8(%rbp), %eax
	movl	-12(%rbp), %ecx
	addl	%ecx, %eax
	movl	%eax, -8(%rbp)
.Lsum.3:
	movl	-12(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -12(%rbp)
	jmp	.Lsum.2
.Lsum.4:
	.p2align 5
.Lsum.7:
	movl	$1, %eax
	cmpl	$0, %eax
	je	.Lsum.8
	movl	-8(%rbp), %eax
	subl	$1, %eax
	movl	%eax, -8(%rbp)
	movl	-8(%rbp), %eax
	cmpl	$50, %eax
	jge	.Lsum.9
	jmp	.Lsum.8
.Lsum.9:
	jmp	.Lsum.7
.Lsum.8:
	movl	-8(%rbp), %eax
	jmp	.Lsum.1
.Lsum.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	sum, .-sum
	.section	.note.GNU-stack,"",@progbits
