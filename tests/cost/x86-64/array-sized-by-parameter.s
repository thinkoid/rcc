	.file	"array-sized-by-parameter.c"
	.text
	.globl	g
	.type	g, @function
g:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movl	%edi, -4(%rbp)
	movslq	-4(%rbp), %rax
	imulq	$4, %rax, %rax
	movq	%rax, -16(%rbp)
	movq	%rsp, -24(%rbp)
	movq	-16(%rbp), %rax
	subq	%rax, %rsp
	andq	$-16, %rsp
	movq	%rsp, %rax
	movq	%rax, -32(%rbp)
	movl	$0, %eax
	jmp	.Lg.1
.Lg.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	g, .-g
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	$3, -4(%rbp)
	movl	-4(%rbp), %edi
	call	g@PLT
	jmp	.Lmain.1
.Lmain.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	main, .-main
	.section	.note.GNU-stack,"",@progbits
