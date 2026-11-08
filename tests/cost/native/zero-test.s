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
	je	.L2
	movl	$1, %eax
	jmp	.L1
.L2:
	movl	$2, %eax
	jmp	.L1
.L1:
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
	movl	$0, %eax
	cmpl	$0, %eax
	sete	%al
	movzbl	%al, %eax
	jmp	.L3
.L3:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	not_zero, .-not_zero
	.section	.note.GNU-stack,"",@progbits
