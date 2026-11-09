	.file	"add-zero.c"
	.text
	.globl	add_zero
	.type	add_zero, @function
add_zero:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	addl	$0, %eax
	jmp	.Ladd_zero.1
.Ladd_zero.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	add_zero, .-add_zero
	.section	.note.GNU-stack,"",@progbits
