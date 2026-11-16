	.file	"div-busy.c"
	.text
	.globl	div_busy
	.type	div_busy, @function
div_busy:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	%edx, -12(%rbp)
	movl	-4(%rbp), %eax
	movl	-8(%rbp), %ecx
	movq	%rax, %rsi
	movl	%ecx, %eax
	cltd
	idivl	-12(%rbp)
	movl	%eax, %ecx
	addl	%ecx, %esi
	movl	%esi, %eax
	jmp	.Ldiv_busy.1
.Ldiv_busy.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	div_busy, .-div_busy
	.globl	rem_busy
	.type	rem_busy, @function
rem_busy:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	%edx, -12(%rbp)
	movl	-4(%rbp), %eax
	movl	-8(%rbp), %ecx
	movq	%rax, %rsi
	movl	%ecx, %eax
	cltd
	idivl	-12(%rbp)
	movl	%edx, %ecx
	addl	%ecx, %esi
	movl	%esi, %eax
	jmp	.Lrem_busy.1
.Lrem_busy.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	rem_busy, .-rem_busy
	.section	.note.GNU-stack,"",@progbits
