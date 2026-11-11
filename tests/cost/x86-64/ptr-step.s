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
	addq	$4, -8(%rbp)
	subq	$8, -8(%rbp)
	movq	-8(%rbp), %rax
	jmp	.Lstep.1
.Lstep.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	step, .-step
	.globl	in_value
	.type	in_value, @function
in_value:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	subq	$8, %rax
	movq	%rax, -8(%rbp)
	jmp	.Lin_value.1
.Lin_value.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	in_value, .-in_value
	.section	.note.GNU-stack,"",@progbits
