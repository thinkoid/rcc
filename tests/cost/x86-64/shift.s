	.file	"shift.c"
	.text
	.globl	shl
	.type	shl, @function
shl:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	movl	-8(%rbp), %ecx
	shll	%cl, %eax
	jmp	.Lshl.1
.Lshl.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	shl, .-shl
	.globl	sar
	.type	sar, @function
sar:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	movl	-8(%rbp), %ecx
	sarl	%cl, %eax
	jmp	.Lsar.1
.Lsar.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	sar, .-sar
	.globl	shr
	.type	shr, @function
shr:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	movl	-8(%rbp), %ecx
	shrl	%cl, %eax
	jmp	.Lshr.1
.Lshr.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	shr, .-shr
	.globl	count_extended
	.type	count_extended, @function
count_extended:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	movq	%rax, -16(%rbp)
	movl	-8(%rbp), %eax
	movl	%eax, -20(%rbp)
	movl	-20(%rbp), %edi
	call	narrow@PLT
	movsbl	%al, %ecx
	movq	-16(%rbp), %rax
	shll	%cl, %eax
	jmp	.Lcount_extended.1
.Lcount_extended.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	count_extended, .-count_extended
	.globl	count_compound
	.type	count_compound, @function
count_compound:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-8(%rbp), %eax
	movl	%eax, -12(%rbp)
	movl	-12(%rbp), %edi
	call	narrow@PLT
	movsbl	%al, %ecx
	shll	%cl, -4(%rbp)
	movl	-4(%rbp), %eax
	jmp	.Lcount_compound.1
.Lcount_compound.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	count_compound, .-count_compound
	.globl	count_postfix
	.type	count_postfix, @function
count_postfix:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	movl	-8(%rbp), %edx
	movl	%edx, %ecx
	addl	$1, %edx
	movl	%edx, -8(%rbp)
	shll	%cl, %eax
	jmp	.Lcount_postfix.1
.Lcount_postfix.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	count_postfix, .-count_postfix
	.globl	count_called
	.type	count_called, @function
count_called:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	movq	%rax, -16(%rbp)
	movl	-8(%rbp), %eax
	movl	%eax, -20(%rbp)
	movl	-20(%rbp), %edi
	call	wide@PLT
	movq	-16(%rbp), %rdx
	movl	%eax, %ecx
	shll	%cl, %edx
	movl	%edx, %eax
	jmp	.Lcount_called.1
.Lcount_called.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	count_called, .-count_called
	.globl	count_held
	.type	count_held, @function
count_held:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	movl	%ecx, -28(%rbp)
	movl	%r8d, -32(%rbp)
	movq	-8(%rbp), %rax
	movq	-16(%rbp), %rcx
	movq	-24(%rbp), %rdx
	movl	-28(%rbp), %esi
	movl	-32(%rbp), %edi
	addl	$1, %edi
	movsbl	%dil, %edi
	movq	%rcx, %r8
	movl	%edi, %ecx
	shll	%cl, %esi
	movslq	%esi, %rsi
	addq	%rsi, %rdx
	addq	%rdx, %r8
	addq	%r8, %rax
	jmp	.Lcount_held.1
.Lcount_held.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	count_held, .-count_held
	.section	.note.GNU-stack,"",@progbits
