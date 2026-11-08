	.file	"register-undef.c"
	.text
	.globl	which
	.type	which, @function
which:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	cmpl	$10, %ebx
	jge	.L2
	movl	$1, %eax
	jmp	.L1
.L2:
	cmpl	$10, %ebx
	jl	.L3
	movl	$2, %eax
	jmp	.L1
.L3:
	movl	$3, %eax
	jmp	.L1
.L1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	which, .-which
	.section	.note.GNU-stack,"",@progbits
