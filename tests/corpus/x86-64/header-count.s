	.file	"header-count.c"
	.text
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$96, %rsp
	movl	$0, %eax
	movl	%eax, -16(%rbp)
	movl	%eax, -12(%rbp)
	movl	$0, -8(%rbp)
	.p2align 5
.Lmain.2:
	cmpl	$10, -8(%rbp)
	jge	.Lmain.4
	movslq	-8(%rbp), %rax
	movl	$0, -56(%rbp,%rax,4)
.Lmain.3:
	addl	$1, -8(%rbp)
	jmp	.Lmain.2
.Lmain.4:
	movl	$0, -8(%rbp)
	.p2align 5
.Lmain.5:
	leaq	text(%rip), %rax
	movslq	-8(%rbp), %rcx
	movsbl	(%rax,%rcx,1), %eax
	movl	%eax, -4(%rbp)
	cmpl	$0, %eax
	je	.Lmain.7
	movl	-4(%rbp), %eax
	movl	$3, %ecx
	xorl	%edx, %edx
	divl	%ecx
	leaq	.Lmain.23(%rip), %rcx
	jmp	*(%rcx,%rdx,8)
	.data
	.p2align 3
.Lmain.23:
	.quad	.Lmain.24
	.quad	.Lmain.25
	.quad	.Lmain.26
	.text
.Lmain.24:
	cmpl	$3, %eax
	je	.Lmain.20
	cmpl	$16, %eax
	je	.Lmain.8
	cmpl	$17, %eax
	je	.Lmain.11
	cmpl	$18, %eax
	je	.Lmain.14
	cmpl	$19, %eax
	je	.Lmain.17
	jmp	.Lmain.21
.Lmain.25:
	cmpl	$3, %eax
	je	.Lmain.19
	cmpl	$16, %eax
	je	.Lmain.9
	cmpl	$17, %eax
	je	.Lmain.12
	cmpl	$18, %eax
	je	.Lmain.15
	jmp	.Lmain.21
.Lmain.26:
	cmpl	$10, %eax
	je	.Lmain.18
	cmpl	$16, %eax
	je	.Lmain.10
	cmpl	$17, %eax
	je	.Lmain.13
	cmpl	$18, %eax
	je	.Lmain.16
	jmp	.Lmain.21
.Lmain.8:
.Lmain.9:
.Lmain.10:
.Lmain.11:
.Lmain.12:
.Lmain.13:
.Lmain.14:
.Lmain.15:
.Lmain.16:
.Lmain.17:
	movl	-4(%rbp), %eax
	subl	$48, %eax
	movslq	%eax, %rax
	addl	$1, -56(%rbp,%rax,4)
	jmp	.Lmain.22
.Lmain.18:
.Lmain.19:
.Lmain.20:
	addl	$1, -12(%rbp)
	jmp	.Lmain.22
.Lmain.21:
	addl	$1, -16(%rbp)
	jmp	.Lmain.22
.Lmain.22:
.Lmain.6:
	addl	$1, -8(%rbp)
	jmp	.Lmain.5
.Lmain.7:
	leaq	.Ls0(%rip), %rax
	movq	%rax, -64(%rbp)
	movq	-64(%rbp), %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$0, -8(%rbp)
	.p2align 5
.Lmain.27:
	cmpl	$10, -8(%rbp)
	jge	.Lmain.29
	leaq	.Ls1(%rip), %rax
	movq	%rax, -72(%rbp)
	movslq	-8(%rbp), %rax
	movl	-56(%rbp,%rax,4), %eax
	movl	%eax, -76(%rbp)
	movq	-72(%rbp), %rdi
	movl	-76(%rbp), %esi
	movl	$0, %eax
	call	printf@PLT
.Lmain.28:
	addl	$1, -8(%rbp)
	jmp	.Lmain.27
.Lmain.29:
	leaq	.Ls2(%rip), %rax
	movq	%rax, -88(%rbp)
	movl	-12(%rbp), %eax
	movl	%eax, -92(%rbp)
	movl	-16(%rbp), %eax
	movl	%eax, -96(%rbp)
	movq	-88(%rbp), %rdi
	movl	-92(%rbp), %esi
	movl	-96(%rbp), %edx
	movl	$0, %eax
	call	printf@PLT
	cmpl	$2, -52(%rbp)
	jne	.Lmain.31
	cmpl	$5, -48(%rbp)
	jne	.Lmain.31
	cmpl	$2, -28(%rbp)
	je	.Lmain.30
.Lmain.31:
	movl	$101, %eax
	jmp	.Lmain.1
.Lmain.30:
	cmpl	$4, -24(%rbp)
	jne	.Lmain.33
	cmpl	$2, -20(%rbp)
	jne	.Lmain.33
	cmpl	$0, -56(%rbp)
	je	.Lmain.32
.Lmain.33:
	movl	$102, %eax
	jmp	.Lmain.1
.Lmain.32:
	cmpl	$13, -12(%rbp)
	je	.Lmain.34
	movl	$103, %eax
	jmp	.Lmain.1
.Lmain.34:
	movl	-16(%rbp), %eax
	jmp	.Lmain.1
.Lmain.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	main, .-main
	.data
.Ls0:
	.string	"digits ="
.Ls1:
	.string	" %d"
.Ls2:
	.string	", white space = %d, other = %d\n"
	.data
	.type	text, @object
	.size	text, 61
	.p2align 4
text:
	.string	"In 1978 K&R had 228 pages;\n\tthe 2nd edition, 1988, has 272.\n"
	.section	.note.GNU-stack,"",@progbits
