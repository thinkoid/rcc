	.file	"typedef.c"
	.text
	.globl	length
	.type	length, @function
length:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	$0, -16(%rbp)
	.p2align 5
.Llength.2:
	movq	-8(%rbp), %rax
	movq	-16(%rbp), %rcx
	movsbl	(%rax,%rcx,1), %eax
	cmpb	$0, %al
	je	.Llength.3
	addq	$1, -16(%rbp)
	jmp	.Llength.2
.Llength.3:
	movq	-16(%rbp), %rax
	jmp	.Llength.1
.Llength.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	length, .-length
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
	leaq	-20(%rbp), %rax
	movq	%rax, -32(%rbp)
	leaq	-20(%rbp), %rax
	movl	-4(%rbp), %ecx
	movl	%ecx, (%rax)
	leaq	-20(%rbp), %rax
	movl	$8, 4(%rax)
	leaq	-20(%rbp), %rax
	movl	(%rax), %eax
	leaq	-20(%rbp), %rcx
	movl	4(%rcx), %ecx
	addl	%ecx, %eax
	movq	-32(%rbp), %rcx
	movsbl	(%rcx), %ecx
	movsbq	%cl, %rcx
	addl	%ecx, %eax
	jmp	.Lsum.1
.Lsum.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	sum, .-sum
	.section	.note.GNU-stack,"",@progbits
