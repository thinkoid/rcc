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
	jge	.Lwhich.2
	movl	$1, %eax
	jmp	.Lwhich.1
.Lwhich.2:
	cmpl	$10, %ebx
	jl	.Lwhich.3
	movl	$2, %eax
	jmp	.Lwhich.1
.Lwhich.3:
	movl	$3, %eax
	jmp	.Lwhich.1
.Lwhich.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	which, .-which
	.section	.note.GNU-stack,"",@progbits
