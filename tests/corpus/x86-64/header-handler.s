	.file	"header-handler.c"
	.text
	.globl	handler
	.type	handler, @function
handler:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	cmpl	$10, -4(%rbp)
	jne	.Lhandler.2
	movl	$42, %eax
	jmp	.Lhandler.3
.Lhandler.2:
	movl	$1, %eax
.Lhandler.3:
	movl	%eax, -8(%rbp)
	movl	-8(%rbp), %edi
	call	_Exit@PLT
.Lhandler.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	handler, .-handler
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
	movl	$10, -4(%rbp)
	leaq	handler(%rip), %rax
	movq	%rax, -16(%rbp)
	movl	-4(%rbp), %edi
	movq	-16(%rbp), %rsi
	call	signal@PLT
	cmpq	$-1, %rax
	jne	.Lmain.2
	movl	$2, %eax
	jmp	.Lmain.1
.Lmain.2:
	movl	$10, -20(%rbp)
	movl	-20(%rbp), %edi
	call	raise@PLT
	movl	$3, %eax
	jmp	.Lmain.1
.Lmain.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	main, .-main
	.section	.note.GNU-stack,"",@progbits
