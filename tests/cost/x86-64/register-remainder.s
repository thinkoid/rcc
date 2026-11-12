	.file	"register-remainder.c"
	.text
	.globl	rem
	.type	rem, @function
rem:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movl	%edi, %ebx
	movl	%esi, -12(%rbp)
	movl	-12(%rbp), %ecx
	addl	$1, %ecx
	movl	%ebx, %eax
	cltd
	idivl	%ecx
	movl	%edx, %eax
	jmp	.Lrem.1
.Lrem.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	rem, .-rem
	.globl	urem
	.type	urem, @function
urem:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movl	%edi, %ebx
	movl	%esi, -12(%rbp)
	movl	-12(%rbp), %ecx
	addl	$1, %ecx
	movl	%ebx, %eax
	xorl	%edx, %edx
	divl	%ecx
	movl	%edx, %eax
	jmp	.Lurem.1
.Lurem.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	urem, .-urem
	.section	.note.GNU-stack,"",@progbits
