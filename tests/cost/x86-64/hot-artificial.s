	.file	"hot-artificial.c"
	.text
	.globl	both
	.type	both, @function
both:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -8(%rbp)
	movl	-8(%rbp), %edi
	call	warm@PLT
	movq	%rax, -16(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -20(%rbp)
	movl	-20(%rbp), %edi
	call	plain@PLT
	addl	-16(%rbp), %eax
	jmp	.Lboth.1
.Lboth.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	both, .-both
	.section	.note.GNU-stack,"",@progbits
