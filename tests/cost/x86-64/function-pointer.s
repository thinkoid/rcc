	.file	"function-pointer.c"
	.text
	.globl	swap
	.type	swap, @function
swap:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$48, %rsp
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -28(%rbp)
	movq	-16(%rbp), %rax
	movq	%rax, -40(%rbp)
	movl	-28(%rbp), %edi
	movq	-40(%rbp), %rsi
	call	install@PLT
	movq	%rax, -24(%rbp)
	movq	-24(%rbp), %rax
	movq	-16(%rbp), %rcx
	cmpq	%rcx, %rax
	je	.Lswap.3
	movq	-24(%rbp), %rax
	cmpq	$0, %rax
	jne	.Lswap.2
.Lswap.3:
	movq	$0, %rax
	jmp	.Lswap.1
.Lswap.2:
	movq	-24(%rbp), %rax
	jmp	.Lswap.1
.Lswap.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	swap, .-swap
	.section	.note.GNU-stack,"",@progbits
