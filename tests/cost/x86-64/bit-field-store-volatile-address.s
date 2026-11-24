	.file	"bit-field-store-volatile-address.c"
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
	movl	%edi, -4(%rbp)
	movq	d(%rip), %rax
	movq	%rax, -16(%rbp)
	movq	-16(%rbp), %rax
	movl	-4(%rbp), %ecx
	shll	$3, %ecx
	movq	-16(%rbp), %rdx
	movsbl	(%rdx), %edx
	andl	$-249, %edx
	orl	%edx, %ecx
	movb	%cl, (%rax)
.Lstore.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	store, .-store
	.bss
	.globl	d
	.type	d, @object
	.size	d, 8
	.p2align 3
d:
	.zero	8
	.section	.note.GNU-stack,"",@progbits
