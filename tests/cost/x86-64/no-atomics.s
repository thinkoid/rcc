	.file	"no-atomics.c"
	.text
	.globl	no_atomics
	.type	no_atomics, @function
no_atomics:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	$1, %eax
	jmp	.Lno_atomics.1
.Lno_atomics.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	no_atomics, .-no_atomics
	.section	.note.GNU-stack,"",@progbits
