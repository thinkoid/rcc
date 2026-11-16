	.file	"global-array.c"
	.text
	.globl	get
	.type	get, @function
get:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	leaq	table(%rip), %rax
	movslq	-4(%rbp), %rcx
	movl	(%rax,%rcx,4), %eax
	jmp	.Lget.1
.Lget.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	get, .-get
	.globl	third
	.type	third, @function
third:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	table+12(%rip), %eax
	jmp	.Lthird.1
.Lthird.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	third, .-third
	.globl	put
	.type	put, @function
put:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	leaq	table(%rip), %rax
	movslq	-4(%rbp), %rcx
	movl	-8(%rbp), %edx
	movl	%edx, (%rax,%rcx,4)
.Lput.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	put, .-put
	.globl	get_other
	.type	get_other, @function
get_other:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movq	other@GOTPCREL(%rip), %rax
	movslq	-4(%rbp), %rcx
	movl	(%rax,%rcx,4), %eax
	jmp	.Lget_other.1
.Lget_other.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	get_other, .-get_other
	.globl	third_other
	.type	third_other, @function
third_other:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	other@GOTPCREL(%rip), %rax
	movl	12(%rax), %eax
	jmp	.Lthird_other.1
.Lthird_other.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	third_other, .-third_other
	.bss
	.globl	table
	.type	table, @object
	.size	table, 32
	.p2align 4
table:
	.zero	32
	.section	.note.GNU-stack,"",@progbits
