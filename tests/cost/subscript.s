	.file	"subscript.c"
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
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	leaq	-24(%rbp), %rax
	movl	-4(%rbp), %ecx
	movslq	%ecx, %rcx
	movl	-8(%rbp), %edx
	movl	%edx, (%rax,%rcx,4)
	leaq	-24(%rbp), %rax
	movl	-4(%rbp), %ecx
	movslq	%ecx, %rcx
	movl	(%rax,%rcx,4), %eax
	jmp	.L1
.L1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	subscript, .-subscript
	.section	.note.GNU-stack,"",@progbits
