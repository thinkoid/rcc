	.file	"gnu-declarations.c"
	.text
	.globl	use
	.type	use, @function
use:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$80, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -32(%rbp)
	movq	-32(%rbp), %rdi
	call	count@PLT
	movq	%rax, -40(%rbp)
	movq	-16(%rbp), %rax
	movl	(%rax), %eax
	movl	%eax, -44(%rbp)
	movl	-44(%rbp), %edi
	call	other_name@PLT
	movq	-40(%rbp), %rcx
	addl	%eax, %ecx
	movq	%rcx, -56(%rbp)
	movq	-16(%rbp), %rax
	movq	%rax, -64(%rbp)
	movq	-24(%rbp), %rax
	movq	%rax, -72(%rbp)
	movq	-64(%rbp), %rdi
	movq	-72(%rbp), %rsi
	call	add@PLT
	movq	-56(%rbp), %rcx
	addl	%ecx, %eax
	jmp	.Luse.1
.Luse.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	use, .-use
	.section	.note.GNU-stack,"",@progbits
