	.file	"header-stdio.c"
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
	subq	$112, %rsp
	leaq	-32(%rbp), %rax
	movq	%rax, -48(%rbp)
	movq	$32, -56(%rbp)
	leaq	.Ls0(%rip), %rax
	movq	%rax, -64(%rbp)
	leaq	.Ls1(%rip), %rax
	movq	%rax, -72(%rbp)
	movl	$42, -76(%rbp)
	movq	-48(%rbp), %rdi
	movq	-56(%rbp), %rsi
	movq	-64(%rbp), %rdx
	movq	-72(%rbp), %rcx
	movl	-76(%rbp), %r8d
	movl	$0, %eax
	call	snprintf@PLT
	movl	%eax, -36(%rbp)
	movl	-36(%rbp), %eax
	cmpl	$15, %eax
	je	.Lmain.2
	movl	$1, %eax
	jmp	.Lmain.1
.Lmain.2:
	leaq	-32(%rbp), %rax
	movq	%rax, -88(%rbp)
	leaq	.Ls2(%rip), %rax
	movq	%rax, -96(%rbp)
	movq	-88(%rbp), %rdi
	movq	-96(%rbp), %rsi
	call	strcmp@PLT
	cmpl	$0, %eax
	je	.Lmain.3
	movl	$2, %eax
	jmp	.Lmain.1
.Lmain.3:
	leaq	.Ls3(%rip), %rax
	movq	%rax, -104(%rbp)
	leaq	-32(%rbp), %rax
	movq	%rax, -112(%rbp)
	movq	-104(%rbp), %rdi
	movq	-112(%rbp), %rsi
	movl	$0, %eax
	call	printf@PLT
	movl	$0, %eax
	jmp	.Lmain.1
.Lmain.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	main, .-main
	.data
.Ls0:
	.string	"hello, %s %d"
.Ls1:
	.string	"world"
.Ls2:
	.string	"hello, world 42"
.Ls3:
	.string	"%s\n"
	.section	.note.GNU-stack,"",@progbits
