	.file	"operation-destination.c"
	.text
	.globl	constant_left
	.type	constant_left, @function
constant_left:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	pushq	%r12
	.cfi_offset %r12, -32
	movl	%edi, -20(%rbp)
	movl	%esi, -24(%rbp)
	movl	-20(%rbp), %ebx
	movl	$5, %r12d
	subl	%ebx, %r12d
	movl	%r12d, %eax
	addl	-24(%rbp), %eax
	jmp	.Lconstant_left.1
.Lconstant_left.1:
	popq	%r12
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	constant_left, .-constant_left
	.globl	constant_wide
	.type	constant_wide, @function
constant_wide:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	pushq	%r12
	.cfi_offset %r12, -32
	movl	%edi, -20(%rbp)
	movl	%esi, -24(%rbp)
	movl	-20(%rbp), %ebx
	movl	$305419896, %r12d
	subl	%ebx, %r12d
	movl	%r12d, %eax
	addl	-24(%rbp), %eax
	jmp	.Lconstant_wide.1
.Lconstant_wide.1:
	popq	%r12
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	constant_wide, .-constant_wide
	.globl	constant_itself
	.type	constant_itself, @function
constant_itself:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movl	%edi, -12(%rbp)
	movl	-12(%rbp), %ebx
	movl	$5, %eax
	subl	%ebx, %eax
	movl	%eax, %ebx
	movl	%ebx, %eax
	jmp	.Lconstant_itself.1
.Lconstant_itself.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	constant_itself, .-constant_itself
	.globl	constant_shift
	.type	constant_shift, @function
constant_shift:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	pushq	%r12
	.cfi_offset %r12, -32
	movl	%edi, -20(%rbp)
	movl	%esi, -24(%rbp)
	movl	-20(%rbp), %ebx
	movl	$5, %r12d
	movl	%ebx, %ecx
	shll	%cl, %r12d
	movl	%r12d, %eax
	addl	-24(%rbp), %eax
	jmp	.Lconstant_shift.1
.Lconstant_shift.1:
	popq	%r12
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	constant_shift, .-constant_shift
	.globl	retyped_twice
	.type	retyped_twice, @function
retyped_twice:
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
	movl	%ebx, %ebx
	movl	-12(%rbp), %ebx
	addl	$5, %ebx
	movl	%ebx, %eax
	addl	-16(%rbp), %eax
	jmp	.Lretyped_twice.1
.Lretyped_twice.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	retyped_twice, .-retyped_twice
	.globl	difference
	.type	difference, @function
difference:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	pushq	%r12
	.cfi_offset %r12, -32
	movq	%rdi, -24(%rbp)
	movq	%rsi, -32(%rbp)
	movq	-24(%rbp), %rbx
	movl	$0, -48(%rbp)
	leaq	-48(%rbp), %rcx
	movq	%rbx, %rax
	subq	%rcx, %rax
	movq	$4, %rcx
	cqto
	idivq	%rcx
	movq	%rax, %r12
	movq	%r12, %rax
	addq	-32(%rbp), %rax
	jmp	.Ldifference.1
.Ldifference.1:
	popq	%r12
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	difference, .-difference
	.globl	difference_char
	.type	difference_char, @function
difference_char:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	pushq	%r12
	.cfi_offset %r12, -32
	movq	%rdi, -24(%rbp)
	movq	%rsi, -32(%rbp)
	movq	-24(%rbp), %rbx
	movb	$0, -36(%rbp)
	leaq	-36(%rbp), %rax
	movq	%rbx, %rcx
	subq	%rax, %rcx
	movq	%rcx, %r12
	movq	%r12, %rax
	addq	-32(%rbp), %rax
	jmp	.Ldifference_char.1
.Ldifference_char.1:
	popq	%r12
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	difference_char, .-difference_char
	.section	.note.GNU-stack,"",@progbits
