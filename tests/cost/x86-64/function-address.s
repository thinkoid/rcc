	.file	"function-address.c"
	.text
	.globl	defined
	.type	defined, @function
defined:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	jmp	.Ldefined.1
.Ldefined.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	defined, .-defined
	.globl	own
	.type	own, @function
own:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	leaq	defined(%rip), %rax
	jmp	.Lown.1
.Lown.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	own, .-own
	.globl	other
	.type	other, @function
other:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	declared@GOTPCREL(%rip), %rax
	jmp	.Lother.1
.Lother.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	other, .-other
	.section	.note.GNU-stack,"",@progbits
