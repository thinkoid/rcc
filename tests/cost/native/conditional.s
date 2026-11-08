	.file	"conditional.c"
	.text
	.globl	conditional
	.type	conditional, @function
conditional:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	%edx, -12(%rbp)
	movl	-4(%rbp), %eax
	cmpl	$0, %eax
	je	.L2
	movl	-8(%rbp), %eax
	jmp	.L3
.L2:
	movl	-12(%rbp), %eax
.L3:
	jmp	.L1
.L1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	conditional, .-conditional
	.section	.note.GNU-stack,"",@progbits
