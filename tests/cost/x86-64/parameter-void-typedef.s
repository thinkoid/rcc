	.file	"parameter-void-typedef.c"
	.text
	.globl	f
	.type	f, @function
f:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	$0, %eax
	jmp	.Lf.1
.Lf.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	f, .-f
	.section	.note.GNU-stack,"",@progbits
