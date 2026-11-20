	.file	"void-arithmetic.c"
	.text
	.globl	forward
	.type	forward, @function
forward:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rax
	addq	-16(%rbp), %rax
	jmp	.Lforward.1
.Lforward.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	forward, .-forward
	.globl	distance
	.type	distance, @function
distance:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rax
	subq	-16(%rbp), %rax
	jmp	.Ldistance.1
.Ldistance.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	distance, .-distance
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
	addq	$1, -8(%rbp)
	subq	$2, -8(%rbp)
	addq	$3, -8(%rbp)
	subq	$1, -8(%rbp)
	movq	-8(%rbp), %rcx
	movq	%rcx, %rax
	addq	$1, %rcx
	movq	%rcx, -8(%rbp)
	jmp	.Lstep.1
.Lstep.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	step, .-step
	.section	.note.GNU-stack,"",@progbits
