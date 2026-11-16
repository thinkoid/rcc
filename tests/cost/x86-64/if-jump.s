	.file	"if-jump.c"
	.text
	.globl	low
	.type	low, @function
low:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	.p2align 5
.Llow.2:
	movl	-4(%rbp), %eax
	cmpl	$0, %eax
	je	.Llow.3
	movl	-4(%rbp), %eax
	testl	$1, %eax
	jne	.Llow.3
	movl	-4(%rbp), %eax
	sarl	$1, %eax
	movl	%eax, -4(%rbp)
	jmp	.Llow.2
.Llow.3:
	movl	-4(%rbp), %eax
	jmp	.Llow.1
.Llow.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	low, .-low
	.globl	low_else
	.type	low_else, @function
low_else:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	$0, -8(%rbp)
	.p2align 5
.Llow_else.2:
	movl	-4(%rbp), %eax
	cmpl	$0, %eax
	je	.Llow_else.3
	movl	-4(%rbp), %eax
	testl	$1, %eax
	jne	.Llow_else.3
	movl	-8(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -8(%rbp)
	movl	-4(%rbp), %eax
	sarl	$1, %eax
	movl	%eax, -4(%rbp)
	jmp	.Llow_else.2
.Llow_else.3:
	movl	-8(%rbp), %eax
	jmp	.Llow_else.1
.Llow_else.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	low_else, .-low_else
	.globl	even
	.type	even, @function
even:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	$0, -8(%rbp)
	movl	$0, -12(%rbp)
	.p2align 5
.Leven.2:
	movl	-12(%rbp), %eax
	movl	-4(%rbp), %ecx
	cmpl	%ecx, %eax
	jge	.Leven.4
	movl	-12(%rbp), %eax
	testl	$1, %eax
	jne	.Leven.3
	movl	-8(%rbp), %eax
	movl	-12(%rbp), %ecx
	addl	%ecx, %eax
	movl	%eax, -8(%rbp)
.Leven.3:
	movl	-12(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -12(%rbp)
	jmp	.Leven.2
.Leven.4:
	movl	-8(%rbp), %eax
	jmp	.Leven.1
.Leven.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	even, .-even
	.globl	even_else
	.type	even_else, @function
even_else:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	$0, -8(%rbp)
	movl	$0, -12(%rbp)
	.p2align 5
.Leven_else.2:
	movl	-12(%rbp), %eax
	movl	-4(%rbp), %ecx
	cmpl	%ecx, %eax
	jge	.Leven_else.4
	movl	-12(%rbp), %eax
	testl	$1, %eax
	jne	.Leven_else.3
	movl	-8(%rbp), %eax
	movl	-12(%rbp), %ecx
	addl	%ecx, %eax
	movl	%eax, -8(%rbp)
	movl	-8(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -8(%rbp)
.Leven_else.3:
	movl	-12(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -12(%rbp)
	jmp	.Leven_else.2
.Leven_else.4:
	movl	-8(%rbp), %eax
	jmp	.Leven_else.1
.Leven_else.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	even_else, .-even_else
	.globl	clamp
	.type	clamp, @function
clamp:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	cmpl	$0, %eax
	jge	.Lclamp.2
	movl	$0, -4(%rbp)
.Lclamp.2:
	movl	-4(%rbp), %eax
	jmp	.Lclamp.1
.Lclamp.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	clamp, .-clamp
	.globl	clamp_else
	.type	clamp_else, @function
clamp_else:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	cmpl	$0, %eax
	jge	.Lclamp_else.2
	movl	$0, -4(%rbp)
	movl	-4(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -4(%rbp)
.Lclamp_else.2:
	movl	-4(%rbp), %eax
	jmp	.Lclamp_else.1
.Lclamp_else.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	clamp_else, .-clamp_else
	.globl	early
	.type	early, @function
early:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movl	(%rax), %eax
	cmpl	$0, %eax
	jne	.Learly.1
	movq	-8(%rbp), %rax
	movl	$1, (%rax)
.Learly.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	early, .-early
	.globl	early_else
	.type	early_else, @function
early_else:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movl	(%rax), %eax
	cmpl	$0, %eax
	jne	.Learly_else.1
	movq	-8(%rbp), %rax
	movl	$1, (%rax)
	movq	-8(%rbp), %rax
	movq	-8(%rbp), %rcx
	movl	(%rcx), %ecx
	addl	$1, %ecx
	movl	%ecx, (%rax)
.Learly_else.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	early_else, .-early_else
	.globl	tally
	.type	tally, @function
tally:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	$0, -8(%rbp)
	movl	$0, -12(%rbp)
	.p2align 5
.Ltally.2:
	movl	-12(%rbp), %eax
	movl	-4(%rbp), %ecx
	cmpl	%ecx, %eax
	jge	.Ltally.4
	movl	-12(%rbp), %eax
	andl	$3, %eax
	cmpl	$1, %eax
	ja	.Ltally.7
	movl	%eax, %eax
	leaq	.Ltally.8(%rip), %rcx
	jmp	*(%rcx,%rax,8)
	.data
	.p2align 3
.Ltally.8:
	.quad	.Ltally.5
	.quad	.Ltally.6
	.text
.Ltally.5:
	movl	-8(%rbp), %eax
	cmpl	$100, %eax
	jg	.Ltally.7
	movl	-8(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -8(%rbp)
	jmp	.Ltally.7
.Ltally.6:
	movl	-12(%rbp), %eax
	cmpl	$10, %eax
	jg	.Ltally.3
	movl	-8(%rbp), %eax
	addl	$2, %eax
	movl	%eax, -8(%rbp)
.Ltally.7:
	movl	-8(%rbp), %eax
	addl	$4, %eax
	movl	%eax, -8(%rbp)
.Ltally.3:
	movl	-12(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -12(%rbp)
	jmp	.Ltally.2
.Ltally.4:
	movl	-8(%rbp), %eax
	jmp	.Ltally.1
.Ltally.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	tally, .-tally
	.globl	low_braced
	.type	low_braced, @function
low_braced:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	.p2align 5
.Llow_braced.2:
	movl	-4(%rbp), %eax
	cmpl	$0, %eax
	je	.Llow_braced.3
	movl	-4(%rbp), %eax
	testl	$1, %eax
	jne	.Llow_braced.3
	movl	-4(%rbp), %eax
	sarl	$1, %eax
	movl	%eax, -4(%rbp)
	jmp	.Llow_braced.2
.Llow_braced.3:
	movl	-4(%rbp), %eax
	jmp	.Llow_braced.1
.Llow_braced.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	low_braced, .-low_braced
	.section	.note.GNU-stack,"",@progbits
