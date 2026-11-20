	.file	"void-function-pointer.c"
	.text
	.globl	install
	.type	install, @function
install:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, handler(%rip)
.Linstall.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	install, .-install
	.globl	erase
	.type	erase, @function
erase:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	jmp	.Lerase.1
.Lerase.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	erase, .-erase
	.bss
	.globl	handler
	.type	handler, @object
	.size	handler, 8
	.p2align 3
handler:
	.zero	8
	.section	.note.GNU-stack,"",@progbits
