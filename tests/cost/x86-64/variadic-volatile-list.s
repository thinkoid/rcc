	.file	"variadic-volatile-list.c"
	.text
	.globl	first
	.type	first, @function
first:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$240, %rsp
	movl	%edi, -4(%rbp)
	movq	%rdi, -192(%rbp)
	movq	%rsi, -184(%rbp)
	movq	%rdx, -176(%rbp)
	movq	%rcx, -168(%rbp)
	movq	%r8, -160(%rbp)
	movq	%r9, -152(%rbp)
	movaps	%xmm0, -144(%rbp)
	movaps	%xmm1, -128(%rbp)
	movaps	%xmm2, -112(%rbp)
	movaps	%xmm3, -96(%rbp)
	movaps	%xmm4, -80(%rbp)
	movaps	%xmm5, -64(%rbp)
	movaps	%xmm6, -48(%rbp)
	movaps	%xmm7, -32(%rbp)
	leaq	16(%rbp), %rax
	movq	%rax, -16(%rbp)
	leaq	-224(%rbp), %rax
	movq	%rax, -232(%rbp)
	movl	$8, -224(%rbp)
	movl	$48, -220(%rbp)
	movq	-16(%rbp), %rax
	movq	%rax, -216(%rbp)
	leaq	-192(%rbp), %rax
	movq	%rax, -208(%rbp)
	movq	-232(%rbp), %rax
	movq	%rax, -200(%rbp)
	movq	-200(%rbp), %rax
	cmpl	$40, (%rax)
	ja	.Lfirst.2
	movq	-200(%rbp), %rax
	addl	$8, (%rax)
	movq	-200(%rbp), %rax
	movq	16(%rax), %rax
	movq	-200(%rbp), %rcx
	movl	(%rcx), %ecx
	subl	$8, %ecx
	addq	%rcx, %rax
	jmp	.Lfirst.3
.Lfirst.2:
	movq	-200(%rbp), %rax
	movq	-200(%rbp), %rcx
	movq	8(%rcx), %rcx
	addq	$8, %rcx
	movq	%rcx, 8(%rax)
	movq	-200(%rbp), %rax
	movq	8(%rax), %rax
	subq	$8, %rax
.Lfirst.3:
	movl	(%rax), %eax
	movl	%eax, -236(%rbp)
	leaq	-224(%rbp), %rax
	movl	-236(%rbp), %eax
	jmp	.Lfirst.1
.Lfirst.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	first, .-first
	.section	.note.GNU-stack,"",@progbits
