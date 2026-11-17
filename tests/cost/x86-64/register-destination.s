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
	movslq	%ecx, %rcx
	movq	%rcx, %rbx
	movslq	-12(%rbp), %rax
	movq	%rbx, %rcx
	addq	%rax, %rcx
	movq	%rcx, %rax
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
