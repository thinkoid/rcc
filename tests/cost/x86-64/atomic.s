	.file	"atomic.c"
	.text
	.globl	load
	.type	load, @function
load:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	counter(%rip), %eax
	jmp	.Lload.1
.Lload.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	load, .-load
	.globl	bump
	.type	bump, @function
bump:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	addl	$1, counter(%rip)
.Lbump.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	bump, .-bump
	.globl	arm
	.type	arm, @function
arm:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	orb	$1, (%rax)
.Larm.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	arm, .-arm
	.globl	status
	.type	status, @function
status:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movzbl	(%rax), %eax
	shll	$31, %eax
	shrl	$31, %eax
	movq	-8(%rbp), %rcx
	movzbl	(%rcx), %ecx
	shll	$26, %ecx
	shrl	$27, %ecx
	addl	%ecx, %eax
	jmp	.Lstatus.1
.Lstatus.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	status, .-status
	.bss
	.globl	counter
	.type	counter, @object
	.size	counter, 4
	.p2align 2
counter:
	.zero	4
	.section	.note.GNU-stack,"",@progbits
