	.file	"large-frame.c"
	.text
	.globl	large
	.type	large, @function
large:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$8192, %rsp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	jmp	.Llarge.1
.Llarge.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	large, .-large
	.globl	aligned
	.type	aligned, @function
aligned:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$8176, %rsp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	jmp	.Laligned.1
.Laligned.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	aligned, .-aligned
	.section	.note.GNU-stack,"",@progbits
