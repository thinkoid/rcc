	.file	"void-return.c"
	.text
	.globl	tail
	.type	tail, @function
tail:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	call	g@PLT
	jmp	.Ltail.1
.Ltail.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	tail, .-tail
	.globl	pick
	.type	pick, @function
pick:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	cmpl	$0, -4(%rbp)
	je	.Lpick.2
	movl	-4(%rbp), %eax
	movl	%eax, -8(%rbp)
	movl	-8(%rbp), %edi
	call	h@PLT
	jmp	.Lpick.1
.Lpick.2:
	call	g@PLT
.Lpick.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	pick, .-pick
	.section	.note.GNU-stack,"",@progbits
