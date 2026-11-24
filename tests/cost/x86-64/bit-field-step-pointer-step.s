	.file	"bit-field-step-pointer-step.c"
	.text
	.globl	next
	.type	next, @function
next:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	cursor(%rip), %rax
	movq	%rax, %rcx
	addq	$8, %rax
	movq	%rax, cursor(%rip)
	addq	$4, %rcx
	movq	%rcx, -16(%rbp)
	movq	-16(%rbp), %rax
	movzbl	(%rax), %eax
	movb	%al, -1(%rbp)
	movq	-16(%rbp), %rax
	movzbl	-1(%rbp), %ecx
	shll	$28, %ecx
	shrl	$29, %ecx
	addl	$1, %ecx
	shll	$1, %ecx
	andl	$14, %ecx
	movzbl	-1(%rbp), %edx
	andl	$-15, %edx
	orl	%edx, %ecx
	movb	%cl, (%rax)
	movzbl	-1(%rbp), %eax
	shll	$28, %eax
	shrl	$29, %eax
	jmp	.Lnext.1
.Lnext.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	next, .-next
	.bss
	.globl	cursor
	.type	cursor, @object
	.size	cursor, 8
	.p2align 3
cursor:
	.zero	8
	.section	.note.GNU-stack,"",@progbits
