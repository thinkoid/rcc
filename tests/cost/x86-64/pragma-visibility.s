	.file	"pragma-visibility.c"
	.text
	.globl	shown_value
	.type	shown_value, @function
shown_value:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	shown(%rip), %eax
	jmp	.Lshown_value.1
.Lshown_value.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	shown_value, .-shown_value
	.type	twice, @function
twice:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	addl	-4(%rbp), %eax
	jmp	.Ltwice.1
.Ltwice.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	twice, .-twice
	.globl	twice_shown
	.type	twice_shown, @function
twice_shown:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	shown(%rip), %eax
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %edi
	call	twice@PLT
	jmp	.Ltwice_shown.1
.Ltwice_shown.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	twice_shown, .-twice_shown
	.data
	.globl	shown
	.type	shown, @object
	.size	shown, 4
	.p2align 2
shown:
	.long	1
	.section	.note.GNU-stack,"",@progbits
