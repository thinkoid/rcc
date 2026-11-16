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
	movslq	-4(%rbp), %rax
	movl	-8(%rbp), %ecx
	movl	%ecx, -24(%rbp,%rax,4)
	movslq	-4(%rbp), %rax
	movl	-24(%rbp,%rax,4), %eax
	jmp	.Lsubscript.1
.Lsubscript.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	subscript, .-subscript
	.section	.note.GNU-stack,"",@progbits
