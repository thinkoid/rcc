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
	movl	-4(%rbp), %ecx
	movl	-8(%rbp), %eax
	cltd
	idivl	-12(%rbp)
	addl	%eax, %ecx
	movl	%ecx, %eax
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
	movl	-4(%rbp), %ecx
	movl	-8(%rbp), %eax
	cltd
	idivl	-12(%rbp)
	movl	%edx, %eax
	addl	%eax, %ecx
	movl	%ecx, %eax
	jmp	.Lrem_busy.1
.Lrem_busy.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	rem_busy, .-rem_busy
	.globl	div_deeper
	.type	div_deeper, @function
div_deeper:
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
	addl	$1, %ecx
	addl	%ecx, %esi
	movl	%esi, %eax
	jmp	.Ldiv_deeper.1
.Ldiv_deeper.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	div_deeper, .-div_deeper
	.globl	div_held
	.type	div_held, @function
div_held:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	%edx, -12(%rbp)
	movl	%ecx, -16(%rbp)
	movl	%r8d, -20(%rbp)
	movl	%r9d, -24(%rbp)
	movl	-4(%rbp), %ecx
	movl	-8(%rbp), %esi
	movl	-12(%rbp), %edi
	movl	-16(%rbp), %r8d
	movl	-20(%rbp), %r9d
	movl	-24(%rbp), %r10d
	movl	16(%rbp), %r11d
	movl	24(%rbp), %eax
	movl	32(%rbp), %edx
	movq	%rcx, -32(%rbp)
	movq	%rax, %rcx
	movl	%edx, %eax
	cltd
	idivl	40(%rbp)
	movl	%eax, %edx
	addl	%edx, %ecx
	addl	%ecx, %r11d
	addl	%r11d, %r10d
	addl	%r10d, %r9d
	addl	%r9d, %r8d
	addl	%r8d, %edi
	addl	%edi, %esi
	movq	-32(%rbp), %rcx
	addl	%esi, %ecx
	movl	48(%rbp), %eax
	cltd
	idivl	56(%rbp)
	addl	%eax, %ecx
	movl	%ecx, %eax
	jmp	.Ldiv_held.1
.Ldiv_held.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	div_held, .-div_held
	.section	.note.GNU-stack,"",@progbits
