	.file	"subscript-displacement.c"
	.text
	.globl	row
	.type	row, @function
row:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rax
	movslq	-12(%rbp), %rcx
	movsbl	2(%rax,%rcx,8), %eax
	jmp	.Lrow.1
.Lrow.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	row, .-row
	.globl	local
	.type	local, @function
local:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movslq	-4(%rbp), %rax
	movsbl	-34(%rbp,%rax,8), %eax
	jmp	.Llocal.1
.Llocal.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	local, .-local
	.globl	cell
	.type	cell, @function
cell:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movl	20(%rax), %eax
	jmp	.Lcell.1
.Lcell.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	cell, .-cell
	.globl	address
	.type	address, @function
address:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rax
	movslq	-12(%rbp), %rcx
	leaq	2(%rax,%rcx,8), %rax
	jmp	.Laddress.1
.Laddress.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	address, .-address
	.globl	corner
	.type	corner, @function
corner:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	leaq	20(%rax), %rax
	jmp	.Lcorner.1
.Lcorner.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	corner, .-corner
	.section	.note.GNU-stack,"",@progbits
