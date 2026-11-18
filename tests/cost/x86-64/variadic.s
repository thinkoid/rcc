	.file	"variadic.c"
	.text
	.globl	sum
	.type	sum, @function
sum:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$224, %rsp
	movl	%edi, -4(%rbp)
	movq	%rdi, -192(%rbp)
	movq	%rsi, -184(%rbp)
	movq	%rdx, -176(%rbp)
	movq	%rcx, -168(%rbp)
	movq	%r8, -160(%rbp)
	movq	%r9, -152(%rbp)
	movaps	%xmm0, -144(%rbp)
	movaps	%xmm1, -128(%rbp)
	movaps	%xmm2, -112(%rbp)
	movaps	%xmm3, -96(%rbp)
	movaps	%xmm4, -80(%rbp)
	movaps	%xmm5, -64(%rbp)
	movaps	%xmm6, -48(%rbp)
	movaps	%xmm7, -32(%rbp)
	leaq	16(%rbp), %rax
	movq	%rax, -16(%rbp)
	movq	$0, -224(%rbp)
	movl	$8, -216(%rbp)
	movl	$48, -212(%rbp)
	movq	-16(%rbp), %rax
	movq	%rax, -208(%rbp)
	leaq	-192(%rbp), %rax
	movq	%rax, -200(%rbp)
	.p2align 5
.Lsum.2:
	movl	-4(%rbp), %eax
	movl	%eax, %ecx
	subl	$1, %eax
	movl	%eax, -4(%rbp)
	cmpl	$0, %ecx
	jle	.Lsum.3
	cmpl	$40, -216(%rbp)
	ja	.Lsum.4
	addl	$8, -216(%rbp)
	movq	-200(%rbp), %rax
	movl	-216(%rbp), %ecx
	subl	$8, %ecx
	addq	%rcx, %rax
	jmp	.Lsum.5
.Lsum.4:
	movq	-208(%rbp), %rax
	addq	$8, %rax
	movq	%rax, -208(%rbp)
	movq	-208(%rbp), %rax
	subq	$8, %rax
.Lsum.5:
	movq	(%rax), %rax
	addq	%rax, -224(%rbp)
	jmp	.Lsum.2
.Lsum.3:
	leaq	-216(%rbp), %rax
	movq	-224(%rbp), %rax
	jmp	.Lsum.1
.Lsum.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	sum, .-sum
	.globl	twice
	.type	twice, @function
twice:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$256, %rsp
	movl	%edi, -4(%rbp)
	movq	%rdi, -192(%rbp)
	movq	%rsi, -184(%rbp)
	movq	%rdx, -176(%rbp)
	movq	%rcx, -168(%rbp)
	movq	%r8, -160(%rbp)
	movq	%r9, -152(%rbp)
	movaps	%xmm0, -144(%rbp)
	movaps	%xmm1, -128(%rbp)
	movaps	%xmm2, -112(%rbp)
	movaps	%xmm3, -96(%rbp)
	movaps	%xmm4, -80(%rbp)
	movaps	%xmm5, -64(%rbp)
	movaps	%xmm6, -48(%rbp)
	movaps	%xmm7, -32(%rbp)
	leaq	16(%rbp), %rax
	movq	%rax, -16(%rbp)
	movl	$0, -244(%rbp)
	movl	$8, -216(%rbp)
	movl	$48, -212(%rbp)
	movq	-16(%rbp), %rax
	movq	%rax, -208(%rbp)
	leaq	-192(%rbp), %rax
	movq	%rax, -200(%rbp)
	movq	-216(%rbp), %rax
	movq	%rax, -240(%rbp)
	movq	-208(%rbp), %rax
	movq	%rax, -232(%rbp)
	movq	-200(%rbp), %rax
	movq	%rax, -224(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -248(%rbp)
	.p2align 5
.Ltwice.2:
	cmpl	$0, -248(%rbp)
	jle	.Ltwice.4
	cmpl	$40, -216(%rbp)
	ja	.Ltwice.5
	addl	$8, -216(%rbp)
	movq	-200(%rbp), %rax
	movl	-216(%rbp), %ecx
	subl	$8, %ecx
	addq	%rcx, %rax
	jmp	.Ltwice.6
.Ltwice.5:
	movq	-208(%rbp), %rax
	addq	$8, %rax
	movq	%rax, -208(%rbp)
	movq	-208(%rbp), %rax
	subq	$8, %rax
.Ltwice.6:
	movl	(%rax), %eax
	movq	%rax, -256(%rbp)
	cmpl	$40, -240(%rbp)
	ja	.Ltwice.7
	addl	$8, -240(%rbp)
	movq	-224(%rbp), %rax
	movl	-240(%rbp), %ecx
	subl	$8, %ecx
	addq	%rcx, %rax
	jmp	.Ltwice.8
.Ltwice.7:
	movq	-232(%rbp), %rax
	addq	$8, %rax
	movq	%rax, -232(%rbp)
	movq	-232(%rbp), %rax
	subq	$8, %rax
.Ltwice.8:
	movq	-256(%rbp), %rcx
	addl	(%rax), %ecx
	addl	%ecx, -244(%rbp)
	subl	$1, -248(%rbp)
	jmp	.Ltwice.2
.Ltwice.4:
	leaq	-240(%rbp), %rax
	leaq	-216(%rbp), %rax
	movl	-244(%rbp), %eax
	jmp	.Ltwice.1
.Ltwice.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	twice, .-twice
	.globl	stepped
	.type	stepped, @function
stepped:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	$0, -32(%rbp)
	.p2align 5
.Lstepped.2:
	movl	-12(%rbp), %eax
	movl	%eax, %ecx
	subl	$1, %eax
	movl	%eax, -12(%rbp)
	cmpl	$0, %ecx
	jle	.Lstepped.3
	movq	-8(%rbp), %rax
	movq	%rax, %rcx
	addq	$24, %rax
	movq	%rax, -8(%rbp)
	movq	%rcx, -24(%rbp)
	movq	-24(%rbp), %rax
	cmpl	$40, (%rax)
	ja	.Lstepped.4
	movq	-24(%rbp), %rax
	addl	$8, (%rax)
	movq	-24(%rbp), %rax
	movq	16(%rax), %rax
	movq	-24(%rbp), %rcx
	movl	(%rcx), %ecx
	subl	$8, %ecx
	addq	%rcx, %rax
	jmp	.Lstepped.5
.Lstepped.4:
	movq	-24(%rbp), %rax
	movq	-24(%rbp), %rcx
	movq	8(%rcx), %rcx
	addq	$8, %rcx
	movq	%rcx, 8(%rax)
	movq	-24(%rbp), %rax
	movq	8(%rax), %rax
	subq	$8, %rax
.Lstepped.5:
	movq	(%rax), %rax
	addq	%rax, -32(%rbp)
	jmp	.Lstepped.2
.Lstepped.3:
	movq	-32(%rbp), %rax
	jmp	.Lstepped.1
.Lstepped.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	stepped, .-stepped
	.globl	print
	.type	print, @function
print:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$240, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rdi, -192(%rbp)
	movq	%rsi, -184(%rbp)
	movq	%rdx, -176(%rbp)
	movq	%rcx, -168(%rbp)
	movq	%r8, -160(%rbp)
	movq	%r9, -152(%rbp)
	movaps	%xmm0, -144(%rbp)
	movaps	%xmm1, -128(%rbp)
	movaps	%xmm2, -112(%rbp)
	movaps	%xmm3, -96(%rbp)
	movaps	%xmm4, -80(%rbp)
	movaps	%xmm5, -64(%rbp)
	movaps	%xmm6, -48(%rbp)
	movaps	%xmm7, -32(%rbp)
	leaq	16(%rbp), %rax
	movq	%rax, -16(%rbp)
	movl	$8, -216(%rbp)
	movl	$48, -212(%rbp)
	movq	-16(%rbp), %rax
	movq	%rax, -208(%rbp)
	leaq	-192(%rbp), %rax
	movq	%rax, -200(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -232(%rbp)
	leaq	-216(%rbp), %rax
	movq	%rax, -240(%rbp)
	movq	-232(%rbp), %rdi
	movq	-240(%rbp), %rsi
	call	vprintf@PLT
	movl	%eax, -220(%rbp)
	leaq	-216(%rbp), %rax
	movl	-220(%rbp), %eax
	jmp	.Lprint.1
.Lprint.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	print, .-print
	.section	.note.GNU-stack,"",@progbits
