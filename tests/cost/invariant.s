	.file	"invariant.c"
	.text
	.globl	invariant
	.type	invariant, @function
invariant:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	%edx, -12(%rbp)
	movl	$0, %eax
	movl	%eax, -16(%rbp)
	movl	$0, %eax
	movl	%eax, -20(%rbp)
.L2:
	movl	-20(%rbp), %eax
	movl	-12(%rbp), %ecx
	cmpl	%ecx, %eax
	jge	.L4
	movl	-16(%rbp), %eax
	movl	-4(%rbp), %ecx
	movl	-8(%rbp), %edx
	movq	%rdx, %rsi
	movq	%rax, %rdi
	movl	%ecx, %eax
	cltd
	idivl	%esi
	movl	%eax, %ecx
	addl	%ecx, %edi
	movl	%edi, -16(%rbp)
.L3:
	movl	-20(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -20(%rbp)
	jmp	.L2
.L4:
	movl	-16(%rbp), %eax
	jmp	.L1
.L1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	invariant, .-invariant
	.globl	invariant_constant
	.type	invariant_constant, @function
invariant_constant:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	$0, %eax
	movl	%eax, -8(%rbp)
	movl	$0, %eax
	movl	%eax, -12(%rbp)
.L6:
	movl	-12(%rbp), %eax
	movl	-4(%rbp), %ecx
	cmpl	%ecx, %eax
	jge	.L8
	movl	-8(%rbp), %eax
	addl	$408, %eax
	movl	%eax, -8(%rbp)
.L7:
	movl	-12(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -12(%rbp)
	jmp	.L6
.L8:
	movl	-8(%rbp), %eax
	jmp	.L5
.L5:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	invariant_constant, .-invariant_constant
	.section	.note.GNU-stack,"",@progbits
