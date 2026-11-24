	.file	"bit-field-statement-expression-value.c"
	.text
	.globl	store
	.type	store, @function
store:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rcx
	movl	-12(%rbp), %eax
	shll	$3, %eax
	movq	-8(%rbp), %rdx
	movsbl	(%rdx), %edx
	andl	$-249, %edx
	orl	%edx, %eax
	movb	%al, (%rcx)
	movsbl	%al, %eax
	sarl	$3, %eax
	jmp	.Lstore.1
.Lstore.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	store, .-store
	.section	.note.GNU-stack,"",@progbits
