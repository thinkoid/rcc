	.file	"header-getch.c"
	.text
	.type	read_char, @function
read_char:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	input(%rip), %rax
	movsbl	(%rax), %eax
	cmpl	$0, %eax
	je	.Lread_char.2
	movq	input(%rip), %rax
	movq	%rax, %rcx
	addq	$1, %rax
	movq	%rax, input(%rip)
	movsbl	(%rcx), %eax
	jmp	.Lread_char.3
.Lread_char.2:
	movl	$-1, %eax
.Lread_char.3:
	jmp	.Lread_char.1
.Lread_char.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	read_char, .-read_char
	.globl	getch
	.type	getch, @function
getch:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	cmpl	$0, bufp(%rip)
	jle	.Lgetch.2
	leaq	buf(%rip), %rax
	movl	bufp(%rip), %ecx
	subl	$1, %ecx
	movl	%ecx, bufp(%rip)
	movslq	%ecx, %rcx
	movsbl	(%rax,%rcx,1), %eax
	jmp	.Lgetch.3
.Lgetch.2:
	call	read_char@PLT
.Lgetch.3:
	jmp	.Lgetch.1
.Lgetch.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	getch, .-getch
	.globl	ungetch
	.type	ungetch, @function
ungetch:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	cmpl	$100, bufp(%rip)
	jl	.Lungetch.2
	leaq	.Ls0(%rip), %rax
	movq	%rax, -16(%rbp)
	movq	-16(%rbp), %rdi
	movl	$0, %eax
	call	printf@PLT
	jmp	.Lungetch.3
.Lungetch.2:
	leaq	buf(%rip), %rax
	movl	bufp(%rip), %ecx
	movl	%ecx, %edx
	addl	$1, %ecx
	movl	%ecx, bufp(%rip)
	movslq	%edx, %rdx
	movl	-4(%rbp), %ecx
	movb	%cl, (%rax,%rdx,1)
.Lungetch.3:
.Lungetch.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	ungetch, .-ungetch
	.globl	getint
	.type	getint, @function
getint:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	.p2align 5
.Lgetint.2:
	call	getch@PLT
	movl	%eax, -12(%rbp)
	cmpl	$32, %eax
	je	.Lgetint.4
	cmpl	$9, -12(%rbp)
	je	.Lgetint.4
	cmpl	$10, -12(%rbp)
	jne	.Lgetint.3
.Lgetint.4:
	jmp	.Lgetint.2
.Lgetint.3:
	cmpl	$-1, -12(%rbp)
	jne	.Lgetint.5
	movl	$0, %eax
	jmp	.Lgetint.1
.Lgetint.5:
	cmpl	$45, -12(%rbp)
	jne	.Lgetint.6
	movl	$-1, %eax
	jmp	.Lgetint.7
.Lgetint.6:
	movl	$1, %eax
.Lgetint.7:
	movl	%eax, -16(%rbp)
	cmpl	$43, -12(%rbp)
	je	.Lgetint.9
	cmpl	$45, -12(%rbp)
	jne	.Lgetint.8
.Lgetint.9:
	call	getch@PLT
	movl	%eax, -12(%rbp)
.Lgetint.8:
	movq	-8(%rbp), %rax
	movl	$0, (%rax)
	.p2align 5
.Lgetint.10:
	cmpl	$48, -12(%rbp)
	jl	.Lgetint.12
	cmpl	$57, -12(%rbp)
	jg	.Lgetint.12
	movq	-8(%rbp), %rax
	movq	-8(%rbp), %rcx
	imull	$10, (%rcx), %ecx
	movl	-12(%rbp), %edx
	subl	$48, %edx
	addl	%edx, %ecx
	movl	%ecx, (%rax)
	call	getch@PLT
	movl	%eax, -12(%rbp)
	jmp	.Lgetint.10
.Lgetint.12:
	movq	-8(%rbp), %rax
	movl	-16(%rbp), %ecx
	imull	(%rax), %ecx
	movl	%ecx, (%rax)
	cmpl	$-1, -12(%rbp)
	je	.Lgetint.13
	movl	-12(%rbp), %eax
	movl	%eax, -20(%rbp)
	movl	-20(%rbp), %edi
	call	ungetch@PLT
.Lgetint.13:
	movl	-12(%rbp), %eax
	jmp	.Lgetint.1
.Lgetint.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	getint, .-getint
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$64, %rsp
	movl	$0, -8(%rbp)
	movl	$0, -12(%rbp)
	.p2align 5
.Lmain.2:
	leaq	-4(%rbp), %rax
	movq	%rax, -24(%rbp)
	movq	-24(%rbp), %rdi
	call	getint@PLT
	cmpl	$0, %eax
	je	.Lmain.3
	movl	-4(%rbp), %eax
	addl	%eax, -8(%rbp)
	addl	$1, -12(%rbp)
	jmp	.Lmain.2
.Lmain.3:
	movl	$97, -28(%rbp)
	movl	-28(%rbp), %edi
	call	ungetch@PLT
	movl	$98, -32(%rbp)
	movl	-32(%rbp), %edi
	call	ungetch@PLT
	movl	$99, -36(%rbp)
	movl	-36(%rbp), %edi
	call	ungetch@PLT
	call	getch@PLT
	cmpl	$99, %eax
	jne	.Lmain.5
	call	getch@PLT
	cmpl	$98, %eax
	jne	.Lmain.5
	call	getch@PLT
	cmpl	$97, %eax
	je	.Lmain.4
.Lmain.5:
	movl	$1, %eax
	jmp	.Lmain.1
.Lmain.4:
	call	getch@PLT
	cmpl	$-1, %eax
	je	.Lmain.6
	movl	$2, %eax
	jmp	.Lmain.1
.Lmain.6:
	leaq	.Ls1(%rip), %rax
	movq	%rax, -48(%rbp)
	movl	-12(%rbp), %eax
	movl	%eax, -52(%rbp)
	movl	-8(%rbp), %eax
	movl	%eax, -56(%rbp)
	movq	-48(%rbp), %rdi
	movl	-52(%rbp), %esi
	movl	-56(%rbp), %edx
	movl	$0, %eax
	call	printf@PLT
	movl	-8(%rbp), %eax
	jmp	.Lmain.1
.Lmain.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	main, .-main
	.data
.Ls0:
	.string	"ungetch: too many characters\n"
.Ls1:
	.string	"%d integers, sum %d\n"
.Ls2:
	.string	" 12 -7  +30\t10\n"
	.data
	.type	bufp, @object
	.size	bufp, 4
	.p2align 2
bufp:
	.long	0
	.type	input, @object
	.size	input, 8
	.p2align 3
input:
	.quad	.Ls2
	.bss
	.type	buf, @object
	.size	buf, 100
	.p2align 4
buf:
	.zero	100
	.section	.note.GNU-stack,"",@progbits
