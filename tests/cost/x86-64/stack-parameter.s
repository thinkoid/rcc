	.file	"stack-parameter.c"
	.text
	.globl	ten
	.type	ten, @function
ten:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	%edx, -12(%rbp)
	movl	%ecx, -16(%rbp)
	movl	%r8d, -20(%rbp)
	movl	%r9d, -24(%rbp)
	movl	-4(%rbp), %eax
	movsbl	32(%rbp), %ecx
	addl	%ecx, %eax
	movslq	%eax, %rax
	movq	40(%rbp), %rcx
	addq	%rcx, %rax
	jmp	.Lten.1
.Lten.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	ten, .-ten
	.section	.note.GNU-stack,"",@progbits
