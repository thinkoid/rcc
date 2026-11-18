	.file	"enum.c"
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
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	cmpl	$1, %eax
	ja	.Lnext.4
	leaq	.Lnext.6(%rip), %rcx
	jmp	*(%rcx,%rax,8)
	.data
	.p2align 3
.Lnext.6:
	.quad	.Lnext.2
	.quad	.Lnext.3
	.text
.Lnext.2:
	movl	$1, %eax
	jmp	.Lnext.1
.Lnext.3:
	movl	$2, %eax
	jmp	.Lnext.1
.Lnext.4:
	movl	$0, %eax
	jmp	.Lnext.1
.Lnext.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	next, .-next
	.globl	signs
	.type	signs, @function
signs:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	subl	$1, %eax
	cmpl	$0, %eax
	setb	%al
	movzbl	%al, %eax
	cmpl	$0, -8(%rbp)
	setl	%cl
	movzbl	%cl, %ecx
	imull	$2, %ecx, %ecx
	addl	%ecx, %eax
	addl	$8, %eax
	jmp	.Lsigns.1
.Lsigns.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	signs, .-signs
	.globl	widen
	.type	widen, @function
widen:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-16(%rbp), %rax
	movl	(%rax), %eax
	movl	%eax, -24(%rbp)
	movq	-8(%rbp), %rax
	movl	%eax, -20(%rbp)
	movq	-8(%rbp), %rax
	movl	-24(%rbp), %ecx
	addq	%rcx, %rax
	movl	-20(%rbp), %ecx
	addq	%rcx, %rax
	movabsq	$4294967296, %rcx
	addq	%rcx, %rax
	addq	$-2, %rax
	jmp	.Lwiden.1
.Lwiden.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	widen, .-widen
	.section	.note.GNU-stack,"",@progbits
