	.file	"ptr-compare.c"
	.text
	.globl	less
	.type	less, @function
less:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rax
	movq	-16(%rbp), %rcx
	cmpq	%rcx, %rax
	setb	%al
	movzbl	%al, %eax
	jmp	.Lless.1
.Lless.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	less, .-less
	.section	.note.GNU-stack,"",@progbits
