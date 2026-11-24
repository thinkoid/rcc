	.file	"builtin-expect-volatile.c"
	.text
	.globl	expect_ready
	.type	expect_ready, @function
expect_ready:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -16(%rbp)
	movl	ready(%rip), %eax
	movq	-16(%rbp), %rax
	jmp	.Lexpect_ready.1
.Lexpect_ready.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	expect_ready, .-expect_ready
	.bss
	.globl	ready
	.type	ready, @object
	.size	ready, 4
	.p2align 2
ready:
	.zero	4
	.section	.note.GNU-stack,"",@progbits
