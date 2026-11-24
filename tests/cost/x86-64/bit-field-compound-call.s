	.file	"bit-field-compound-call.c"
	.text
	.globl	raise
	.type	raise, @function
raise:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	call	current@PLT
	addq	$4, %rax
	movq	%rax, -16(%rbp)
	movq	-16(%rbp), %rax
	movzbl	(%rax), %eax
	movb	%al, -5(%rbp)
	movq	-16(%rbp), %rax
	movzbl	-5(%rbp), %ecx
	shll	$28, %ecx
	shrl	$29, %ecx
	addl	-4(%rbp), %ecx
	shll	$1, %ecx
	andl	$14, %ecx
	movzbl	-5(%rbp), %edx
	andl	$-15, %edx
	orl	%edx, %ecx
	movb	%cl, (%rax)
.Lraise.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	raise, .-raise
	.section	.note.GNU-stack,"",@progbits
