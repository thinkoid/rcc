	.file	"div-four.c"
	.text
	.globl	div_four
	.type	div_four, @function
div_four:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	$4, %ecx
	cltd
	idivl	%ecx
	jmp	.Ldiv_four.1
.Ldiv_four.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	div_four, .-div_four
	.section	.note.GNU-stack,"",@progbits
