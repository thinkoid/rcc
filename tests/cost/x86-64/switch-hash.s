	.file	"switch-hash.c"
	.text
	.globl	reason
	.type	reason, @function
reason:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	$5, %ecx
	xorl	%edx, %edx
	divl	%ecx
	leaq	.Lreason.16(%rip), %rcx
	jmp	*(%rcx,%rdx,8)
	.data
	.p2align 3
.Lreason.16:
	.quad	.Lreason.17
	.quad	.Lreason.18
	.quad	.Lreason.19
	.quad	.Lreason.20
	.quad	.Lreason.21
	.text
.Lreason.17:
	cmpl	$40, %eax
	je	.Lreason.2
	cmpl	$80, %eax
	je	.Lreason.8
	cmpl	$100, %eax
	je	.Lreason.12
	jmp	.Lreason.14
.Lreason.18:
	cmpl	$40, %eax
	je	.Lreason.3
	cmpl	$60, %eax
	je	.Lreason.5
	cmpl	$80, %eax
	je	.Lreason.9
	jmp	.Lreason.14
.Lreason.19:
	cmpl	$60, %eax
	je	.Lreason.6
	jmp	.Lreason.14
.Lreason.20:
	cmpl	$80, %eax
	je	.Lreason.10
	cmpl	$100, %eax
	je	.Lreason.13
	jmp	.Lreason.14
.Lreason.21:
	cmpl	$40, %eax
	je	.Lreason.4
	cmpl	$60, %eax
	je	.Lreason.7
	cmpl	$80, %eax
	je	.Lreason.11
	jmp	.Lreason.14
.Lreason.2:
	movl	$1, %eax
	jmp	.Lreason.1
.Lreason.3:
	movl	$2, %eax
	jmp	.Lreason.1
.Lreason.4:
	movl	$3, %eax
	jmp	.Lreason.1
.Lreason.5:
	movl	$4, %eax
	jmp	.Lreason.1
.Lreason.6:
	movl	$5, %eax
	jmp	.Lreason.1
.Lreason.7:
	movl	$6, %eax
	jmp	.Lreason.1
.Lreason.8:
	movl	$7, %eax
	jmp	.Lreason.1
.Lreason.9:
	movl	$8, %eax
	jmp	.Lreason.1
.Lreason.10:
	movl	$9, %eax
	jmp	.Lreason.1
.Lreason.11:
	movl	$10, %eax
	jmp	.Lreason.1
.Lreason.12:
	movl	$11, %eax
	jmp	.Lreason.1
.Lreason.13:
	movl	$12, %eax
	jmp	.Lreason.1
.Lreason.14:
	movl	$0, %eax
	jmp	.Lreason.1
.Lreason.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	reason, .-reason
	.section	.note.GNU-stack,"",@progbits
