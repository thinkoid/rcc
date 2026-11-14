	.file	"subscript-row.c"
	.text
	.globl	first
	.type	first, @function
first:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	leaq	-24(%rbp), %rax
	jmp	.Lfirst.1
.Lfirst.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	first, .-first
	.globl	element
	.type	element, @function
element:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	leaq	-24(%rbp), %rax
	movl	4(%rax), %eax
	jmp	.Lelement.1
.Lelement.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	element, .-element
	.globl	second
	.type	second, @function
second:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	leaq	-24(%rbp), %rax
	addq	$12, %rax
	jmp	.Lsecond.1
.Lsecond.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	second, .-second
	.section	.note.GNU-stack,"",@progbits
