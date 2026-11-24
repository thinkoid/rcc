	.file	"bit-field-compound-volatile.c"
	.text
	.globl	add
	.type	add, @function
add:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	v(%rip), %eax
	movl	%eax, -16(%rbp)
	movq	-8(%rbp), %rax
	movsbl	(%rax), %eax
	movb	%al, -9(%rbp)
	movq	-8(%rbp), %rax
	movsbl	-9(%rbp), %ecx
	sarl	$3, %ecx
	addl	-16(%rbp), %ecx
	shll	$3, %ecx
	movsbl	-9(%rbp), %edx
	andl	$-249, %edx
	orl	%edx, %ecx
	movb	%cl, (%rax)
.Ladd.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	add, .-add
	.bss
	.globl	v
	.type	v, @object
	.size	v, 4
	.p2align 2
v:
	.zero	4
	.section	.note.GNU-stack,"",@progbits
