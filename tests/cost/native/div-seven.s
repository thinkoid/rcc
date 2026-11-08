	.file	"div-seven.c"
	.text
	.globl	div_seven
	.type	div_seven, @function
div_seven:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	$7, %ecx
	cltd
	idivl	%ecx
	jmp	.L1
.L1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	div_seven, .-div_seven
	.section	.note.GNU-stack,"",@progbits
