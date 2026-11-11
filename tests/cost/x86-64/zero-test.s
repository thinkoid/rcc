	.file	"zero-test.c"
	.text
	.globl	never
	.type	never, @function
never:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	$0, %eax
	cmpl	$0, %eax
	je	.Lnever.2
	movl	$1, %eax
	jmp	.Lnever.1
.Lnever.2:
	movl	$2, %eax
	jmp	.Lnever.1
.Lnever.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	never, .-never
	.globl	not_zero
	.type	not_zero, @function
not_zero:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	$1, %eax
	jmp	.Lnot_zero.1
.Lnot_zero.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	not_zero, .-not_zero
	.section	.note.GNU-stack,"",@progbits
