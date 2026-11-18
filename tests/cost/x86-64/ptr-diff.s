	.file	"ptr-diff.c"
	.text
	.globl	diff
	.type	diff, @function
diff:
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
	movq	$4, %rcx
	cqto
	idivq	%rcx
	jmp	.Ldiff.1
.Ldiff.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	diff, .-diff
	.globl	register_char
	.type	register_char, @function
register_char:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movq	%rdi, -16(%rbp)
	movq	-16(%rbp), %rbx
	leaq	chars(%rip), %rcx
	movq	%rbx, %rax
	subq	%rcx, %rax
	jmp	.Lregister_char.1
.Lregister_char.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	register_char, .-register_char
	.globl	register_int
	.type	register_int, @function
register_int:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movq	%rdi, -16(%rbp)
	movq	-16(%rbp), %rbx
	leaq	ints(%rip), %rcx
	movq	%rbx, %rax
	subq	%rcx, %rax
	movq	$4, %rcx
	cqto
	idivq	%rcx
	jmp	.Lregister_int.1
.Lregister_int.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	register_int, .-register_int
	.globl	register_row
	.type	register_row, @function
register_row:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movq	%rdi, -16(%rbp)
	movq	-16(%rbp), %rbx
	leaq	rows(%rip), %rcx
	movq	%rbx, %rax
	subq	%rcx, %rax
	movq	$12, %rcx
	cqto
	idivq	%rcx
	jmp	.Lregister_row.1
.Lregister_row.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	register_row, .-register_row
	.globl	assigned
	.type	assigned, @function
assigned:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	leaq	ints+4(%rip), %rbx
	leaq	ints(%rip), %rcx
	movq	%rbx, %rax
	subq	%rcx, %rax
	movq	$4, %rcx
	cqto
	idivq	%rcx
	jmp	.Lassigned.1
.Lassigned.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	assigned, .-assigned
	.globl	called
	.type	called, @function
called:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -24(%rbp)
	movq	-16(%rbp), %rax
	movq	%rax, -32(%rbp)
	movq	-32(%rbp), %rdi
	call	h@PLT
	movq	-24(%rbp), %rcx
	subq	%rax, %rcx
	movq	$4, %rsi
	movq	%rcx, %rax
	cqto
	idivq	%rsi
	jmp	.Lcalled.1
.Lcalled.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	called, .-called
	.globl	called_count
	.type	called_count, @function
called_count:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$48, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	movq	-8(%rbp), %rsi
	movq	-16(%rbp), %rax
	movq	%rax, -32(%rbp)
	movq	%rsi, -40(%rbp)
	movq	-24(%rbp), %rax
	movq	%rax, -48(%rbp)
	movq	-48(%rbp), %rdi
	call	h@PLT
	movq	-32(%rbp), %rcx
	subq	%rax, %rcx
	movq	$4, %rsi
	movq	%rcx, %rax
	cqto
	idivq	%rsi
	movq	%rax, %rcx
	movq	-40(%rbp), %rax
	shlq	%cl, %rax
	jmp	.Lcalled_count.1
.Lcalled_count.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	called_count, .-called_count
	.globl	called_char
	.type	called_char, @function
called_char:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -24(%rbp)
	movq	-16(%rbp), %rax
	movq	%rax, -32(%rbp)
	movq	-32(%rbp), %rdi
	call	hc@PLT
	movq	-24(%rbp), %rcx
	subq	%rax, %rcx
	movq	%rcx, %rax
	jmp	.Lcalled_char.1
.Lcalled_char.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	called_char, .-called_char
	.bss
	.globl	chars
	.type	chars, @object
	.size	chars, 8
chars:
	.zero	8
	.globl	ints
	.type	ints, @object
	.size	ints, 32
	.p2align 4
ints:
	.zero	32
	.globl	rows
	.type	rows, @object
	.size	rows, 96
	.p2align 4
rows:
	.zero	96
	.section	.note.GNU-stack,"",@progbits
