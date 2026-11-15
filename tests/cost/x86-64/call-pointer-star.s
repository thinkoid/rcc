	.file	"call-pointer-star.c"
	.text
	.globl	plain
	.type	plain, @function
plain:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -24(%rbp)
	movl	-12(%rbp), %eax
	movl	%eax, -28(%rbp)
	movl	-28(%rbp), %edi
	call	*-24(%rbp)
	jmp	.Lplain.1
.Lplain.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	plain, .-plain
	.globl	star
	.type	star, @function
star:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -24(%rbp)
	movl	-12(%rbp), %eax
	movl	%eax, -28(%rbp)
	movl	-28(%rbp), %edi
	call	*-24(%rbp)
	jmp	.Lstar.1
.Lstar.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	star, .-star
	.globl	stars
	.type	stars, @function
stars:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -24(%rbp)
	movl	-12(%rbp), %eax
	movl	%eax, -28(%rbp)
	movl	-28(%rbp), %edi
	call	*-24(%rbp)
	jmp	.Lstars.1
.Lstars.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	stars, .-stars
	.section	.note.GNU-stack,"",@progbits
