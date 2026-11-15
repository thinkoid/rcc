	.file	"header-signal.c"
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
	subq	$32, %rsp
	movl	$0, -4(%rbp)
	movl	-4(%rbp), %edi
	call	raise@PLT
	cmpl	$0, %eax
	je	.Lmain.2
	movl	$1, %eax
	jmp	.Lmain.1
.Lmain.2:
	movl	$10, -8(%rbp)
	movq	$1, -16(%rbp)
	movl	-8(%rbp), %edi
	movq	-16(%rbp), %rsi
	call	signal@PLT
	cmpq	$0, %rax
	je	.Lmain.3
	movl	$2, %eax
	jmp	.Lmain.1
.Lmain.3:
	movl	$10, -20(%rbp)
	movl	-20(%rbp), %edi
	call	raise@PLT
	cmpl	$0, %eax
	je	.Lmain.4
	movl	$3, %eax
	jmp	.Lmain.1
.Lmain.4:
	movl	$10, -24(%rbp)
	movq	$0, -32(%rbp)
	movl	-24(%rbp), %edi
	movq	-32(%rbp), %rsi
	call	signal@PLT
	cmpq	$1, %rax
	je	.Lmain.5
	movl	$4, %eax
	jmp	.Lmain.1
.Lmain.5:
	movl	$0, %eax
	jmp	.Lmain.1
.Lmain.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	main, .-main
	.section	.note.GNU-stack,"",@progbits
