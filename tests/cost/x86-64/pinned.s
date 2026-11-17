	.file	"pinned.c"
	.text
	.globl	fill
	.type	fill, @function
fill:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	leaq	a(%rip), %rcx
	movslq	-4(%rbp), %rsi
	movl	-8(%rbp), %eax
	shrl	$16, %eax
	movl	$256, %edi
	xorl	%edx, %edx
	divl	%edi
	movl	%edx, (%rcx,%rsi,4)
.Lfill.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	fill, .-fill
	.globl	shifted
	.type	shifted, @function
shifted:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	%edx, -12(%rbp)
	movslq	-4(%rbp), %rax
	movl	-8(%rbp), %edx
	movl	-12(%rbp), %ecx
	shll	%cl, %edx
	movl	%edx, -44(%rbp,%rax,4)
.Lshifted.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	shifted, .-shifted
	.globl	shifted_through
	.type	shifted_through, @function
shifted_through:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movl	%edx, -16(%rbp)
	movl	%ecx, -20(%rbp)
	movq	-8(%rbp), %rax
	movslq	-12(%rbp), %rdx
	movl	-16(%rbp), %esi
	movl	-20(%rbp), %ecx
	shll	%cl, %esi
	movl	%esi, (%rax,%rdx,4)
.Lshifted_through.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	shifted_through, .-shifted_through
	.globl	divided_in_place
	.type	divided_in_place, @function
divided_in_place:
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
	movslq	-12(%rbp), %rsi
	movl	-16(%rbp), %edi
	movl	(%rcx,%rsi,4), %eax
	cltd
	idivl	%edi
	movl	%eax, (%rcx,%rsi,4)
.Ldivided_in_place.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	divided_in_place, .-divided_in_place
	.globl	shifted_in_place
	.type	shifted_in_place, @function
shifted_in_place:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movl	%edx, -16(%rbp)
	movq	-8(%rbp), %rax
	movslq	-12(%rbp), %rdx
	movl	-16(%rbp), %ecx
	shll	%cl, (%rax,%rdx,4)
.Lshifted_in_place.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	shifted_in_place, .-shifted_in_place
	.globl	remainder_call
	.type	remainder_call, @function
remainder_call:
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
	movq	%rax, -16(%rbp)
	call	f@PLT
	movslq	-8(%rbp), %rcx
	movq	%rax, %rsi
	movq	-16(%rbp), %rax
	cltd
	idivl	(%rsi,%rcx,4)
	movl	%edx, %eax
	jmp	.Lremainder_call.1
.Lremainder_call.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	remainder_call, .-remainder_call
	.globl	less
	.type	less, @function
less:
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
	cmpl	%eax, %ecx
	setl	%al
	movzbl	%al, %eax
	jmp	.Lless.1
.Lless.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	less, .-less
	.globl	subscript
	.type	subscript, @function
subscript:
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
	movslq	%eax, %rax
	movl	(%rcx,%rax,4), %eax
	jmp	.Lsubscript.1
.Lsubscript.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	subscript, .-subscript
	.bss
	.globl	a
	.type	a, @object
	.size	a, 32
	.p2align 4
a:
	.zero	32
	.section	.note.GNU-stack,"",@progbits
