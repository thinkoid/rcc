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
	addl	%edx, %ecx
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
	addl	%eax, %ecx
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
	.globl	rem_stored
	.type	rem_stored, @function
rem_stored:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movl	%edx, -16(%rbp)
	movq	-8(%rbp), %rcx
	movl	-12(%rbp), %eax
	cltd
	idivl	-16(%rbp)
	movl	%edx, (%rcx)
.Lrem_stored.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	rem_stored, .-rem_stored
	.globl	rem_returned
	.type	rem_returned, @function
rem_returned:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	cltd
	idivl	-8(%rbp)
	movl	%edx, %eax
	jmp	.Lrem_returned.1
.Lrem_returned.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	rem_returned, .-rem_returned
	.globl	rem_left
	.type	rem_left, @function
rem_left:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	%edx, -12(%rbp)
	movl	-8(%rbp), %eax
	cltd
	idivl	-12(%rbp)
	addl	-4(%rbp), %edx
	movl	%edx, %eax
	jmp	.Lrem_left.1
.Lrem_left.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	rem_left, .-rem_left
	.globl	rem_argument
	.type	rem_argument, @function
rem_argument:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	cltd
	idivl	-8(%rbp)
	movl	%edx, -12(%rbp)
	movl	-12(%rbp), %edi
	call	f@PLT
	jmp	.Lrem_argument.1
.Lrem_argument.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	rem_argument, .-rem_argument
	.globl	rem_quotient
	.type	rem_quotient, @function
rem_quotient:
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
	cltd
	idivl	-8(%rbp)
	movl	%edx, %eax
	cltd
	idivl	-12(%rbp)
	jmp	.Lrem_quotient.1
.Lrem_quotient.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	rem_quotient, .-rem_quotient
	.globl	rem_remainder
	.type	rem_remainder, @function
rem_remainder:
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
	cltd
	idivl	-8(%rbp)
	movl	%edx, %eax
	cltd
	idivl	-12(%rbp)
	movl	%edx, %eax
	jmp	.Lrem_remainder.1
.Lrem_remainder.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	rem_remainder, .-rem_remainder
	.globl	rem_register
	.type	rem_register, @function
rem_register:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movl	%edi, -12(%rbp)
	movl	%esi, -16(%rbp)
	movl	-12(%rbp), %ebx
	movl	-16(%rbp), %ecx
	movl	%ebx, %eax
	cltd
	idivl	%ecx
	movl	%edx, %ebx
	movl	%ebx, %eax
	jmp	.Lrem_register.1
.Lrem_register.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	rem_register, .-rem_register
	.globl	rem_local
	.type	rem_local, @function
rem_local:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -12(%rbp)
	movl	-8(%rbp), %ecx
	movl	-12(%rbp), %eax
	cltd
	idivl	%ecx
	movl	%edx, -12(%rbp)
	movl	-12(%rbp), %eax
	jmp	.Lrem_local.1
.Lrem_local.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	rem_local, .-rem_local
	.globl	rem_arm
	.type	rem_arm, @function
rem_arm:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	%edx, -12(%rbp)
	cmpl	$0, -4(%rbp)
	je	.Lrem_arm.2
	movl	-8(%rbp), %eax
	cltd
	idivl	-12(%rbp)
	movl	%edx, %eax
	jmp	.Lrem_arm.3
.Lrem_arm.2:
	movl	-12(%rbp), %eax
.Lrem_arm.3:
	jmp	.Lrem_arm.1
.Lrem_arm.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	rem_arm, .-rem_arm
	.section	.note.GNU-stack,"",@progbits
