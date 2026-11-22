	.file	"bit-field-volatile.c"
	.text
	.globl	status
	.type	status, @function
status:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movzbl	(%rax), %eax
	shll	$31, %eax
	shrl	$31, %eax
	movq	-8(%rbp), %rcx
	movzbl	1(%rcx), %ecx
	addl	%ecx, %eax
	jmp	.Lstatus.1
.Lstatus.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	status, .-status
	.globl	arm
	.type	arm, @function
arm:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	orb	$1, (%rax)
.Larm.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	arm, .-arm
	.globl	reset
	.type	reset, @function
reset:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rax
	movb	$0, 1(%rax)
	movq	-8(%rbp), %rax
	movl	-12(%rbp), %ecx
	shll	$1, %ecx
	andl	$62, %ecx
	movq	-8(%rbp), %rdx
	movzbl	(%rdx), %edx
	andl	$-63, %edx
	orl	%edx, %ecx
	movb	%cl, (%rax)
.Lreset.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	reset, .-reset
	.section	.note.GNU-stack,"",@progbits
