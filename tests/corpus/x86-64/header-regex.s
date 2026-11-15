	.file	"header-regex.c"
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
	movq	%rsi, -16(%rbp)
	movl	$0, -20(%rbp)
	movl	$0, -24(%rbp)
	.p2align 5
.Lsum.2:
	movl	-24(%rbp), %eax
	movl	-4(%rbp), %ecx
	cmpl	%ecx, %eax
	jge	.Lsum.4
	movq	-16(%rbp), %rax
	movl	-24(%rbp), %ecx
	movslq	%ecx, %rcx
	movl	(%rax,%rcx,4), %eax
	addl	%eax, -20(%rbp)
.Lsum.3:
	addl	$1, -24(%rbp)
	jmp	.Lsum.2
.Lsum.4:
	movl	-20(%rbp), %eax
	jmp	.Lsum.1
.Lsum.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	sum, .-sum
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movl	$1, -12(%rbp)
	movl	$2, -8(%rbp)
	movl	$3, -4(%rbp)
	movl	$3, -16(%rbp)
	leaq	-12(%rbp), %rax
	movq	%rax, -24(%rbp)
	movl	-16(%rbp), %edi
	movq	-24(%rbp), %rsi
	call	sum@PLT
	cmpl	$6, %eax
	je	.Lmain.2
	movl	$1, %eax
	jmp	.Lmain.1
.Lmain.2:
	movl	$0, %eax
	jmp	.Lmain.1
.Lmain.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	main, .-main
	.section	.note.GNU-stack,"",@progbits
