	.file	"header-months.c"
	.text
	.globl	month
	.type	month, @function
month:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	leaq	months(%rip), %rax
	movl	$3, %ecx
	movl	-4(%rbp), %edx
	subl	$1, %edx
	imull	%edx, %ecx
	movslq	%ecx, %rcx
	addq	%rcx, %rax
	jmp	.Lmonth.1
.Lmonth.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	month, .-month
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
	movq	print(%rip), %rax
	movq	%rax, -24(%rbp)
	movq	title(%rip), %rax
	movq	%rax, -32(%rbp)
	movq	out(%rip), %rax
	movq	(%rax), %rax
	movq	%rax, -40(%rbp)
	movq	-32(%rbp), %rdi
	movq	-40(%rbp), %rsi
	call	*-24(%rbp)
	cmpl	$0, %eax
	jge	.Lmain.2
	movl	$1, %eax
	jmp	.Lmain.1
.Lmain.2:
	movl	$1, -4(%rbp)
	.p2align 5
.Lmain.3:
	movl	-4(%rbp), %eax
	cmpl	$12, %eax
	jg	.Lmain.5
	movq	put(%rip), %rax
	movq	%rax, -48(%rbp)
	movl	$32, -52(%rbp)
	movl	-52(%rbp), %edi
	call	*-48(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -56(%rbp)
	movl	-56(%rbp), %edi
	call	month@PLT
	movq	%rax, -16(%rbp)
	movl	$0, -8(%rbp)
	.p2align 5
.Lmain.6:
	movl	-8(%rbp), %eax
	cmpl	$3, %eax
	jge	.Lmain.8
	movq	put(%rip), %rax
	movq	%rax, -64(%rbp)
	movq	-16(%rbp), %rax
	movslq	-8(%rbp), %rcx
	movsbl	(%rax,%rcx,1), %eax
	movl	%eax, -68(%rbp)
	movl	-68(%rbp), %edi
	call	*-64(%rbp)
.Lmain.7:
	addl	$1, -8(%rbp)
	jmp	.Lmain.6
.Lmain.8:
.Lmain.4:
	addl	$1, -4(%rbp)
	jmp	.Lmain.3
.Lmain.5:
	movq	print(%rip), %rax
	movq	%rax, -80(%rbp)
	leaq	.Ls0(%rip), %rax
	movq	%rax, -88(%rbp)
	movq	out(%rip), %rax
	movq	(%rax), %rax
	movq	%rax, -96(%rbp)
	movq	-88(%rbp), %rdi
	movq	-96(%rbp), %rsi
	call	*-80(%rbp)
	cmpl	$0, %eax
	jge	.Lmain.9
	movl	$2, %eax
	jmp	.Lmain.1
.Lmain.9:
	movq	out(%rip), %rax
	movq	(%rax), %rax
	movq	stdout@GOTPCREL(%rip), %rcx
	movq	(%rcx), %rcx
	cmpq	%rcx, %rax
	je	.Lmain.10
	movl	$3, %eax
	jmp	.Lmain.1
.Lmain.10:
	movq	put(%rip), %rax
	movq	putchar@GOTPCREL(%rip), %rcx
	cmpq	%rcx, %rax
	je	.Lmain.11
	movl	$4, %eax
	jmp	.Lmain.1
.Lmain.11:
	movl	$37, %eax
	jmp	.Lmain.1
.Lmain.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	main, .-main
	.data
.Ls0:
	.string	"\n"
.Ls1:
	.string	"months:"
	.data
	.globl	months
	.type	months, @object
	.size	months, 37
	.p2align 4
months:
	.string	"JanFebMarAprMayJunJulAugSepOctNovDec"
	.globl	title
	.type	title, @object
	.size	title, 8
	.p2align 3
title:
	.quad	.Ls1
	.globl	out
	.type	out, @object
	.size	out, 8
	.p2align 3
out:
	.quad	stdout
	.globl	put
	.type	put, @object
	.size	put, 8
	.p2align 3
put:
	.quad	putchar
	.globl	print
	.type	print, @object
	.size	print, 8
	.p2align 3
print:
	.quad	fputs
	.section	.note.GNU-stack,"",@progbits
