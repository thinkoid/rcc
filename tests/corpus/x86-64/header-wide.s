	.file	"header-wide.c"
	.text
	.type	length, @function
length:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	$0, -12(%rbp)
	.p2align 5
.Llength.2:
	movq	-8(%rbp), %rax
	movslq	-12(%rbp), %rcx
	cmpl	$0, (%rax,%rcx,4)
	je	.Llength.3
	addl	$1, -12(%rbp)
	jmp	.Llength.2
.Llength.3:
	movl	-12(%rbp), %eax
	jmp	.Llength.1
.Llength.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	length, .-length
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
	leaq	.Ls0(%rip), %rax
	movq	%rax, -8(%rbp)
	movl	$0, -12(%rbp)
	movl	$0, -16(%rbp)
	.p2align 5
.Lmain.2:
	leaq	word(%rip), %rax
	movslq	-16(%rbp), %rcx
	cmpl	$0, (%rax,%rcx,4)
	je	.Lmain.4
	leaq	word(%rip), %rax
	movslq	-16(%rbp), %rcx
	cmpl	$127, (%rax,%rcx,4)
	jle	.Lmain.5
	addl	$1, -12(%rbp)
.Lmain.5:
	addl	$1, -16(%rbp)
	jmp	.Lmain.2
.Lmain.4:
	leaq	.Ls1(%rip), %rax
	movq	%rax, -24(%rbp)
	leaq	word(%rip), %rax
	movq	%rax, -40(%rbp)
	movq	-40(%rbp), %rdi
	call	length@PLT
	movl	%eax, -28(%rbp)
	leaq	word(%rip), %rax
	movq	%rax, -56(%rbp)
	movq	-56(%rbp), %rdi
	call	wcslen@PLT
	movl	%eax, -44(%rbp)
	movl	-12(%rbp), %eax
	movl	%eax, -60(%rbp)
	movl	$52, -64(%rbp)
	movq	-24(%rbp), %rdi
	movl	-28(%rbp), %esi
	movl	-44(%rbp), %edx
	movl	-60(%rbp), %ecx
	movl	-64(%rbp), %r8d
	movl	$0, %eax
	call	printf@PLT
	leaq	word(%rip), %rax
	movq	%rax, -72(%rbp)
	movq	-72(%rbp), %rdi
	call	length@PLT
	cmpl	$12, %eax
	jne	.Lmain.7
	leaq	word(%rip), %rax
	movq	%rax, -80(%rbp)
	movq	-80(%rbp), %rdi
	call	wcslen@PLT
	cmpq	$12, %rax
	jne	.Lmain.7
	cmpl	$3, -12(%rbp)
	je	.Lmain.6
.Lmain.7:
	movl	$201, %eax
	jmp	.Lmain.1
.Lmain.6:
	cmpl	$232, word+8(%rip)
	jne	.Lmain.9
	cmpl	$233, word+40(%rip)
	jne	.Lmain.9
	movl	$0, %eax
	cmpl	$0, %eax
	je	.Lmain.8
.Lmain.9:
	movl	$202, %eax
	jmp	.Lmain.1
.Lmain.8:
	movl	$0, %eax
	cmpl	$0, %eax
	je	.Lmain.10
	movl	$203, %eax
	jmp	.Lmain.1
.Lmain.10:
	movl	$0, %eax
	cmpl	$0, %eax
	jne	.Lmain.12
	movq	-8(%rbp), %rax
	movq	%rax, -88(%rbp)
	movq	-88(%rbp), %rdi
	call	length@PLT
	cmpl	$3, %eax
	jne	.Lmain.12
	movq	-8(%rbp), %rax
	cmpl	$99, 8(%rax)
	je	.Lmain.11
.Lmain.12:
	movl	$204, %eax
	jmp	.Lmain.1
.Lmain.11:
	movl	$0, %eax
	cmpl	$0, %eax
	jne	.Lmain.14
	movzwl	smile+2(%rip), %eax
	cmpl	$55357, %eax
	jne	.Lmain.14
	movzwl	smile+4(%rip), %eax
	cmpl	$56832, %eax
	je	.Lmain.13
.Lmain.14:
	movl	$205, %eax
	jmp	.Lmain.1
.Lmain.13:
	movl	$0, %eax
	cmpl	$0, %eax
	je	.Lmain.15
	movl	$206, %eax
	jmp	.Lmain.1
.Lmain.15:
	leaq	word(%rip), %rax
	movq	%rax, -96(%rbp)
	movq	-96(%rbp), %rdi
	call	length@PLT
	imull	$10, %eax, %eax
	addl	-12(%rbp), %eax
	jmp	.Lmain.1
.Lmain.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	main, .-main
	.data
	.p2align 2
.Ls0:
	.long	97, 98, 99, 0
.Ls1:
	.string	"%d %d %d %d\n"
	.data
	.type	word, @object
	.size	word, 52
	.p2align 4
word:
	.long	99, 114, 232, 109, 101, 32, 98, 114, 251, 108, 233, 101, 0
	.type	smile, @object
	.size	smile, 8
	.p2align 1
smile:
	.short	233, 55357, 56832, 0
	.section	.note.GNU-stack,"",@progbits
