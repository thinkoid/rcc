	.file	"conditional.c"
	.text
	.globl	conditional
	.type	conditional, @function
conditional:
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
	je	.Lconditional.2
	movl	-8(%rbp), %eax
	jmp	.Lconditional.3
.Lconditional.2:
	movl	-12(%rbp), %eax
.Lconditional.3:
	jmp	.Lconditional.1
.Lconditional.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	conditional, .-conditional
	.globl	stepped
	.type	stepped, @function
stepped:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movq	%rdx, -16(%rbp)
	movl	%ecx, -20(%rbp)
	cmpl	$0, -4(%rbp)
	je	.Lstepped.2
	movq	-16(%rbp), %rcx
	movslq	-20(%rbp), %rdx
	movl	(%rcx,%rdx,4), %esi
	movl	%esi, %eax
	addl	$1, %esi
	movl	%esi, (%rcx,%rdx,4)
	jmp	.Lstepped.3
.Lstepped.2:
	movl	-8(%rbp), %eax
.Lstepped.3:
	jmp	.Lstepped.1
.Lstepped.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	stepped, .-stepped
	.globl	stored
	.type	stored, @function
stored:
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
	cmpl	$0, -4(%rbp)
	je	.Lstored.2
	movl	-8(%rbp), %eax
	cltd
	idivl	-12(%rbp)
	jmp	.Lstored.3
.Lstored.2:
	movl	-16(%rbp), %edx
.Lstored.3:
	movl	%edx, g(%rip)
.Lstored.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	stored, .-stored
	.globl	summed
	.type	summed, @function
summed:
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
	cmpl	$0, -4(%rbp)
	je	.Lsummed.2
	movl	-8(%rbp), %eax
	cltd
	idivl	-12(%rbp)
	movl	%edx, %eax
	jmp	.Lsummed.3
.Lsummed.2:
	movl	-16(%rbp), %eax
.Lsummed.3:
	addl	-20(%rbp), %eax
	jmp	.Lsummed.1
.Lsummed.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	summed, .-summed
	.globl	widened
	.type	widened, @function
widened:
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
	je	.Lwidened.2
	movl	-8(%rbp), %eax
	cltd
	idivl	-12(%rbp)
	movslq	%edx, %rax
	jmp	.Lwidened.3
.Lwidened.2:
	movslq	-12(%rbp), %rax
.Lwidened.3:
	jmp	.Lwidened.1
.Lwidened.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	widened, .-widened
	.globl	widened_constants
	.type	widened_constants, @function
widened_constants:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	cmpl	$0, -4(%rbp)
	je	.Lwidened_constants.2
	movq	$4, %rax
	jmp	.Lwidened_constants.3
.Lwidened_constants.2:
	movq	$-5, %rax
.Lwidened_constants.3:
	jmp	.Lwidened_constants.1
.Lwidened_constants.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	widened_constants, .-widened_constants
	.globl	widened_quotients
	.type	widened_quotients, @function
widened_quotients:
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
	je	.Lwidened_quotients.2
	movl	-8(%rbp), %eax
	cltd
	idivl	-12(%rbp)
	jmp	.Lwidened_quotients.3
.Lwidened_quotients.2:
	movl	-12(%rbp), %eax
	cltd
	idivl	-8(%rbp)
.Lwidened_quotients.3:
	movslq	%eax, %rax
	jmp	.Lwidened_quotients.1
.Lwidened_quotients.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	widened_quotients, .-widened_quotients
	.globl	widened_stored
	.type	widened_stored, @function
widened_stored:
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
	je	.Lwidened_stored.2
	movl	-8(%rbp), %eax
	cltd
	idivl	-12(%rbp)
	movslq	%edx, %rdx
	jmp	.Lwidened_stored.3
.Lwidened_stored.2:
	movslq	-12(%rbp), %rdx
.Lwidened_stored.3:
	movq	%rdx, h(%rip)
.Lwidened_stored.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	widened_stored, .-widened_stored
	.bss
	.globl	g
	.type	g, @object
	.size	g, 4
	.p2align 2
g:
	.zero	4
	.globl	h
	.type	h, @object
	.size	h, 8
	.p2align 3
h:
	.zero	8
	.section	.note.GNU-stack,"",@progbits
