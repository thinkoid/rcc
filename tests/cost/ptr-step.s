	.file	"ptr-step.c"
	.text
	.globl	step
	.type	step, @function
step:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	addq	$4, %rax
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	subq	$8, %rax
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	jmp	.Lstep.1
.Lstep.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	step, .-step
	.section	.note.GNU-stack,"",@progbits
