	.file	"udiv-urem.c"
	.text
	.globl	udiv
	.type	udiv, @function
udiv:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	movl	-8(%rbp), %ecx
	xorl	%edx, %edx
	divl	%ecx
	jmp	.Ludiv.1
.Ludiv.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	udiv, .-udiv
	.globl	urem
	.type	urem, @function
urem:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	movl	-8(%rbp), %ecx
	xorl	%edx, %edx
	divl	%ecx
	movl	%edx, %eax
	jmp	.Lurem.1
.Lurem.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	urem, .-urem
	.section	.note.GNU-stack,"",@progbits
