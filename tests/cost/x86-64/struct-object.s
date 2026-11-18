	.file	"struct-object.c"
	.text
	.globl	locals
	.type	locals, @function
locals:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$48, %rsp
	leaq	-3(%rbp), %rax
	movq	%rax, -32(%rbp)
	movq	-32(%rbp), %rdi
	call	use@PLT
	leaq	-16(%rbp), %rax
	movq	%rax, -40(%rbp)
	movq	-40(%rbp), %rdi
	call	use@PLT
	leaq	-24(%rbp), %rax
	movq	%rax, -48(%rbp)
	movq	-48(%rbp), %rdi
	call	use@PLT
	movl	-8(%rbp), %eax
	jmp	.Llocals.1
.Llocals.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	locals, .-locals
	.globl	globals
	.type	globals, @function
globals:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	leaq	three(%rip), %rax
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rdi
	call	use@PLT
	leaq	one(%rip), %rax
	movq	%rax, -16(%rbp)
	movq	-16(%rbp), %rdi
	call	use@PLT
	movl	wide+8(%rip), %eax
	movswl	six+4(%rip), %ecx
	addl	%ecx, %eax
	jmp	.Lglobals.1
.Lglobals.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	globals, .-globals
	.globl	element
	.type	element, @function
element:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	pairs+40(%rip), %rax
	jmp	.Lelement.1
.Lelement.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	element, .-element
	.globl	sizes
	.type	sizes, @function
sizes:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	$77, %rax
	jmp	.Lsizes.1
.Lsizes.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	sizes, .-sizes
	.bss
	.globl	three
	.type	three, @object
	.size	three, 3
three:
	.zero	3
	.globl	six
	.type	six, @object
	.size	six, 6
	.p2align 1
six:
	.zero	6
	.globl	wide
	.type	wide, @object
	.size	wide, 12
	.p2align 2
wide:
	.zero	12
	.globl	pairs
	.type	pairs, @object
	.size	pairs, 48
	.p2align 4
pairs:
	.zero	48
	.type	one, @object
	.size	one, 8
	.p2align 3
one:
	.zero	8
	.section	.note.GNU-stack,"",@progbits
