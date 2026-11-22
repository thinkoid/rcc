	.file	"bit-field-condition.c"
	.text
	.globl	if_used
	.type	if_used, @function
if_used:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movzbl	7(%rax), %eax
	shll	$28, %eax
	shrl	$31, %eax
	cmpl	$0, %eax
	je	.Lif_used.2
	call	f@PLT
.Lif_used.2:
.Lif_used.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	if_used, .-if_used
	.globl	if_not_used
	.type	if_not_used, @function
if_not_used:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movzbl	7(%rax), %eax
	shll	$28, %eax
	shrl	$31, %eax
	cmpl	$0, %eax
	jne	.Lif_not_used.2
	call	f@PLT
.Lif_not_used.2:
.Lif_not_used.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	if_not_used, .-if_not_used
	.globl	if_used_zero
	.type	if_used_zero, @function
if_used_zero:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movzbl	7(%rax), %eax
	shll	$28, %eax
	shrl	$31, %eax
	cmpl	$0, %eax
	jne	.Lif_used_zero.2
	call	f@PLT
.Lif_used_zero.2:
.Lif_used_zero.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	if_used_zero, .-if_used_zero
	.globl	used_and_waited
	.type	used_and_waited, @function
used_and_waited:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movzbl	7(%rax), %eax
	shll	$28, %eax
	shrl	$31, %eax
	cmpl	$0, %eax
	je	.Lused_and_waited.2
	movq	-8(%rbp), %rax
	movzbl	7(%rax), %eax
	shll	$29, %eax
	shrl	$31, %eax
	cmpl	$0, %eax
	jne	.Lused_and_waited.2
	movl	$1, %eax
	jmp	.Lused_and_waited.3
.Lused_and_waited.2:
	movl	$0, %eax
.Lused_and_waited.3:
	jmp	.Lused_and_waited.1
.Lused_and_waited.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	used_and_waited, .-used_and_waited
	.globl	if_level
	.type	if_level, @function
if_level:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movsbl	(%rax), %eax
	shll	$25, %eax
	sarl	$28, %eax
	cmpl	$0, %eax
	je	.Lif_level.2
	call	f@PLT
.Lif_level.2:
.Lif_level.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	if_level, .-if_level
	.globl	if_not_level
	.type	if_not_level, @function
if_not_level:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movsbl	(%rax), %eax
	shll	$25, %eax
	sarl	$28, %eax
	cmpl	$0, %eax
	jne	.Lif_not_level.2
	call	f@PLT
.Lif_not_level.2:
.Lif_not_level.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	if_not_level, .-if_not_level
	.globl	if_level_zero
	.type	if_level_zero, @function
if_level_zero:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movsbl	(%rax), %eax
	shll	$25, %eax
	sarl	$28, %eax
	cmpl	$0, %eax
	jne	.Lif_level_zero.2
	call	f@PLT
.Lif_level_zero.2:
.Lif_level_zero.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	if_level_zero, .-if_level_zero
	.section	.note.GNU-stack,"",@progbits
