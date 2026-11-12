	.file	"subscript-long.c"
	.text
	.globl	subscript
	.type	subscript, @function
subscript:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	leaq	-28(%rbp), %rax
	movq	-8(%rbp), %rcx
	movl	-12(%rbp), %edx
	movl	%edx, (%rax,%rcx,4)
	leaq	-28(%rbp), %rax
	movq	-8(%rbp), %rcx
	movl	(%rax,%rcx,4), %eax
	jmp	.Lsubscript.1
.Lsubscript.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	subscript, .-subscript
	.section	.note.GNU-stack,"",@progbits
