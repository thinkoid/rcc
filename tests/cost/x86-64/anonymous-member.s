	.file	"anonymous-member.c"
	.text
	.globl	read_union
	.type	read_union, @function
read_union:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	8(%rax), %rax
	jmp	.Lread_union.1
.Lread_union.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	read_union, .-read_union
	.globl	write_union
	.type	write_union, @function
write_union:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rax
	movq	-16(%rbp), %rcx
	movq	%rcx, 8(%rax)
.Lwrite_union.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	write_union, .-write_union
	.globl	read_struct
	.type	read_struct, @function
read_struct:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movswl	16(%rax), %eax
	movq	-8(%rbp), %rcx
	addl	20(%rcx), %eax
	jmp	.Lread_struct.1
.Lread_struct.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	read_struct, .-read_struct
	.globl	write_struct
	.type	write_struct, @function
write_struct:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rax
	movl	-12(%rbp), %ecx
	movw	%cx, 16(%rax)
	movq	-8(%rbp), %rax
	movl	-12(%rbp), %ecx
	movl	%ecx, 20(%rax)
.Lwrite_struct.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	write_struct, .-write_struct
	.globl	local
	.type	local, @function
local:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	$1, -16(%rbp)
	movl	$2, -4(%rbp)
	movl	-16(%rbp), %eax
	addl	-4(%rbp), %eax
	jmp	.Llocal.1
.Llocal.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	local, .-local
	.globl	global
	.type	global, @function
global:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	$3, g+8(%rip)
	movq	g+8(%rip), %rax
	movswq	g+16(%rip), %rcx
	addq	%rcx, %rax
	jmp	.Lglobal.1
.Lglobal.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	global, .-global
	.globl	both
	.type	both, @function
both:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movl	$1, (%rax)
	movq	-8(%rbp), %rax
	movl	4(%rax), %eax
	jmp	.Lboth.1
.Lboth.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	both, .-both
	.globl	listed
	.type	listed, @function
listed:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	$0, -8(%rbp)
	movl	$5, -8(%rbp)
	movl	$6, -4(%rbp)
	movl	-4(%rbp), %eax
	jmp	.Llisted.1
.Llisted.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	listed, .-listed
	.data
	.globl	preset
	.type	preset, @object
	.size	preset, 24
	.p2align 3
preset:
	.long	1
	.zero	4
	.long	2
	.zero	4
	.short	3
	.zero	2
	.long	4
	.bss
	.globl	g
	.type	g, @object
	.size	g, 24
	.p2align 3
g:
	.zero	24
	.section	.note.GNU-stack,"",@progbits
