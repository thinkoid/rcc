	.file	"switch-direct.c"
	.text
	.globl	day
	.type	day, @function
day:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	subl	$1, %eax
	cmpl	$4, %eax
	ja	.Lday.6
	leaq	.Lday.8(%rip), %rcx
	jmp	*(%rcx,%rax,8)
	.data
	.p2align 3
.Lday.8:
	.quad	.Lday.2
	.quad	.Lday.3
	.quad	.Lday.4
	.quad	.Lday.6
	.quad	.Lday.5
	.text
.Lday.2:
	movl	$10, %eax
	jmp	.Lday.1
.Lday.3:
	movl	$20, %eax
	jmp	.Lday.1
.Lday.4:
	movl	$30, %eax
	jmp	.Lday.1
.Lday.5:
	movl	$50, %eax
	jmp	.Lday.1
.Lday.6:
	movl	$0, %eax
	jmp	.Lday.1
.Lday.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	day, .-day
	.globl	digit
	.type	digit, @function
digit:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	cmpl	$3, %eax
	ja	.Ldigit.6
	movl	%eax, %eax
	leaq	.Ldigit.7(%rip), %rcx
	jmp	*(%rcx,%rax,8)
	.data
	.p2align 3
.Ldigit.7:
	.quad	.Ldigit.2
	.quad	.Ldigit.3
	.quad	.Ldigit.4
	.quad	.Ldigit.5
	.text
.Ldigit.2:
	movl	$7, %eax
	jmp	.Ldigit.1
.Ldigit.3:
	movl	$6, %eax
	jmp	.Ldigit.1
.Ldigit.4:
	movl	$5, %eax
	jmp	.Ldigit.1
.Ldigit.5:
	movl	$4, %eax
	jmp	.Ldigit.1
.Ldigit.6:
	movl	$-1, %eax
	jmp	.Ldigit.1
.Ldigit.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	digit, .-digit
	.section	.note.GNU-stack,"",@progbits
