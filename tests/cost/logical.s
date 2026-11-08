	.file	"logical.c"
	.text
	.globl	both
	.type	both, @function
both:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	cmpl	$0, %eax
	je	.Lboth.2
	movl	-8(%rbp), %eax
	cmpl	$0, %eax
	je	.Lboth.2
	movl	$1, %eax
	jmp	.Lboth.3
.Lboth.2:
	movl	$0, %eax
.Lboth.3:
	jmp	.Lboth.1
.Lboth.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	both, .-both
	.globl	either
	.type	either, @function
either:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	cmpl	$0, %eax
	jne	.Leither.4
	movl	-8(%rbp), %eax
	cmpl	$0, %eax
	je	.Leither.2
.Leither.4:
	movl	$1, %eax
	jmp	.Leither.3
.Leither.2:
	movl	$0, %eax
.Leither.3:
	jmp	.Leither.1
.Leither.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	either, .-either
	.section	.note.GNU-stack,"",@progbits
