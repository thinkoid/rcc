	.file	"condition-flags.c"
	.text
	.globl	add
	.type	add, @function
add:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	addl	-8(%rbp), %eax
	je	.Ladd.2
	movl	$1, %eax
	jmp	.Ladd.1
.Ladd.2:
	movl	$0, %eax
	jmp	.Ladd.1
.Ladd.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	add, .-add
	.globl	sub
	.type	sub, @function
sub:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	subl	-8(%rbp), %eax
	je	.Lsub.2
	movl	$1, %eax
	jmp	.Lsub.1
.Lsub.2:
	movl	$0, %eax
	jmp	.Lsub.1
.Lsub.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	sub, .-sub
	.globl	or
	.type	or, @function
or:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	orl	-8(%rbp), %eax
	je	.Lor.2
	movl	$1, %eax
	jmp	.Lor.1
.Lor.2:
	movl	$0, %eax
	jmp	.Lor.1
.Lor.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	or, .-or
	.globl	xor
	.type	xor, @function
xor:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	xorl	-8(%rbp), %eax
	je	.Lxor.2
	movl	$1, %eax
	jmp	.Lxor.1
.Lxor.2:
	movl	$0, %eax
	jmp	.Lxor.1
.Lxor.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	xor, .-xor
	.globl	neg
	.type	neg, @function
neg:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	negl	%eax
	je	.Lneg.2
	movl	$1, %eax
	jmp	.Lneg.1
.Lneg.2:
	movl	$0, %eax
	jmp	.Lneg.1
.Lneg.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	neg, .-neg
	.globl	test_register
	.type	test_register, @function
test_register:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	testl	-8(%rbp), %eax
	je	.Ltest_register.2
	movl	$1, %eax
	jmp	.Ltest_register.1
.Ltest_register.2:
	movl	$0, %eax
	jmp	.Ltest_register.1
.Ltest_register.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	test_register, .-test_register
	.globl	test_immediate
	.type	test_immediate, @function
test_immediate:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	testl	$1, -4(%rbp)
	je	.Ltest_immediate.2
	movl	$1, %eax
	jmp	.Ltest_immediate.1
.Ltest_immediate.2:
	movl	$0, %eax
	jmp	.Ltest_immediate.1
.Ltest_immediate.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	test_immediate, .-test_immediate
	.globl	test_variable
	.type	test_variable, @function
test_variable:
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
	movl	%eax, %ebx
	testl	-16(%rbp), %ebx
	je	.Ltest_variable.2
	movl	$1, %eax
	jmp	.Ltest_variable.1
.Ltest_variable.2:
	movl	$0, %eax
	jmp	.Ltest_variable.1
.Ltest_variable.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	test_variable, .-test_variable
	.globl	add_to_local
	.type	add_to_local, @function
add_to_local:
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
	movl	-8(%rbp), %eax
	addl	%eax, -12(%rbp)
	je	.Ladd_to_local.2
	movl	$1, %eax
	jmp	.Ladd_to_local.1
.Ladd_to_local.2:
	movl	$0, %eax
	jmp	.Ladd_to_local.1
.Ladd_to_local.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	add_to_local, .-add_to_local
	.globl	add_to_char
	.type	add_to_char, @function
add_to_char:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	movb	%al, -9(%rbp)
	movl	-8(%rbp), %eax
	addb	%al, -9(%rbp)
	je	.Ladd_to_char.2
	movl	$1, %eax
	jmp	.Ladd_to_char.1
.Ladd_to_char.2:
	movl	$0, %eax
	jmp	.Ladd_to_char.1
.Ladd_to_char.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	add_to_char, .-add_to_char
	.globl	add_to_global
	.type	add_to_global, @function
add_to_global:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	addl	%eax, counter(%rip)
	je	.Ladd_to_global.2
	movl	$1, %eax
	jmp	.Ladd_to_global.1
.Ladd_to_global.2:
	movl	$0, %eax
	jmp	.Ladd_to_global.1
.Ladd_to_global.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	add_to_global, .-add_to_global
	.globl	add_to_register
	.type	add_to_register, @function
add_to_register:
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
	movl	%eax, %ebx
	movl	-16(%rbp), %eax
	addl	%eax, %ebx
	je	.Ladd_to_register.2
	movl	$1, %eax
	jmp	.Ladd_to_register.1
.Ladd_to_register.2:
	movl	$0, %eax
	jmp	.Ladd_to_register.1
.Ladd_to_register.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	add_to_register, .-add_to_register
	.globl	sub_from_local
	.type	sub_from_local, @function
sub_from_local:
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
	movl	-8(%rbp), %eax
	subl	%eax, -12(%rbp)
	je	.Lsub_from_local.2
	movl	$1, %eax
	jmp	.Lsub_from_local.1
.Lsub_from_local.2:
	movl	$0, %eax
	jmp	.Lsub_from_local.1
.Lsub_from_local.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	sub_from_local, .-sub_from_local
	.globl	sub_from_char
	.type	sub_from_char, @function
sub_from_char:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	movb	%al, -9(%rbp)
	movl	-8(%rbp), %eax
	subb	%al, -9(%rbp)
	je	.Lsub_from_char.2
	movl	$1, %eax
	jmp	.Lsub_from_char.1
.Lsub_from_char.2:
	movl	$0, %eax
	jmp	.Lsub_from_char.1
.Lsub_from_char.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	sub_from_char, .-sub_from_char
	.globl	sub_from_global
	.type	sub_from_global, @function
sub_from_global:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	subl	%eax, counter(%rip)
	je	.Lsub_from_global.2
	movl	$1, %eax
	jmp	.Lsub_from_global.1
.Lsub_from_global.2:
	movl	$0, %eax
	jmp	.Lsub_from_global.1
.Lsub_from_global.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	sub_from_global, .-sub_from_global
	.globl	sub_from_register
	.type	sub_from_register, @function
sub_from_register:
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
	movl	%eax, %ebx
	movl	-16(%rbp), %eax
	subl	%eax, %ebx
	je	.Lsub_from_register.2
	movl	$1, %eax
	jmp	.Lsub_from_register.1
.Lsub_from_register.2:
	movl	$0, %eax
	jmp	.Lsub_from_register.1
.Lsub_from_register.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	sub_from_register, .-sub_from_register
	.globl	and_to_local
	.type	and_to_local, @function
and_to_local:
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
	movl	-8(%rbp), %eax
	andl	%eax, -12(%rbp)
	je	.Land_to_local.2
	movl	$1, %eax
	jmp	.Land_to_local.1
.Land_to_local.2:
	movl	$0, %eax
	jmp	.Land_to_local.1
.Land_to_local.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	and_to_local, .-and_to_local
	.globl	and_to_char
	.type	and_to_char, @function
and_to_char:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	movb	%al, -9(%rbp)
	movl	-8(%rbp), %eax
	andb	%al, -9(%rbp)
	je	.Land_to_char.2
	movl	$1, %eax
	jmp	.Land_to_char.1
.Land_to_char.2:
	movl	$0, %eax
	jmp	.Land_to_char.1
.Land_to_char.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	and_to_char, .-and_to_char
	.globl	and_to_global
	.type	and_to_global, @function
and_to_global:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	andl	%eax, counter(%rip)
	je	.Land_to_global.2
	movl	$1, %eax
	jmp	.Land_to_global.1
.Land_to_global.2:
	movl	$0, %eax
	jmp	.Land_to_global.1
.Land_to_global.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	and_to_global, .-and_to_global
	.globl	and_to_register
	.type	and_to_register, @function
and_to_register:
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
	movl	%eax, %ebx
	movl	-16(%rbp), %eax
	andl	%eax, %ebx
	je	.Land_to_register.2
	movl	$1, %eax
	jmp	.Land_to_register.1
.Land_to_register.2:
	movl	$0, %eax
	jmp	.Land_to_register.1
.Land_to_register.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	and_to_register, .-and_to_register
	.globl	or_to_local
	.type	or_to_local, @function
or_to_local:
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
	movl	-8(%rbp), %eax
	orl	%eax, -12(%rbp)
	je	.Lor_to_local.2
	movl	$1, %eax
	jmp	.Lor_to_local.1
.Lor_to_local.2:
	movl	$0, %eax
	jmp	.Lor_to_local.1
.Lor_to_local.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	or_to_local, .-or_to_local
	.globl	or_to_char
	.type	or_to_char, @function
or_to_char:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	movb	%al, -9(%rbp)
	movl	-8(%rbp), %eax
	orb	%al, -9(%rbp)
	je	.Lor_to_char.2
	movl	$1, %eax
	jmp	.Lor_to_char.1
.Lor_to_char.2:
	movl	$0, %eax
	jmp	.Lor_to_char.1
.Lor_to_char.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	or_to_char, .-or_to_char
	.globl	or_to_global
	.type	or_to_global, @function
or_to_global:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	orl	%eax, counter(%rip)
	je	.Lor_to_global.2
	movl	$1, %eax
	jmp	.Lor_to_global.1
.Lor_to_global.2:
	movl	$0, %eax
	jmp	.Lor_to_global.1
.Lor_to_global.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	or_to_global, .-or_to_global
	.globl	or_to_register
	.type	or_to_register, @function
or_to_register:
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
	movl	%eax, %ebx
	movl	-16(%rbp), %eax
	orl	%eax, %ebx
	je	.Lor_to_register.2
	movl	$1, %eax
	jmp	.Lor_to_register.1
.Lor_to_register.2:
	movl	$0, %eax
	jmp	.Lor_to_register.1
.Lor_to_register.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	or_to_register, .-or_to_register
	.globl	xor_to_local
	.type	xor_to_local, @function
xor_to_local:
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
	movl	-8(%rbp), %eax
	xorl	%eax, -12(%rbp)
	je	.Lxor_to_local.2
	movl	$1, %eax
	jmp	.Lxor_to_local.1
.Lxor_to_local.2:
	movl	$0, %eax
	jmp	.Lxor_to_local.1
.Lxor_to_local.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	xor_to_local, .-xor_to_local
	.globl	xor_to_char
	.type	xor_to_char, @function
xor_to_char:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	movb	%al, -9(%rbp)
	movl	-8(%rbp), %eax
	xorb	%al, -9(%rbp)
	je	.Lxor_to_char.2
	movl	$1, %eax
	jmp	.Lxor_to_char.1
.Lxor_to_char.2:
	movl	$0, %eax
	jmp	.Lxor_to_char.1
.Lxor_to_char.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	xor_to_char, .-xor_to_char
	.globl	xor_to_global
	.type	xor_to_global, @function
xor_to_global:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	xorl	%eax, counter(%rip)
	je	.Lxor_to_global.2
	movl	$1, %eax
	jmp	.Lxor_to_global.1
.Lxor_to_global.2:
	movl	$0, %eax
	jmp	.Lxor_to_global.1
.Lxor_to_global.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	xor_to_global, .-xor_to_global
	.globl	xor_to_register
	.type	xor_to_register, @function
xor_to_register:
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
	movl	%eax, %ebx
	movl	-16(%rbp), %eax
	xorl	%eax, %ebx
	je	.Lxor_to_register.2
	movl	$1, %eax
	jmp	.Lxor_to_register.1
.Lxor_to_register.2:
	movl	$0, %eax
	jmp	.Lxor_to_register.1
.Lxor_to_register.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	xor_to_register, .-xor_to_register
	.globl	inc_local
	.type	inc_local, @function
inc_local:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -8(%rbp)
	addl	$1, -8(%rbp)
	je	.Linc_local.2
	movl	$1, %eax
	jmp	.Linc_local.1
.Linc_local.2:
	movl	$0, %eax
	jmp	.Linc_local.1
.Linc_local.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	inc_local, .-inc_local
	.globl	inc_char
	.type	inc_char, @function
inc_char:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	movb	%al, -5(%rbp)
	addb	$1, -5(%rbp)
	je	.Linc_char.2
	movl	$1, %eax
	jmp	.Linc_char.1
.Linc_char.2:
	movl	$0, %eax
	jmp	.Linc_char.1
.Linc_char.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	inc_char, .-inc_char
	.globl	inc_global
	.type	inc_global, @function
inc_global:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	addl	$1, counter(%rip)
	je	.Linc_global.2
	movl	$1, %eax
	jmp	.Linc_global.1
.Linc_global.2:
	movl	$0, %eax
	jmp	.Linc_global.1
.Linc_global.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	inc_global, .-inc_global
	.globl	inc_register
	.type	inc_register, @function
inc_register:
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
	movl	%eax, %ebx
	addl	$1, %ebx
	je	.Linc_register.2
	movl	$1, %eax
	jmp	.Linc_register.1
.Linc_register.2:
	movl	$0, %eax
	jmp	.Linc_register.1
.Linc_register.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	inc_register, .-inc_register
	.globl	dec_local
	.type	dec_local, @function
dec_local:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -8(%rbp)
	subl	$1, -8(%rbp)
	je	.Ldec_local.2
	movl	$1, %eax
	jmp	.Ldec_local.1
.Ldec_local.2:
	movl	$0, %eax
	jmp	.Ldec_local.1
.Ldec_local.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	dec_local, .-dec_local
	.globl	dec_char
	.type	dec_char, @function
dec_char:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	movb	%al, -5(%rbp)
	subb	$1, -5(%rbp)
	je	.Ldec_char.2
	movl	$1, %eax
	jmp	.Ldec_char.1
.Ldec_char.2:
	movl	$0, %eax
	jmp	.Ldec_char.1
.Ldec_char.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	dec_char, .-dec_char
	.globl	dec_global
	.type	dec_global, @function
dec_global:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subl	$1, counter(%rip)
	je	.Ldec_global.2
	movl	$1, %eax
	jmp	.Ldec_global.1
.Ldec_global.2:
	movl	$0, %eax
	jmp	.Ldec_global.1
.Ldec_global.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	dec_global, .-dec_global
	.globl	dec_register
	.type	dec_register, @function
dec_register:
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
	movl	%eax, %ebx
	subl	$1, %ebx
	je	.Ldec_register.2
	movl	$1, %eax
	jmp	.Ldec_register.1
.Ldec_register.2:
	movl	$0, %eax
	jmp	.Ldec_register.1
.Ldec_register.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	dec_register, .-dec_register
	.globl	equal_zero
	.type	equal_zero, @function
equal_zero:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	testl	-8(%rbp), %eax
	jne	.Lequal_zero.2
	movl	$1, %eax
	jmp	.Lequal_zero.1
.Lequal_zero.2:
	movl	$0, %eax
	jmp	.Lequal_zero.1
.Lequal_zero.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	equal_zero, .-equal_zero
	.globl	unequal_zero
	.type	unequal_zero, @function
unequal_zero:
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
	movl	-8(%rbp), %eax
	subl	%eax, -12(%rbp)
	je	.Lunequal_zero.2
	movl	$1, %eax
	jmp	.Lunequal_zero.1
.Lunequal_zero.2:
	movl	$0, %eax
	jmp	.Lunequal_zero.1
.Lunequal_zero.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	unequal_zero, .-unequal_zero
	.globl	product
	.type	product, @function
product:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	imull	-8(%rbp), %eax
	cmpl	$0, %eax
	je	.Lproduct.2
	movl	$1, %eax
	jmp	.Lproduct.1
.Lproduct.2:
	movl	$0, %eax
	jmp	.Lproduct.1
.Lproduct.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	product, .-product
	.globl	shift
	.type	shift, @function
shift:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	movl	-8(%rbp), %ecx
	shll	%cl, %eax
	cmpl	$0, %eax
	je	.Lshift.2
	movl	$1, %eax
	jmp	.Lshift.1
.Lshift.2:
	movl	$0, %eax
	jmp	.Lshift.1
.Lshift.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	shift, .-shift
	.globl	postfix
	.type	postfix, @function
postfix:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -8(%rbp)
	movl	-8(%rbp), %eax
	movl	%eax, %ecx
	addl	$1, %eax
	movl	%eax, -8(%rbp)
	cmpl	$0, %ecx
	je	.Lpostfix.2
	movl	$1, %eax
	jmp	.Lpostfix.1
.Lpostfix.2:
	movl	$0, %eax
	jmp	.Lpostfix.1
.Lpostfix.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	postfix, .-postfix
	.globl	narrow_register
	.type	narrow_register, @function
narrow_register:
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
	movsbl	%al, %ebx
	addl	$1, %ebx
	movsbl	%bl, %ebx
	cmpb	$0, %bl
	je	.Lnarrow_register.2
	movl	$1, %eax
	jmp	.Lnarrow_register.1
.Lnarrow_register.2:
	movl	$0, %eax
	jmp	.Lnarrow_register.1
.Lnarrow_register.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	narrow_register, .-narrow_register
	.globl	negative
	.type	negative, @function
negative:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	addl	-8(%rbp), %eax
	cmpl	$0, %eax
	jge	.Lnegative.2
	movl	$1, %eax
	jmp	.Lnegative.1
.Lnegative.2:
	movl	$0, %eax
	jmp	.Lnegative.1
.Lnegative.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	negative, .-negative
	.bss
	.globl	counter
	.type	counter, @object
	.size	counter, 4
	.p2align 2
counter:
	.zero	4
	.section	.note.GNU-stack,"",@progbits
