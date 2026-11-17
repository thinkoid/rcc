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
	movq	-24(%rbp), %rax
	imulq	$4, %rax, %rax
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
	.section	.note.GNU-stack,"",@progbits
