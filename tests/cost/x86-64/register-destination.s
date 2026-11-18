	.file	"register-destination.c"
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
	pushq	%rbx
	.cfi_offset %rbx, -24
	movl	%edi, -12(%rbp)
	movl	-12(%rbp), %ebx
	movl	%ebx, %eax
	jmp	.Lload.1
.Lload.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	load, .-load
	.globl	constant_leaf
	.type	constant_leaf, @function
constant_leaf:
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
	addl	$5, %ebx
	movl	%ebx, %eax
	jmp	.Lconstant_leaf.1
.Lconstant_leaf.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	constant_leaf, .-constant_leaf
	.globl	named_leaf
	.type	named_leaf, @function
named_leaf:
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
	addl	-16(%rbp), %ebx
	movl	%ebx, %eax
	jmp	.Lnamed_leaf.1
.Lnamed_leaf.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	named_leaf, .-named_leaf
	.globl	register_leaf
	.type	register_leaf, @function
register_leaf:
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
	movl	%edi, %ebx
	movl	%esi, -20(%rbp)
	movl	-20(%rbp), %r12d
	addl	%ebx, %r12d
	movl	%r12d, %eax
	jmp	.Lregister_leaf.1
.Lregister_leaf.1:
	popq	%r12
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	register_leaf, .-register_leaf
	.globl	pointer_leaf
	.type	pointer_leaf, @function
pointer_leaf:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movl	%edi, -12(%rbp)
	movq	%rsi, -24(%rbp)
	movl	-12(%rbp), %ebx
	movq	-24(%rbp), %rax
	addl	(%rax), %ebx
	movl	%ebx, %eax
	jmp	.Lpointer_leaf.1
.Lpointer_leaf.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	pointer_leaf, .-pointer_leaf
	.globl	promoted_leaf
	.type	promoted_leaf, @function
promoted_leaf:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movl	%edi, -12(%rbp)
	movb	%sil, -13(%rbp)
	movl	-12(%rbp), %ebx
	movsbl	-13(%rbp), %eax
	addl	%eax, %ebx
	movl	%ebx, %eax
	jmp	.Lpromoted_leaf.1
.Lpromoted_leaf.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	promoted_leaf, .-promoted_leaf
	.globl	nested
	.type	nested, @function
nested:
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
	movl	%edx, -20(%rbp)
	movl	-12(%rbp), %eax
	movl	-16(%rbp), %ecx
	addl	-20(%rbp), %ecx
	addl	%ecx, %eax
	movl	%eax, %ebx
	movl	%ebx, %eax
	jmp	.Lnested.1
.Lnested.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	nested, .-nested
	.globl	named
	.type	named, @function
named:
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
	movl	-16(%rbp), %ebx
	movl	-12(%rbp), %eax
	subl	%ebx, %eax
	movl	%eax, %ebx
	movl	%ebx, %eax
	jmp	.Lnamed.1
.Lnamed.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	named, .-named
	.globl	itself
	.type	itself, @function
itself:
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
	addl	-16(%rbp), %ebx
	movl	%ebx, %eax
	jmp	.Litself.1
.Litself.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	itself, .-itself
	.globl	itself_left
	.type	itself_left, @function
itself_left:
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
	addl	$1, %ebx
	addl	-16(%rbp), %ebx
	movl	%ebx, %eax
	jmp	.Litself_left.1
.Litself_left.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	itself_left, .-itself_left
	.globl	itself_assigned
	.type	itself_assigned, @function
itself_assigned:
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
	movl	%ebx, %eax
	jmp	.Litself_assigned.1
.Litself_assigned.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	itself_assigned, .-itself_assigned
	.globl	quotient
	.type	quotient, @function
quotient:
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
	movl	-12(%rbp), %eax
	cltd
	idivl	-16(%rbp)
	movl	%eax, %ebx
	movl	%ebx, %eax
	jmp	.Lquotient.1
.Lquotient.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	quotient, .-quotient
	.globl	initializer
	.type	initializer, @function
initializer:
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
	imull	-16(%rbp), %ebx
	movl	%ebx, %eax
	jmp	.Linitializer.1
.Linitializer.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	initializer, .-initializer
	.globl	address_global
	.type	address_global, @function
address_global:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	leaq	defined(%rip), %rbx
	movq	%rbx, %rax
	jmp	.Laddress_global.1
.Laddress_global.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	address_global, .-address_global
	.globl	address_declared
	.type	address_declared, @function
address_declared:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movq	declared@GOTPCREL(%rip), %rbx
	movq	%rbx, %rax
	jmp	.Laddress_declared.1
.Laddress_declared.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	address_declared, .-address_declared
	.globl	address_local
	.type	address_local, @function
address_local:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movl	%edi, -12(%rbp)
	leaq	-12(%rbp), %rbx
	movl	(%rbx), %eax
	jmp	.Laddress_local.1
.Laddress_local.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	address_local, .-address_local
	.globl	address_element
	.type	address_element, @function
address_element:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movl	%edi, -12(%rbp)
	movslq	-12(%rbp), %rax
	leaq	-28(%rbp,%rax,4), %rbx
	movl	(%rbx), %eax
	jmp	.Laddress_element.1
.Laddress_element.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	address_element, .-address_element
	.globl	pointer_sum
	.type	pointer_sum, @function
pointer_sum:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movq	%rdi, -16(%rbp)
	movq	%rsi, -24(%rbp)
	movq	-16(%rbp), %rax
	movq	-24(%rbp), %rcx
	leaq	(%rax,%rcx,4), %rbx
	movq	%rbx, %rax
	jmp	.Lpointer_sum.1
.Lpointer_sum.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	pointer_sum, .-pointer_sum
	.globl	pointer_step
	.type	pointer_step, @function
pointer_step:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movq	%rdi, -16(%rbp)
	movq	-16(%rbp), %rbx
	addq	$4, %rbx
	movq	%rbx, %rax
	jmp	.Lpointer_step.1
.Lpointer_step.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	pointer_step, .-pointer_step
	.globl	pointer_back
	.type	pointer_back, @function
pointer_back:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movq	%rdi, -16(%rbp)
	movq	%rsi, -24(%rbp)
	movq	-16(%rbp), %rbx
	imulq	$4, -24(%rbp), %rax
	subq	%rax, %rbx
	movq	%rbx, %rax
	jmp	.Lpointer_back.1
.Lpointer_back.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	pointer_back, .-pointer_back
	.globl	pointer_before
	.type	pointer_before, @function
pointer_before:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movq	%rdi, -16(%rbp)
	movq	-16(%rbp), %rbx
	subq	$4, %rbx
	movq	%rbx, %rax
	jmp	.Lpointer_before.1
.Lpointer_before.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	pointer_before, .-pointer_before
	.globl	negated
	.type	negated, @function
negated:
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
	negl	%ebx
	movl	%ebx, %eax
	jmp	.Lnegated.1
.Lnegated.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	negated, .-negated
	.globl	complemented
	.type	complemented, @function
complemented:
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
	notl	%ebx
	movl	%ebx, %eax
	jmp	.Lcomplemented.1
.Lcomplemented.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	complemented, .-complemented
	.globl	negated_itself
	.type	negated_itself, @function
negated_itself:
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
	negl	%ebx
	movl	%ebx, %eax
	jmp	.Lnegated_itself.1
.Lnegated_itself.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	negated_itself, .-negated_itself
	.globl	compared
	.type	compared, @function
compared:
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
	movl	-12(%rbp), %eax
	cmpl	-16(%rbp), %eax
	setl	%bl
	movzbl	%bl, %ebx
	movl	%ebx, %eax
	jmp	.Lcompared.1
.Lcompared.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	compared, .-compared
	.globl	compared_itself
	.type	compared_itself, @function
compared_itself:
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
	cmpl	-16(%rbp), %ebx
	setl	%bl
	movzbl	%bl, %ebx
	movl	%ebx, %eax
	jmp	.Lcompared_itself.1
.Lcompared_itself.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	compared_itself, .-compared_itself
	.globl	compared_constant
	.type	compared_constant, @function
compared_constant:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movl	%edi, -12(%rbp)
	cmpl	$5, -12(%rbp)
	sete	%bl
	movzbl	%bl, %ebx
	movl	%ebx, %eax
	jmp	.Lcompared_constant.1
.Lcompared_constant.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	compared_constant, .-compared_constant
	.globl	element_leaf
	.type	element_leaf, @function
element_leaf:
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
	addl	table+12(%rip), %ebx
	movl	%ebx, %eax
	jmp	.Lelement_leaf.1
.Lelement_leaf.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	element_leaf, .-element_leaf
	.globl	pointer_element_leaf
	.type	pointer_element_leaf, @function
pointer_element_leaf:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movl	%edi, -12(%rbp)
	movq	%rsi, -24(%rbp)
	movl	-12(%rbp), %ebx
	movq	-24(%rbp), %rax
	addl	12(%rax), %ebx
	movl	%ebx, %eax
	jmp	.Lpointer_element_leaf.1
.Lpointer_element_leaf.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	pointer_element_leaf, .-pointer_element_leaf
	.globl	named_right
	.type	named_right, @function
named_right:
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
	movl	-16(%rbp), %ebx
	movl	-12(%rbp), %eax
	addl	%ebx, %eax
	movl	%eax, %ebx
	movl	%ebx, %eax
	jmp	.Lnamed_right.1
.Lnamed_right.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	named_right, .-named_right
	.globl	own_element
	.type	own_element, @function
own_element:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movq	%rdi, -16(%rbp)
	movq	-16(%rbp), %rbx
	movq	-16(%rbp), %rax
	movsbq	1(%rbx), %rcx
	addq	%rcx, %rax
	movq	%rax, %rbx
	movq	%rbx, %rax
	jmp	.Lown_element.1
.Lown_element.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	own_element, .-own_element
	.globl	step_char
	.type	step_char, @function
step_char:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movq	%rdi, -16(%rbp)
	movq	%rsi, -24(%rbp)
	movq	-16(%rbp), %rbx
	movq	-24(%rbp), %rax
	addq	%rax, %rbx
	movq	%rbx, %rax
	jmp	.Lstep_char.1
.Lstep_char.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	step_char, .-step_char
	.globl	back_char
	.type	back_char, @function
back_char:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movq	%rdi, -16(%rbp)
	movq	%rsi, -24(%rbp)
	movq	-16(%rbp), %rbx
	movq	-24(%rbp), %rax
	subq	%rax, %rbx
	movq	%rbx, %rax
	jmp	.Lback_char.1
.Lback_char.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	back_char, .-back_char
	.globl	step_int
	.type	step_int, @function
step_int:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movq	%rdi, -16(%rbp)
	movq	%rsi, -24(%rbp)
	movq	-16(%rbp), %rbx
	movq	-24(%rbp), %rax
	leaq	(%rbx,%rax,4), %rbx
	movq	%rbx, %rax
	jmp	.Lstep_int.1
.Lstep_int.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	step_int, .-step_int
	.globl	back_int
	.type	back_int, @function
back_int:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movq	%rdi, -16(%rbp)
	movq	%rsi, -24(%rbp)
	movq	-16(%rbp), %rbx
	imulq	$4, -24(%rbp), %rax
	subq	%rax, %rbx
	movq	%rbx, %rax
	jmp	.Lback_int.1
.Lback_int.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	back_int, .-back_int
	.globl	step_row
	.type	step_row, @function
step_row:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movq	%rdi, -16(%rbp)
	movq	%rsi, -24(%rbp)
	movq	-16(%rbp), %rbx
	imulq	$12, -24(%rbp), %rax
	addq	%rax, %rbx
	movq	%rbx, %rax
	jmp	.Lstep_row.1
.Lstep_row.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	step_row, .-step_row
	.globl	back_row
	.type	back_row, @function
back_row:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movq	%rdi, -16(%rbp)
	movq	%rsi, -24(%rbp)
	movq	-16(%rbp), %rbx
	imulq	$12, -24(%rbp), %rax
	subq	%rax, %rbx
	movq	%rbx, %rax
	jmp	.Lback_row.1
.Lback_row.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	back_row, .-back_row
	.globl	defined_function
	.type	defined_function, @function
defined_function:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	$0, %eax
	jmp	.Ldefined_function.1
.Ldefined_function.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	defined_function, .-defined_function
	.globl	function_defined
	.type	function_defined, @function
function_defined:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	leaq	defined_function(%rip), %rbx
	movq	%rbx, %rax
	jmp	.Lfunction_defined.1
.Lfunction_defined.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	function_defined, .-function_defined
	.globl	function_declared
	.type	function_declared, @function
function_declared:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movq	declared_function@GOTPCREL(%rip), %rbx
	movq	%rbx, %rax
	jmp	.Lfunction_declared.1
.Lfunction_declared.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	function_declared, .-function_declared
	.globl	address_far
	.type	address_far, @function
address_far:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	subq	$8008, %rsp
	leaq	-2008(%rbp), %rbx
	leaq	-8008(%rbp), %rcx
	movq	%rbx, %rax
	subq	%rcx, %rax
	movq	$4, %rcx
	cqto
	idivq	%rcx
	jmp	.Laddress_far.1
.Laddress_far.1:
	leaq	-8(%rbp), %rsp
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	address_far, .-address_far
	.globl	signedness_load
	.type	signedness_load, @function
signedness_load:
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
	movl	%ebx, %eax
	jmp	.Lsignedness_load.1
.Lsignedness_load.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	signedness_load, .-signedness_load
	.globl	signedness_char
	.type	signedness_char, @function
signedness_char:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movb	%dil, -9(%rbp)
	movsbl	-9(%rbp), %ebx
	movl	%ebx, %eax
	jmp	.Lsignedness_char.1
.Lsignedness_char.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	signedness_char, .-signedness_char
	.globl	signedness_byte
	.type	signedness_byte, @function
signedness_byte:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movb	%dil, -9(%rbp)
	movzbl	-9(%rbp), %ebx
	movl	%ebx, %eax
	jmp	.Lsignedness_byte.1
.Lsignedness_byte.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	signedness_byte, .-signedness_byte
	.globl	signedness_sum
	.type	signedness_sum, @function
signedness_sum:
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
	addl	-16(%rbp), %ebx
	movl	%ebx, %eax
	jmp	.Lsignedness_sum.1
.Lsignedness_sum.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	signedness_sum, .-signedness_sum
	.globl	signedness_unsigned_sum
	.type	signedness_unsigned_sum, @function
signedness_unsigned_sum:
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
	addl	-16(%rbp), %ebx
	movl	%ebx, %eax
	jmp	.Lsignedness_unsigned_sum.1
.Lsignedness_unsigned_sum.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	signedness_unsigned_sum, .-signedness_unsigned_sum
	.globl	signedness_compare
	.type	signedness_compare, @function
signedness_compare:
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
	movl	-12(%rbp), %eax
	cmpl	-16(%rbp), %eax
	setl	%bl
	movzbl	%bl, %ebx
	movl	%ebx, %eax
	jmp	.Lsignedness_compare.1
.Lsignedness_compare.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	signedness_compare, .-signedness_compare
	.globl	signedness_narrowed
	.type	signedness_narrowed, @function
signedness_narrowed:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movq	%rdi, -16(%rbp)
	movl	%ebx, %ebx
	movq	-16(%rbp), %rax
	movl	%eax, %ebx
	movl	%ebx, %eax
	jmp	.Lsignedness_narrowed.1
.Lsignedness_narrowed.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	signedness_narrowed, .-signedness_narrowed
	.globl	signedness_widened
	.type	signedness_widened, @function
signedness_widened:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movb	%dil, -9(%rbp)
	movsbl	-9(%rbp), %eax
	movl	%eax, %ebx
	movl	%ebx, %eax
	jmp	.Lsignedness_widened.1
.Lsignedness_widened.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	signedness_widened, .-signedness_widened
	.globl	decay_sum
	.type	decay_sum, @function
decay_sum:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movq	%rdi, -16(%rbp)
	leaq	characters(%rip), %rbx
	addq	-16(%rbp), %rbx
	movq	%rbx, %rax
	jmp	.Ldecay_sum.1
.Ldecay_sum.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	decay_sum, .-decay_sum
	.globl	decay_declared
	.type	decay_declared, @function
decay_declared:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movq	%rdi, -16(%rbp)
	movq	declared@GOTPCREL(%rip), %rax
	movq	-16(%rbp), %rcx
	leaq	(%rax,%rcx,4), %rbx
	movq	%rbx, %rax
	jmp	.Ldecay_declared.1
.Ldecay_declared.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	decay_declared, .-decay_declared
	.globl	decay_row
	.type	decay_row, @function
decay_row:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movq	%rdi, -16(%rbp)
	leaq	rows(%rip), %rbx
	imulq	$12, -16(%rbp), %rax
	addq	%rax, %rbx
	movq	%rbx, %rax
	jmp	.Ldecay_row.1
.Ldecay_row.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	decay_row, .-decay_row
	.globl	decay_twice
	.type	decay_twice, @function
decay_twice:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movq	%rdi, -16(%rbp)
	leaq	characters(%rip), %rbx
	addq	-16(%rbp), %rbx
	addq	-16(%rbp), %rbx
	movq	%rbx, %rax
	jmp	.Ldecay_twice.1
.Ldecay_twice.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	decay_twice, .-decay_twice
	.globl	decay_named
	.type	decay_named, @function
decay_named:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movq	%rdi, -16(%rbp)
	movq	-16(%rbp), %rbx
	leaq	characters(%rip), %rax
	movsbq	(%rbx), %rcx
	addq	%rcx, %rax
	movq	%rax, %rbx
	movq	%rbx, %rax
	jmp	.Ldecay_named.1
.Ldecay_named.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	decay_named, .-decay_named
	.globl	own_value
	.type	own_value, @function
own_value:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movq	%rdi, -16(%rbp)
	movq	-16(%rbp), %rbx
	movq	-16(%rbp), %rbx
	movq	%rbx, %rax
	jmp	.Lown_value.1
.Lown_value.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	own_value, .-own_value
	.globl	element_row
	.type	element_row, @function
element_row:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movq	%rdi, -16(%rbp)
	leaq	rows(%rip), %rbx
	imulq	$12, -16(%rbp), %rax
	addq	%rax, %rbx
	movq	%rbx, %rax
	jmp	.Lelement_row.1
.Lelement_row.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	element_row, .-element_row
	.globl	not_int
	.type	not_int, @function
not_int:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movl	%edi, -12(%rbp)
	movl	-12(%rbp), %eax
	cmpl	$0, %eax
	sete	%bl
	movzbl	%bl, %ebx
	movl	%ebx, %eax
	jmp	.Lnot_int.1
.Lnot_int.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	not_int, .-not_int
	.globl	not_long
	.type	not_long, @function
not_long:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movq	%rdi, -16(%rbp)
	movq	-16(%rbp), %rax
	cmpq	$0, %rax
	sete	%bl
	movzbl	%bl, %ebx
	movq	%rbx, %rax
	jmp	.Lnot_long.1
.Lnot_long.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	not_long, .-not_long
	.globl	not_char
	.type	not_char, @function
not_char:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movl	%edi, -12(%rbp)
	movsbl	%bl, %ebx
	movl	-12(%rbp), %eax
	cmpl	$0, %eax
	sete	%al
	movzbl	%al, %eax
	movsbl	%al, %ebx
	movl	%ebx, %eax
	jmp	.Lnot_char.1
.Lnot_char.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	not_char, .-not_char
	.globl	step_value
	.type	step_value, @function
step_value:
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
	movl	%ebx, %eax
	addl	$1, %eax
	movl	%eax, -12(%rbp)
	addl	-12(%rbp), %ebx
	movl	-12(%rbp), %ebx
	movl	%ebx, %eax
	subl	$1, %eax
	movl	%eax, -12(%rbp)
	movl	%ebx, %eax
	addl	-12(%rbp), %eax
	jmp	.Lstep_value.1
.Lstep_value.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	step_value, .-step_value
	.globl	step_pointer
	.type	step_pointer, @function
step_pointer:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movq	%rdi, -16(%rbp)
	movq	-16(%rbp), %rax
	movl	(%rax), %ebx
	movl	%ebx, %ecx
	addl	$1, %ecx
	movl	%ecx, (%rax)
	movl	%ebx, %eax
	jmp	.Lstep_pointer.1
.Lstep_pointer.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	step_pointer, .-step_pointer
	.globl	step_register
	.type	step_register, @function
step_register:
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
	movl	-20(%rbp), %ebx
	movl	%ebx, %r12d
	addl	$1, %ebx
	movl	%r12d, %eax
	addl	%ebx, %eax
	jmp	.Lstep_register.1
.Lstep_register.1:
	popq	%r12
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	step_register, .-step_register
	.globl	step_indexed
	.type	step_indexed, @function
step_indexed:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movq	%rdi, -16(%rbp)
	movq	%rsi, -24(%rbp)
	movq	-24(%rbp), %rbx
	movq	-16(%rbp), %rax
	movq	(%rax,%rbx,8), %rcx
	movq	%rcx, %rdx
	addq	$1, %rcx
	movq	%rcx, (%rax,%rbx,8)
	movq	%rdx, %rbx
	movq	%rbx, %rax
	jmp	.Lstep_indexed.1
.Lstep_indexed.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	step_indexed, .-step_indexed
	.globl	step_widened
	.type	step_widened, @function
step_widened:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movl	%edi, -12(%rbp)
	movl	-12(%rbp), %eax
	movl	%eax, %ecx
	addl	$1, %eax
	movl	%eax, -12(%rbp)
	movslq	%ecx, %rbx
	movslq	-12(%rbp), %rcx
	movq	%rbx, %rax
	addq	%rcx, %rax
	jmp	.Lstep_widened.1
.Lstep_widened.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	step_widened, .-step_widened
	.globl	mixed_sum
	.type	mixed_sum, @function
mixed_sum:
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
	addl	-16(%rbp), %ebx
	movl	%ebx, %eax
	jmp	.Lmixed_sum.1
.Lmixed_sum.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	mixed_sum, .-mixed_sum
	.globl	promoted_sum
	.type	promoted_sum, @function
promoted_sum:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movb	%dil, -9(%rbp)
	movl	%esi, -16(%rbp)
	movl	%ebx, %ebx
	movsbl	-9(%rbp), %ebx
	addl	-16(%rbp), %ebx
	movl	%ebx, %eax
	jmp	.Lpromoted_sum.1
.Lpromoted_sum.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	promoted_sum, .-promoted_sum
	.globl	narrowed_sum
	.type	narrowed_sum, @function
narrowed_sum:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movq	%rdi, -16(%rbp)
	movl	%esi, -20(%rbp)
	movl	%ebx, %ebx
	movq	-16(%rbp), %rax
	addl	-20(%rbp), %eax
	movl	%eax, %ebx
	movl	%ebx, %eax
	jmp	.Lnarrowed_sum.1
.Lnarrowed_sum.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	narrowed_sum, .-narrowed_sum
	.globl	logical_and
	.type	logical_and, @function
logical_and:
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
	cmpl	$0, -12(%rbp)
	je	.Llogical_and.2
	cmpl	$0, -16(%rbp)
	je	.Llogical_and.2
	movl	$1, %ebx
	jmp	.Llogical_and.3
.Llogical_and.2:
	movl	$0, %ebx
.Llogical_and.3:
	movl	%ebx, %eax
	jmp	.Llogical_and.1
.Llogical_and.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	logical_and, .-logical_and
	.globl	logical_or
	.type	logical_or, @function
logical_or:
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
	cmpl	$0, -12(%rbp)
	jne	.Llogical_or.4
	cmpl	$0, -16(%rbp)
	je	.Llogical_or.2
.Llogical_or.4:
	movl	$1, %ebx
	jmp	.Llogical_or.3
.Llogical_or.2:
	movl	$0, %ebx
.Llogical_or.3:
	movl	%ebx, %eax
	jmp	.Llogical_or.1
.Llogical_or.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	logical_or, .-logical_or
	.globl	logical_itself
	.type	logical_itself, @function
logical_itself:
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
	cmpl	$0, %ebx
	je	.Llogical_itself.2
	cmpl	$0, -16(%rbp)
	je	.Llogical_itself.2
	movl	$1, %ebx
	jmp	.Llogical_itself.3
.Llogical_itself.2:
	movl	$0, %ebx
.Llogical_itself.3:
	movl	%ebx, %eax
	jmp	.Llogical_itself.1
.Llogical_itself.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	logical_itself, .-logical_itself
	.globl	chosen
	.type	chosen, @function
chosen:
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
	movl	%edx, -20(%rbp)
	cmpl	$0, -12(%rbp)
	je	.Lchosen.2
	movl	-16(%rbp), %ebx
	jmp	.Lchosen.3
.Lchosen.2:
	movl	-20(%rbp), %ebx
.Lchosen.3:
	movl	%ebx, %eax
	jmp	.Lchosen.1
.Lchosen.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	chosen, .-chosen
	.globl	chosen_reading
	.type	chosen_reading, @function
chosen_reading:
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
	movl	-16(%rbp), %ebx
	cmpl	$0, -12(%rbp)
	je	.Lchosen_reading.2
	addl	$1, %ebx
	jmp	.Lchosen_reading.3
.Lchosen_reading.2:
	movl	$0, %ebx
.Lchosen_reading.3:
	movl	%ebx, %eax
	jmp	.Lchosen_reading.1
.Lchosen_reading.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	chosen_reading, .-chosen_reading
	.globl	chosen_called
	.type	chosen_called, @function
chosen_called:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	subq	$24, %rsp
	movl	%edi, -12(%rbp)
	movl	%esi, -16(%rbp)
	cmpl	$0, -12(%rbp)
	je	.Lchosen_called.2
	movl	-16(%rbp), %eax
	movl	%eax, -20(%rbp)
	movl	-20(%rbp), %edi
	call	f@PLT
	movl	%eax, %ebx
	jmp	.Lchosen_called.3
.Lchosen_called.2:
	movl	$0, %ebx
.Lchosen_called.3:
	movl	%ebx, %eax
	jmp	.Lchosen_called.1
.Lchosen_called.1:
	leaq	-8(%rbp), %rsp
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	chosen_called, .-chosen_called
	.globl	chosen_widened
	.type	chosen_widened, @function
chosen_widened:
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
	movl	%edx, -20(%rbp)
	cmpl	$0, -12(%rbp)
	je	.Lchosen_widened.2
	movslq	-16(%rbp), %rbx
	jmp	.Lchosen_widened.3
.Lchosen_widened.2:
	movslq	-20(%rbp), %rbx
.Lchosen_widened.3:
	movq	%rbx, %rax
	jmp	.Lchosen_widened.1
.Lchosen_widened.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	chosen_widened, .-chosen_widened
	.globl	chosen_stepped
	.type	chosen_stepped, @function
chosen_stepped:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	subq	$24, %rsp
	movl	%edi, -12(%rbp)
	movl	%esi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	movl	%ecx, -28(%rbp)
	cmpl	$0, -12(%rbp)
	je	.Lchosen_stepped.2
	movl	-16(%rbp), %eax
	movl	%eax, -32(%rbp)
	movl	-32(%rbp), %edi
	call	f@PLT
	movl	%eax, %ebx
	jmp	.Lchosen_stepped.3
.Lchosen_stepped.2:
	movq	-24(%rbp), %rax
	movslq	-28(%rbp), %rcx
	movl	(%rax,%rcx,4), %ebx
	movl	%ebx, %edx
	addl	$1, %edx
	movl	%edx, (%rax,%rcx,4)
.Lchosen_stepped.3:
	movl	%ebx, %eax
	jmp	.Lchosen_stepped.1
.Lchosen_stepped.1:
	leaq	-8(%rbp), %rsp
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	chosen_stepped, .-chosen_stepped
	.globl	chosen_divided
	.type	chosen_divided, @function
chosen_divided:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	subq	$24, %rsp
	movl	%edi, -12(%rbp)
	movl	%esi, -16(%rbp)
	movl	%edx, -20(%rbp)
	movl	%ecx, -24(%rbp)
	cmpl	$0, -12(%rbp)
	je	.Lchosen_divided.2
	movl	-16(%rbp), %eax
	movl	%eax, -28(%rbp)
	movl	-28(%rbp), %edi
	call	f@PLT
	jmp	.Lchosen_divided.3
.Lchosen_divided.2:
	movl	-20(%rbp), %eax
	cltd
	idivl	-24(%rbp)
.Lchosen_divided.3:
	movl	%eax, %ebx
	movl	%ebx, %eax
	jmp	.Lchosen_divided.1
.Lchosen_divided.1:
	leaq	-8(%rbp), %rsp
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	chosen_divided, .-chosen_divided
	.globl	chosen_constant
	.type	chosen_constant, @function
chosen_constant:
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
	cmpl	$0, -12(%rbp)
	je	.Lchosen_constant.2
	movslq	-16(%rbp), %rbx
	jmp	.Lchosen_constant.3
.Lchosen_constant.2:
	movq	$-1, %rbx
.Lchosen_constant.3:
	movq	%rbx, %rax
	jmp	.Lchosen_constant.1
.Lchosen_constant.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	chosen_constant, .-chosen_constant
	.globl	chosen_promoted
	.type	chosen_promoted, @function
chosen_promoted:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movl	%edi, -12(%rbp)
	movb	%sil, -13(%rbp)
	movb	%dl, -14(%rbp)
	cmpl	$0, -12(%rbp)
	je	.Lchosen_promoted.2
	movsbq	-13(%rbp), %rbx
	jmp	.Lchosen_promoted.3
.Lchosen_promoted.2:
	movsbq	-14(%rbp), %rbx
.Lchosen_promoted.3:
	movq	%rbx, %rax
	jmp	.Lchosen_promoted.1
.Lchosen_promoted.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	chosen_promoted, .-chosen_promoted
	.globl	chosen_calls
	.type	chosen_calls, @function
chosen_calls:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	subq	$24, %rsp
	movl	%edi, -12(%rbp)
	movl	%esi, -16(%rbp)
	cmpl	$0, -12(%rbp)
	je	.Lchosen_calls.2
	movl	-16(%rbp), %eax
	movl	%eax, -20(%rbp)
	movl	-20(%rbp), %edi
	call	f@PLT
	jmp	.Lchosen_calls.3
.Lchosen_calls.2:
	movl	-16(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -24(%rbp)
	movl	-24(%rbp), %edi
	call	f@PLT
.Lchosen_calls.3:
	movl	%eax, %ebx
	movl	%ebx, %eax
	jmp	.Lchosen_calls.1
.Lchosen_calls.1:
	leaq	-8(%rbp), %rsp
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	chosen_calls, .-chosen_calls
	.globl	chosen_own_first
	.type	chosen_own_first, @function
chosen_own_first:
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
	movl	-16(%rbp), %ebx
	cmpl	$0, -12(%rbp)
	jne	.Lchosen_own_first.3
	movl	-16(%rbp), %ebx
.Lchosen_own_first.3:
	movl	%ebx, %eax
	jmp	.Lchosen_own_first.1
.Lchosen_own_first.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	chosen_own_first, .-chosen_own_first
	.globl	chosen_own_second
	.type	chosen_own_second, @function
chosen_own_second:
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
	movl	-16(%rbp), %ebx
	cmpl	$0, -12(%rbp)
	je	.Lchosen_own_second.3
	movl	-16(%rbp), %ebx
.Lchosen_own_second.3:
	movl	%ebx, %eax
	jmp	.Lchosen_own_second.1
.Lchosen_own_second.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	chosen_own_second, .-chosen_own_second
	.globl	chosen_own_pointer
	.type	chosen_own_pointer, @function
chosen_own_pointer:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movl	%edi, -12(%rbp)
	movq	%rsi, -24(%rbp)
	movq	-24(%rbp), %rbx
	cmpl	$0, -12(%rbp)
	je	.Lchosen_own_pointer.3
	movq	-24(%rbp), %rbx
.Lchosen_own_pointer.3:
	movq	%rbx, %rax
	jmp	.Lchosen_own_pointer.1
.Lchosen_own_pointer.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	chosen_own_pointer, .-chosen_own_pointer
	.globl	chosen_own_called
	.type	chosen_own_called, @function
chosen_own_called:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	subq	$24, %rsp
	movl	%edi, -12(%rbp)
	movl	%esi, -16(%rbp)
	movl	-16(%rbp), %ebx
	cmpl	$0, -12(%rbp)
	je	.Lchosen_own_called.3
	movl	-16(%rbp), %eax
	movl	%eax, -20(%rbp)
	movl	-20(%rbp), %edi
	call	f@PLT
	movl	%eax, %ebx
.Lchosen_own_called.3:
	movl	%ebx, %eax
	jmp	.Lchosen_own_called.1
.Lchosen_own_called.1:
	leaq	-8(%rbp), %rsp
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	chosen_own_called, .-chosen_own_called
	.globl	chosen_own_call_first
	.type	chosen_own_call_first, @function
chosen_own_call_first:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	subq	$24, %rsp
	movl	%edi, -12(%rbp)
	movl	%esi, -16(%rbp)
	movl	-16(%rbp), %ebx
	cmpl	$0, -12(%rbp)
	jne	.Lchosen_own_call_first.3
	movl	-16(%rbp), %eax
	movl	%eax, -20(%rbp)
	movl	-20(%rbp), %edi
	call	f@PLT
	movl	%eax, %ebx
.Lchosen_own_call_first.3:
	movl	%ebx, %eax
	jmp	.Lchosen_own_call_first.1
.Lchosen_own_call_first.1:
	leaq	-8(%rbp), %rsp
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	chosen_own_call_first, .-chosen_own_call_first
	.globl	chosen_own_both
	.type	chosen_own_both, @function
chosen_own_both:
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
	movl	-16(%rbp), %ebx
	cmpl	$0, -12(%rbp)
	jne	.Lchosen_own_both.3
.Lchosen_own_both.3:
	movl	%ebx, %eax
	jmp	.Lchosen_own_both.1
.Lchosen_own_both.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	chosen_own_both, .-chosen_own_both
	.globl	chosen_own_and
	.type	chosen_own_and, @function
chosen_own_and:
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
	movl	%edx, -20(%rbp)
	movl	-20(%rbp), %ebx
	cmpl	$0, -12(%rbp)
	je	.Lchosen_own_and.4
	cmpl	$0, -16(%rbp)
	jne	.Lchosen_own_and.3
.Lchosen_own_and.4:
	movl	$0, %ebx
.Lchosen_own_and.3:
	movl	%ebx, %eax
	jmp	.Lchosen_own_and.1
.Lchosen_own_and.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	chosen_own_and, .-chosen_own_and
	.globl	chosen_own_or
	.type	chosen_own_or, @function
chosen_own_or:
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
	movl	%edx, -20(%rbp)
	movl	-20(%rbp), %ebx
	cmpl	$0, -12(%rbp)
	jne	.Lchosen_own_or.3
	cmpl	$0, -16(%rbp)
	jne	.Lchosen_own_or.3
	movl	$0, %ebx
.Lchosen_own_or.3:
	movl	%ebx, %eax
	jmp	.Lchosen_own_or.1
.Lchosen_own_or.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	chosen_own_or, .-chosen_own_or
	.globl	chosen_own_nested
	.type	chosen_own_nested, @function
chosen_own_nested:
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
	movl	%edx, -20(%rbp)
	movl	-20(%rbp), %ebx
	cmpl	$0, -12(%rbp)
	je	.Lchosen_own_nested.2
	cmpl	$0, -16(%rbp)
	jne	.Lchosen_own_nested.5
	movl	$0, %ebx
.Lchosen_own_nested.5:
	jmp	.Lchosen_own_nested.3
.Lchosen_own_nested.2:
	movl	$1, %ebx
.Lchosen_own_nested.3:
	movl	%ebx, %eax
	jmp	.Lchosen_own_nested.1
.Lchosen_own_nested.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	chosen_own_nested, .-chosen_own_nested
	.globl	chosen_other
	.type	chosen_other, @function
chosen_other:
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
	movl	%edx, -28(%rbp)
	movl	-24(%rbp), %ebx
	movl	-28(%rbp), %r12d
	cmpl	$0, -20(%rbp)
	je	.Lchosen_other.3
	movl	%ebx, %r12d
.Lchosen_other.3:
	movl	%r12d, %eax
	jmp	.Lchosen_other.1
.Lchosen_other.1:
	popq	%r12
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	chosen_other, .-chosen_other
	.globl	chosen_own_retyped
	.type	chosen_own_retyped, @function
chosen_own_retyped:
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
	movl	%edx, -20(%rbp)
	movl	-16(%rbp), %ebx
	cmpl	$0, -12(%rbp)
	je	.Lchosen_own_retyped.3
	movl	-20(%rbp), %ebx
.Lchosen_own_retyped.3:
	movl	%ebx, %eax
	jmp	.Lchosen_own_retyped.1
.Lchosen_own_retyped.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	chosen_own_retyped, .-chosen_own_retyped
	.globl	chosen_own_narrow
	.type	chosen_own_narrow, @function
chosen_own_narrow:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movl	%edi, -12(%rbp)
	movb	%sil, -13(%rbp)
	movl	%edx, -20(%rbp)
	movsbl	-13(%rbp), %ebx
	cmpl	$0, -12(%rbp)
	je	.Lchosen_own_narrow.2
	movl	%ebx, %eax
	jmp	.Lchosen_own_narrow.3
.Lchosen_own_narrow.2:
	movl	-20(%rbp), %eax
.Lchosen_own_narrow.3:
	movsbl	%al, %ebx
	movl	%ebx, %eax
	jmp	.Lchosen_own_narrow.1
.Lchosen_own_narrow.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	chosen_own_narrow, .-chosen_own_narrow
	.globl	chosen_own_returned
	.type	chosen_own_returned, @function
chosen_own_returned:
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
	movl	-16(%rbp), %ebx
	cmpl	$0, -12(%rbp)
	je	.Lchosen_own_returned.2
	movl	%ebx, %eax
	jmp	.Lchosen_own_returned.3
.Lchosen_own_returned.2:
	movl	$0, %eax
.Lchosen_own_returned.3:
	jmp	.Lchosen_own_returned.1
.Lchosen_own_returned.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	chosen_own_returned, .-chosen_own_returned
	.globl	char_operand
	.type	char_operand, @function
char_operand:
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
	movb	%dil, -17(%rbp)
	movsbl	-17(%rbp), %ebx
	movl	%ebx, %r12d
	addl	$5, %r12d
	movl	%r12d, %eax
	jmp	.Lchar_operand.1
.Lchar_operand.1:
	popq	%r12
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	char_operand, .-char_operand
	.globl	char_operand_left
	.type	char_operand_left, @function
char_operand_left:
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
	movb	%dil, -17(%rbp)
	movsbl	-17(%rbp), %ebx
	movl	%ebx, %r12d
	addl	$5, %r12d
	movl	%r12d, %eax
	jmp	.Lchar_operand_left.1
.Lchar_operand_left.1:
	popq	%r12
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	char_operand_left, .-char_operand_left
	.globl	int_widened
	.type	int_widened, @function
int_widened:
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
	movl	-20(%rbp), %ebx
	movslq	%ebx, %r12
	addq	$5, %r12
	movq	%r12, %rax
	jmp	.Lint_widened.1
.Lint_widened.1:
	popq	%r12
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	int_widened, .-int_widened
	.globl	register_product
	.type	register_product, @function
register_product:
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
	movl	-20(%rbp), %ebx
	imull	$4, %ebx, %r12d
	movl	%r12d, %eax
	addl	%ebx, %eax
	jmp	.Lregister_product.1
.Lregister_product.1:
	popq	%r12
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	register_product, .-register_product
	.globl	prefix_step
	.type	prefix_step, @function
prefix_step:
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
	addl	$1, %ebx
	movl	%ebx, -12(%rbp)
	addl	-12(%rbp), %ebx
	movl	-12(%rbp), %ebx
	subl	$1, %ebx
	movl	%ebx, -12(%rbp)
	movl	%ebx, %eax
	addl	-12(%rbp), %eax
	jmp	.Lprefix_step.1
.Lprefix_step.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	prefix_step, .-prefix_step
	.globl	prefix_pointer
	.type	prefix_pointer, @function
prefix_pointer:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movq	%rdi, -16(%rbp)
	movq	-16(%rbp), %rbx
	addq	$4, %rbx
	movq	%rbx, -16(%rbp)
	movq	%rbx, %rax
	jmp	.Lprefix_pointer.1
.Lprefix_pointer.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	prefix_pointer, .-prefix_pointer
	.globl	prefix_char
	.type	prefix_char, @function
prefix_char:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movb	%dil, -9(%rbp)
	movsbl	-9(%rbp), %eax
	addl	$1, %eax
	movb	%al, -9(%rbp)
	movsbl	%al, %ebx
	movl	%ebx, %eax
	jmp	.Lprefix_char.1
.Lprefix_char.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	prefix_char, .-prefix_char
	.globl	row_element
	.type	row_element, @function
row_element:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movq	%rdi, -16(%rbp)
	leaq	rows(%rip), %rbx
	imulq	$12, -16(%rbp), %rax
	addq	%rax, %rbx
	movq	%rbx, %rax
	jmp	.Lrow_element.1
.Lrow_element.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	row_element, .-row_element
	.globl	row_decay
	.type	row_decay, @function
row_decay:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movq	%rdi, -16(%rbp)
	leaq	rows(%rip), %rbx
	imulq	$12, -16(%rbp), %rax
	addq	%rax, %rbx
	movq	%rbx, %rax
	jmp	.Lrow_decay.1
.Lrow_decay.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	row_decay, .-row_decay
	.globl	widened_sum
	.type	widened_sum, @function
widened_sum:
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
	movl	-12(%rbp), %eax
	addl	-16(%rbp), %eax
	movslq	%eax, %rbx
	movq	%rbx, %rax
	jmp	.Lwidened_sum.1
.Lwidened_sum.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	widened_sum, .-widened_sum
	.globl	widened_register
	.type	widened_register, @function
widened_register:
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
	movl	-20(%rbp), %ebx
	movslq	%ebx, %r12
	movslq	%ebx, %rcx
	movq	%r12, %rax
	addq	%rcx, %rax
	jmp	.Lwidened_register.1
.Lwidened_register.1:
	popq	%r12
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	widened_register, .-widened_register
	.globl	widened_product
	.type	widened_product, @function
widened_product:
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
	imull	-16(%rbp), %ebx
	movq	%rbx, %rax
	jmp	.Lwidened_product.1
.Lwidened_product.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	widened_product, .-widened_product
	.globl	signedness_left
	.type	signedness_left, @function
signedness_left:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movl	%edi, -12(%rbp)
	movl	%ebx, %ebx
	movl	-12(%rbp), %ebx
	addl	$5, %ebx
	movl	%ebx, %eax
	jmp	.Lsignedness_left.1
.Lsignedness_left.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	signedness_left, .-signedness_left
	.bss
	.globl	defined
	.type	defined, @object
	.size	defined, 4
	.p2align 2
defined:
	.zero	4
	.globl	table
	.type	table, @object
	.size	table, 32
	.p2align 4
table:
	.zero	32
	.globl	characters
	.type	characters, @object
	.size	characters, 16
	.p2align 4
characters:
	.zero	16
	.globl	rows
	.type	rows, @object
	.size	rows, 48
	.p2align 4
rows:
	.zero	48
	.section	.note.GNU-stack,"",@progbits
