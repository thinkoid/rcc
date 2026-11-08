	.file	"cgp.c"
	.text
	.globl	empty_if
	.type	empty_if, @function
empty_if:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	$0, %eax
	movl	%eax, -8(%rbp)
	movl	-4(%rbp), %eax
	cmpl	$0, %eax
	je	.Lempty_if.2
.Lempty_if.2:
	movl	-4(%rbp), %eax
	cmpl	$1, %eax
	jle	.Lempty_if.3
	movl	$1, %eax
	movl	%eax, -8(%rbp)
.Lempty_if.3:
	movl	-8(%rbp), %eax
	jmp	.Lempty_if.1
.Lempty_if.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	empty_if, .-empty_if
	.section	.note.GNU-stack,"",@progbits
