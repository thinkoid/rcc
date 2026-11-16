	.file	"static-inline.c"
	.text
	.type	max, @function
max:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	cmpl	-8(%rbp), %eax
	jle	.Lmax.2
	movl	-4(%rbp), %eax
	jmp	.Lmax.3
.Lmax.2:
	movl	-8(%rbp), %eax
.Lmax.3:
	jmp	.Lmax.1
.Lmax.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	max, .-max
	.globl	pick
	.type	pick, @function
pick:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -12(%rbp)
	movl	-8(%rbp), %eax
	movl	%eax, -16(%rbp)
	movl	-12(%rbp), %edi
	movl	-16(%rbp), %esi
	call	max@PLT
	jmp	.Lpick.1
.Lpick.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	pick, .-pick
	.section	.note.GNU-stack,"",@progbits
