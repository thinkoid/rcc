	.file	"g21-bool-byte.c"
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
	leaq	b(%rip), %rax
	movq	%rax, -16(%rbp)
	movl	$0, -20(%rbp)
	movq	-16(%rbp), %rax
	movb	$2, (%rax)
	movzbl	b(%rip), %eax
	imull	$16, %eax, %eax
	addl	%eax, -20(%rbp)
	cmpb	$0, b(%rip)
	je	.Lmain.2
	addl	$4, -20(%rbp)
.Lmain.2:
	movzbl	b(%rip), %eax
	cmpl	$1, %eax
	je	.Lmain.3
	addl	$2, -20(%rbp)
.Lmain.3:
	movzbl	b(%rip), %eax
	movb	%al, -1(%rbp)
	movzbl	-1(%rbp), %eax
	subl	$1, %eax
	cmpl	$0, %eax
	setne	%al
	movzbl	%al, %eax
	movb	%al, b(%rip)
	movzbl	-1(%rbp), %eax
	cmpl	$2, %eax
	je	.Lmain.4
	movl	$101, %eax
	jmp	.Lmain.1
.Lmain.4:
	movq	-16(%rbp), %rax
	movzbl	(%rax), %eax
	addl	%eax, -20(%rbp)
	movl	-20(%rbp), %eax
	jmp	.Lmain.1
.Lmain.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	main, .-main
	.bss
	.globl	b
	.type	b, @object
	.size	b, 1
b:
	.zero	1
	.section	.note.GNU-stack,"",@progbits
