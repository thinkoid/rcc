	.file	"bool-postfix-side-effect.c"
	.text
	.globl	take
	.type	take, @function
take:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	cursor(%rip), %rax
	movq	%rax, %rcx
	addq	$1, %rax
	movq	%rax, cursor(%rip)
	movq	%rcx, -8(%rbp)
	movq	-8(%rbp), %rax
	movzbl	(%rax), %eax
	movb	%al, -9(%rbp)
	movq	-8(%rbp), %rax
	movzbl	-9(%rbp), %ecx
	addl	$1, %ecx
	cmpl	$0, %ecx
	setne	%cl
	movzbl	%cl, %ecx
	movb	%cl, (%rax)
	movzbl	-9(%rbp), %eax
	jmp	.Ltake.1
.Ltake.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	take, .-take
	.bss
	.globl	cursor
	.type	cursor, @object
	.size	cursor, 8
	.p2align 3
cursor:
	.zero	8
	.section	.note.GNU-stack,"",@progbits
