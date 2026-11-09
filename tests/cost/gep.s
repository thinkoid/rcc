	.file	"gep.c"
	.text
	.globl	gep
	.type	gep, @function
gep:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	leaq	-20(%rbp), %rax
	movl	-4(%rbp), %ecx
	movslq	%ecx, %rcx
	leaq	(%rax,%rcx,4), %rax
	movq	%rax, -32(%rbp)
	movq	-32(%rbp), %rax
	jmp	.Lgep.1
.Lgep.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	gep, .-gep
	.section	.note.GNU-stack,"",@progbits
