	.file	"struct-static-completed.c"
	.text
	.globl	y_of
	.type	y_of, @function
y_of:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	origin+4(%rip), %eax
	jmp	.Ly_of.1
.Ly_of.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	y_of, .-y_of
	.bss
	.type	origin, @object
	.size	origin, 8
	.p2align 2
origin:
	.zero	8
	.section	.note.GNU-stack,"",@progbits
