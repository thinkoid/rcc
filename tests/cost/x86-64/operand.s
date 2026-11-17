	.file	"operand.c"
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
	movq	%rsi, -16(%rbp)
	movl	%edx, -20(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -24(%rbp)
	movl	-4(%rbp), %eax
	addl	-24(%rbp), %eax
	addl	defined(%rip), %eax
	movq	declared@GOTPCREL(%rip), %rcx
	addl	(%rcx), %eax
	movq	-16(%rbp), %rcx
	addl	(%rcx), %eax
	movslq	-20(%rbp), %rcx
	addl	-40(%rbp,%rcx,4), %eax
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
	movq	%rsi, -16(%rbp)
	movl	%edx, -20(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -24(%rbp)
	movl	-4(%rbp), %eax
	subl	-24(%rbp), %eax
	subl	defined(%rip), %eax
	movq	declared@GOTPCREL(%rip), %rcx
	subl	(%rcx), %eax
	movq	-16(%rbp), %rcx
	subl	(%rcx), %eax
	movslq	-20(%rbp), %rcx
	subl	-40(%rbp,%rcx,4), %eax
	jmp	.Lsub.1
.Lsub.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	sub, .-sub
	.globl	and
	.type	and, @function
and:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	movl	%edx, -20(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -24(%rbp)
	movl	-4(%rbp), %eax
	andl	-24(%rbp), %eax
	andl	defined(%rip), %eax
	movq	declared@GOTPCREL(%rip), %rcx
	andl	(%rcx), %eax
	movq	-16(%rbp), %rcx
	andl	(%rcx), %eax
	movslq	-20(%rbp), %rcx
	andl	-40(%rbp,%rcx,4), %eax
	jmp	.Land.1
.Land.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	and, .-and
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
	movq	%rsi, -16(%rbp)
	movl	%edx, -20(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -24(%rbp)
	movl	-4(%rbp), %eax
	orl	-24(%rbp), %eax
	orl	defined(%rip), %eax
	movq	declared@GOTPCREL(%rip), %rcx
	orl	(%rcx), %eax
	movq	-16(%rbp), %rcx
	orl	(%rcx), %eax
	movslq	-20(%rbp), %rcx
	orl	-40(%rbp,%rcx,4), %eax
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
	movq	%rsi, -16(%rbp)
	movl	%edx, -20(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -24(%rbp)
	movl	-4(%rbp), %eax
	xorl	-24(%rbp), %eax
	xorl	defined(%rip), %eax
	movq	declared@GOTPCREL(%rip), %rcx
	xorl	(%rcx), %eax
	movq	-16(%rbp), %rcx
	xorl	(%rcx), %eax
	movslq	-20(%rbp), %rcx
	xorl	-40(%rbp,%rcx,4), %eax
	jmp	.Lxor.1
.Lxor.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	xor, .-xor
	.globl	mul
	.type	mul, @function
mul:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	movl	%edx, -20(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -24(%rbp)
	movl	-4(%rbp), %eax
	imull	-24(%rbp), %eax
	imull	defined(%rip), %eax
	movq	declared@GOTPCREL(%rip), %rcx
	imull	(%rcx), %eax
	movq	-16(%rbp), %rcx
	imull	(%rcx), %eax
	movslq	-20(%rbp), %rcx
	imull	-40(%rbp,%rcx,4), %eax
	jmp	.Lmul.1
.Lmul.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	mul, .-mul
	.globl	divide
	.type	divide, @function
divide:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	movl	%edx, -20(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -24(%rbp)
	movl	-4(%rbp), %eax
	cltd
	idivl	-24(%rbp)
	cltd
	idivl	defined(%rip)
	movq	declared@GOTPCREL(%rip), %rcx
	cltd
	idivl	(%rcx)
	movq	-16(%rbp), %rcx
	cltd
	idivl	(%rcx)
	movslq	-20(%rbp), %rcx
	cltd
	idivl	-40(%rbp,%rcx,4)
	jmp	.Ldivide.1
.Ldivide.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	divide, .-divide
	.globl	modulo
	.type	modulo, @function
modulo:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	movl	%edx, -20(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -24(%rbp)
	movl	-4(%rbp), %eax
	cltd
	idivl	-24(%rbp)
	movl	%edx, %eax
	cltd
	idivl	defined(%rip)
	movq	declared@GOTPCREL(%rip), %rcx
	movl	%edx, %eax
	cltd
	idivl	(%rcx)
	movq	-16(%rbp), %rcx
	movl	%edx, %eax
	cltd
	idivl	(%rcx)
	movslq	-20(%rbp), %rcx
	movl	%edx, %eax
	cltd
	idivl	-40(%rbp,%rcx,4)
	movl	%edx, %eax
	jmp	.Lmodulo.1
.Lmodulo.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	modulo, .-modulo
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
	movq	%rsi, -16(%rbp)
	movl	%edx, -20(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -24(%rbp)
	movl	-4(%rbp), %eax
	cmpl	-24(%rbp), %eax
	setl	%al
	movzbl	%al, %eax
	movl	-4(%rbp), %ecx
	cmpl	defined(%rip), %ecx
	setl	%cl
	movzbl	%cl, %ecx
	addl	%ecx, %eax
	movl	-4(%rbp), %ecx
	movq	declared@GOTPCREL(%rip), %rdx
	cmpl	(%rdx), %ecx
	setl	%cl
	movzbl	%cl, %ecx
	addl	%ecx, %eax
	movl	-4(%rbp), %ecx
	movq	-16(%rbp), %rdx
	cmpl	(%rdx), %ecx
	setl	%cl
	movzbl	%cl, %ecx
	addl	%ecx, %eax
	movl	-4(%rbp), %ecx
	movslq	-20(%rbp), %rdx
	cmpl	-40(%rbp,%rdx,4), %ecx
	setl	%cl
	movzbl	%cl, %ecx
	addl	%ecx, %eax
	jmp	.Lless.1
.Lless.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	less, .-less
	.globl	test
	.type	test, @function
test:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	movl	%edx, -20(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -24(%rbp)
	movl	-4(%rbp), %eax
	testl	-24(%rbp), %eax
	je	.Ltest.2
	movl	-4(%rbp), %eax
	testl	defined(%rip), %eax
	je	.Ltest.2
	movl	-4(%rbp), %eax
	movq	declared@GOTPCREL(%rip), %rcx
	testl	(%rcx), %eax
	je	.Ltest.2
	movl	-4(%rbp), %eax
	movq	-16(%rbp), %rcx
	testl	(%rcx), %eax
	je	.Ltest.2
	movl	-4(%rbp), %eax
	movslq	-20(%rbp), %rcx
	testl	-40(%rbp,%rcx,4), %eax
	je	.Ltest.2
	movl	$1, %eax
	jmp	.Ltest.1
.Ltest.2:
	movl	$0, %eax
	jmp	.Ltest.1
.Ltest.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	test, .-test
	.globl	divide_busy
	.type	divide_busy, @function
divide_busy:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movq	%rdx, -16(%rbp)
	movl	-4(%rbp), %ecx
	movl	-8(%rbp), %eax
	movq	-16(%rbp), %rsi
	cltd
	idivl	(%rsi)
	addl	%eax, %ecx
	movl	%ecx, %eax
	jmp	.Ldivide_busy.1
.Ldivide_busy.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	divide_busy, .-divide_busy
	.globl	less_constant
	.type	less_constant, @function
less_constant:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	cmpl	$5, -4(%rbp)
	jge	.Lless_constant.2
	movl	$1, %eax
	jmp	.Lless_constant.1
.Lless_constant.2:
	movl	$0, %eax
	jmp	.Lless_constant.1
.Lless_constant.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	less_constant, .-less_constant
	.globl	nonzero
	.type	nonzero, @function
nonzero:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	cmpl	$0, -4(%rbp)
	je	.Lnonzero.2
	movl	$1, %eax
	jmp	.Lnonzero.1
.Lnonzero.2:
	movl	$0, %eax
	jmp	.Lnonzero.1
.Lnonzero.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	nonzero, .-nonzero
	.globl	nonzero_char
	.type	nonzero_char, @function
nonzero_char:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movb	%dil, -1(%rbp)
	cmpb	$0, -1(%rbp)
	je	.Lnonzero_char.2
	movl	$1, %eax
	jmp	.Lnonzero_char.1
.Lnonzero_char.2:
	movl	$0, %eax
	jmp	.Lnonzero_char.1
.Lnonzero_char.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	nonzero_char, .-nonzero_char
	.globl	bit
	.type	bit, @function
bit:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	testl	$1, -4(%rbp)
	je	.Lbit.2
	movl	$1, %eax
	jmp	.Lbit.1
.Lbit.2:
	movl	$0, %eax
	jmp	.Lbit.1
.Lbit.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	bit, .-bit
	.globl	char_operand
	.type	char_operand, @function
char_operand:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movb	%sil, -5(%rbp)
	movl	-4(%rbp), %eax
	movsbl	-5(%rbp), %ecx
	addl	%ecx, %eax
	jmp	.Lchar_operand.1
.Lchar_operand.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	char_operand, .-char_operand
	.globl	widened
	.type	widened, @function
widened:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rax
	movslq	-12(%rbp), %rcx
	addq	%rcx, %rax
	jmp	.Lwidened.1
.Lwidened.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	widened, .-widened
	.globl	compound
	.type	compound, @function
compound:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-8(%rbp), %eax
	addl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	jmp	.Lcompound.1
.Lcompound.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	compound, .-compound
	.globl	compound_value
	.type	compound_value, @function
compound_value:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-8(%rbp), %ecx
	movl	-4(%rbp), %eax
	addl	%ecx, %eax
	movl	%eax, -4(%rbp)
	jmp	.Lcompound_value.1
.Lcompound_value.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	compound_value, .-compound_value
	.globl	shift_count
	.type	shift_count, @function
shift_count:
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
	jmp	.Lshift_count.1
.Lshift_count.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	shift_count, .-shift_count
	.globl	times_four
	.type	times_four, @function
times_four:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	imull	$4, -4(%rbp), %eax
	jmp	.Ltimes_four.1
.Ltimes_four.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	times_four, .-times_four
	.globl	before_call
	.type	before_call, @function
before_call:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	defined(%rip), %eax
	movq	%rax, -8(%rbp)
	call	f@PLT
	movq	-8(%rbp), %rcx
	cmpl	%eax, %ecx
	jge	.Lbefore_call.2
	movl	$1, %eax
	jmp	.Lbefore_call.1
.Lbefore_call.2:
	movl	$0, %eax
	jmp	.Lbefore_call.1
.Lbefore_call.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	before_call, .-before_call
	.globl	constant_left
	.type	constant_left, @function
constant_left:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	$0, -8(%rbp)
	cmpl	$5, -4(%rbp)
	jle	.Lconstant_left.2
	addl	$1, -8(%rbp)
.Lconstant_left.2:
	cmpl	$5, -4(%rbp)
	jl	.Lconstant_left.3
	addl	$2, -8(%rbp)
.Lconstant_left.3:
	cmpl	$5, -4(%rbp)
	jge	.Lconstant_left.4
	addl	$4, -8(%rbp)
.Lconstant_left.4:
	cmpl	$5, -4(%rbp)
	jg	.Lconstant_left.5
	addl	$8, -8(%rbp)
.Lconstant_left.5:
	cmpl	$5, -4(%rbp)
	jne	.Lconstant_left.6
	addl	$16, -8(%rbp)
.Lconstant_left.6:
	cmpl	$5, -4(%rbp)
	je	.Lconstant_left.7
	addl	$32, -8(%rbp)
.Lconstant_left.7:
	movl	-8(%rbp), %eax
	jmp	.Lconstant_left.1
.Lconstant_left.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	constant_left, .-constant_left
	.globl	constant_left_unsigned
	.type	constant_left_unsigned, @function
constant_left_unsigned:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	$0, -8(%rbp)
	cmpl	$5, -4(%rbp)
	jbe	.Lconstant_left_unsigned.2
	addl	$1, -8(%rbp)
.Lconstant_left_unsigned.2:
	cmpl	$5, -4(%rbp)
	jb	.Lconstant_left_unsigned.3
	addl	$2, -8(%rbp)
.Lconstant_left_unsigned.3:
	cmpl	$5, -4(%rbp)
	jae	.Lconstant_left_unsigned.4
	addl	$4, -8(%rbp)
.Lconstant_left_unsigned.4:
	cmpl	$5, -4(%rbp)
	ja	.Lconstant_left_unsigned.5
	addl	$8, -8(%rbp)
.Lconstant_left_unsigned.5:
	cmpl	$5, -4(%rbp)
	jne	.Lconstant_left_unsigned.6
	addl	$16, -8(%rbp)
.Lconstant_left_unsigned.6:
	cmpl	$5, -4(%rbp)
	je	.Lconstant_left_unsigned.7
	addl	$32, -8(%rbp)
.Lconstant_left_unsigned.7:
	movl	-8(%rbp), %eax
	jmp	.Lconstant_left_unsigned.1
.Lconstant_left_unsigned.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	constant_left_unsigned, .-constant_left_unsigned
	.globl	constant_left_register
	.type	constant_left_register, @function
constant_left_register:
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
	cmpl	$5, %ebx
	jle	.Lconstant_left_register.2
	movl	$1, %eax
	jmp	.Lconstant_left_register.1
.Lconstant_left_register.2:
	movl	$0, %eax
	jmp	.Lconstant_left_register.1
.Lconstant_left_register.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	constant_left_register, .-constant_left_register
	.globl	constant_left_value
	.type	constant_left_value, @function
constant_left_value:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	cmpl	$5, -4(%rbp)
	setg	%al
	movzbl	%al, %eax
	jmp	.Lconstant_left_value.1
.Lconstant_left_value.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	constant_left_value, .-constant_left_value
	.globl	null_left
	.type	null_left, @function
null_left:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	cmpq	$0, -8(%rbp)
	jne	.Lnull_left.2
	movl	$1, %eax
	jmp	.Lnull_left.1
.Lnull_left.2:
	movl	$0, %eax
	jmp	.Lnull_left.1
.Lnull_left.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	null_left, .-null_left
	.globl	pointer_operand
	.type	pointer_operand, @function
pointer_operand:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	movq	%rcx, -32(%rbp)
	movq	%r8, -40(%rbp)
	movq	-8(%rbp), %rcx
	subq	-16(%rbp), %rcx
	movq	-24(%rbp), %rax
	subq	-32(%rbp), %rax
	movq	$4, %rsi
	cqto
	idivq	%rsi
	addq	%rax, %rcx
	movq	-8(%rbp), %rsi
	addq	-40(%rbp), %rsi
	subq	-16(%rbp), %rsi
	addq	%rsi, %rcx
	movq	-24(%rbp), %rax
	movq	-40(%rbp), %rdx
	leaq	(%rax,%rdx,4), %rax
	subq	-32(%rbp), %rax
	movq	$4, %rsi
	cqto
	idivq	%rsi
	addq	%rax, %rcx
	movq	%rcx, %rax
	jmp	.Lpointer_operand.1
.Lpointer_operand.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	pointer_operand, .-pointer_operand
	.globl	signedness
	.type	signedness, @function
signedness:
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
	movl	-4(%rbp), %ecx
	addl	-8(%rbp), %ecx
	movl	-4(%rbp), %esi
	cmpl	-8(%rbp), %esi
	setb	%sil
	movzbl	%sil, %esi
	addl	%esi, %ecx
	movl	-4(%rbp), %esi
	imull	-8(%rbp), %esi
	addl	%esi, %ecx
	movl	-4(%rbp), %eax
	xorl	%edx, %edx
	divl	-8(%rbp)
	addl	%eax, %ecx
	movq	-16(%rbp), %rax
	movslq	-20(%rbp), %rdx
	addq	%rdx, %rax
	addq	%rax, %rcx
	movq	%rcx, %rax
	jmp	.Lsignedness.1
.Lsignedness.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	signedness, .-signedness
	.globl	comparison_value
	.type	comparison_value, @function
comparison_value:
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
	cmpl	$5, -4(%rbp)
	setl	%al
	movzbl	%al, %eax
	cmpl	$5, -8(%rbp)
	setae	%cl
	movzbl	%cl, %ecx
	addl	%ecx, %eax
	movl	-4(%rbp), %ecx
	cmpl	-12(%rbp), %ecx
	setl	%cl
	movzbl	%cl, %ecx
	addl	%ecx, %eax
	movl	-8(%rbp), %ecx
	cmpl	-16(%rbp), %ecx
	seta	%cl
	movzbl	%cl, %ecx
	addl	%ecx, %eax
	jmp	.Lcomparison_value.1
.Lcomparison_value.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	comparison_value, .-comparison_value
	.globl	by_constant
	.type	by_constant, @function
by_constant:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	imull	$4, -4(%rbp), %eax
	imull	$-3, -4(%rbp), %ecx
	addl	%ecx, %eax
	imull	$4, -4(%rbp), %ecx
	addl	%ecx, %eax
	movslq	%eax, %rax
	imulq	$1000, -16(%rbp), %rcx
	addq	%rcx, %rax
	jmp	.Lby_constant.1
.Lby_constant.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	by_constant, .-by_constant
	.globl	commuted
	.type	commuted, @function
commuted:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movb	%sil, -5(%rbp)
	movl	-4(%rbp), %eax
	addl	$5, %eax
	imull	$4, -4(%rbp), %ecx
	subl	%ecx, %eax
	movl	-4(%rbp), %ecx
	andl	$7, %ecx
	subl	%ecx, %eax
	movl	-4(%rbp), %ecx
	orl	$7, %ecx
	subl	%ecx, %eax
	movl	-4(%rbp), %ecx
	xorl	$7, %ecx
	subl	%ecx, %eax
	movsbl	-5(%rbp), %ecx
	addl	$5, %ecx
	subl	%ecx, %eax
	jmp	.Lcommuted.1
.Lcommuted.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	commuted, .-commuted
	.globl	commuted_right
	.type	commuted_right, @function
commuted_right:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movb	%sil, -5(%rbp)
	movl	-4(%rbp), %eax
	addl	$5, %eax
	imull	$4, -4(%rbp), %ecx
	subl	%ecx, %eax
	movl	-4(%rbp), %ecx
	andl	$7, %ecx
	subl	%ecx, %eax
	movl	-4(%rbp), %ecx
	orl	$7, %ecx
	subl	%ecx, %eax
	movl	-4(%rbp), %ecx
	xorl	$7, %ecx
	subl	%ecx, %eax
	movsbl	-5(%rbp), %ecx
	addl	$5, %ecx
	subl	%ecx, %eax
	jmp	.Lcommuted_right.1
.Lcommuted_right.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	commuted_right, .-commuted_right
	.globl	commuted_register
	.type	commuted_register, @function
commuted_register:
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
	addl	$5, %eax
	movl	%ebx, %ecx
	imull	$4, %ecx, %ecx
	subl	%ecx, %eax
	movl	%ebx, %ecx
	andl	$7, %ecx
	subl	%ecx, %eax
	movl	%ebx, %ecx
	orl	$7, %ecx
	subl	%ecx, %eax
	movl	%ebx, %ecx
	xorl	$7, %ecx
	subl	%ecx, %eax
	jmp	.Lcommuted_register.1
.Lcommuted_register.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	commuted_register, .-commuted_register
	.globl	commuted_call
	.type	commuted_call, @function
commuted_call:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	call	f@PLT
	addl	$5, %eax
	jmp	.Lcommuted_call.1
.Lcommuted_call.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	commuted_call, .-commuted_call
	.globl	commuted_into
	.type	commuted_into, @function
commuted_into:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movl	%edi, -12(%rbp)
	imull	$4, -12(%rbp), %ebx
	addl	$5, %ebx
	movl	%ebx, %eax
	jmp	.Lcommuted_into.1
.Lcommuted_into.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	commuted_into, .-commuted_into
	.globl	commuted_bit
	.type	commuted_bit, @function
commuted_bit:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	testl	$1, -4(%rbp)
	je	.Lcommuted_bit.2
	movl	$1, %eax
	jmp	.Lcommuted_bit.1
.Lcommuted_bit.2:
	movl	$0, %eax
	jmp	.Lcommuted_bit.1
.Lcommuted_bit.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	commuted_bit, .-commuted_bit
	.globl	commuted_wide
	.type	commuted_wide, @function
commuted_wide:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movabsq	$4294967296, %rax
	addq	-8(%rbp), %rax
	jmp	.Lcommuted_wide.1
.Lcommuted_wide.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	commuted_wide, .-commuted_wide
	.globl	commuted_char_register
	.type	commuted_char_register, @function
commuted_char_register:
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
	movl	$5, %r12d
	addl	%ebx, %r12d
	movl	%r12d, %eax
	jmp	.Lcommuted_char_register.1
.Lcommuted_char_register.1:
	popq	%r12
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	commuted_char_register, .-commuted_char_register
	.globl	not_commuted
	.type	not_commuted, @function
not_commuted:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	$5, %esi
	subl	-4(%rbp), %esi
	movl	$40, %eax
	cltd
	idivl	-4(%rbp)
	addl	%eax, %esi
	movl	$1, %eax
	movl	-4(%rbp), %ecx
	shll	%cl, %eax
	addl	%eax, %esi
	movl	%esi, %eax
	jmp	.Lnot_commuted.1
.Lnot_commuted.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	not_commuted, .-not_commuted
	.globl	index_sub
	.type	index_sub, @function
index_sub:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rax
	imulq	$4, -16(%rbp), %rcx
	subq	%rcx, %rax
	jmp	.Lindex_sub.1
.Lindex_sub.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	index_sub, .-index_sub
	.globl	index_row
	.type	index_row, @function
index_row:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rax
	imulq	$12, -16(%rbp), %rcx
	addq	%rcx, %rax
	jmp	.Lindex_row.1
.Lindex_row.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	index_row, .-index_row
	.globl	index_first
	.type	index_first, @function
index_first:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	imulq	$12, -16(%rbp), %rax
	addq	-8(%rbp), %rax
	jmp	.Lindex_first.1
.Lindex_first.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	index_first, .-index_first
	.globl	index_compound
	.type	index_compound, @function
index_compound:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	imulq	$4, -16(%rbp), %rax
	subq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	jmp	.Lindex_compound.1
.Lindex_compound.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	index_compound, .-index_compound
	.globl	index_int
	.type	index_int, @function
index_int:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rax
	movslq	-12(%rbp), %rcx
	imulq	$4, %rcx, %rcx
	subq	%rcx, %rax
	jmp	.Lindex_int.1
.Lindex_int.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	index_int, .-index_int
	.globl	index_scaled
	.type	index_scaled, @function
index_scaled:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rax
	movq	-16(%rbp), %rcx
	leaq	(%rax,%rcx,4), %rax
	jmp	.Lindex_scaled.1
.Lindex_scaled.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	index_scaled, .-index_scaled
	.globl	widen_signedness
	.type	widen_signedness, @function
widen_signedness:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movslq	-4(%rbp), %rax
	jmp	.Lwiden_signedness.1
.Lwiden_signedness.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	widen_signedness, .-widen_signedness
	.globl	widen_unsigned
	.type	widen_unsigned, @function
widen_unsigned:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	jmp	.Lwiden_unsigned.1
.Lwiden_unsigned.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	widen_unsigned, .-widen_unsigned
	.globl	widen_narrowed
	.type	widen_narrowed, @function
widen_narrowed:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movslq	%eax, %rax
	jmp	.Lwiden_narrowed.1
.Lwiden_narrowed.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	widen_narrowed, .-widen_narrowed
	.globl	compare_register
	.type	compare_register, @function
compare_register:
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
	cmpl	%ebx, -12(%rbp)
	jge	.Lcompare_register.2
	cmpl	%ebx, -12(%rbp)
	setne	%al
	movzbl	%al, %eax
	jmp	.Lcompare_register.1
.Lcompare_register.2:
	movl	-12(%rbp), %eax
	movl	%ebx, %ecx
	addl	$1, %ecx
	cmpl	%ecx, %eax
	sete	%al
	movzbl	%al, %eax
	jmp	.Lcompare_register.1
.Lcompare_register.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	compare_register, .-compare_register
	.globl	constant_index
	.type	constant_index, @function
constant_index:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	addq	$12, %rax
	jmp	.Lconstant_index.1
.Lconstant_index.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	constant_index, .-constant_index
	.globl	constant_index_right
	.type	constant_index_right, @function
constant_index_right:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	addq	$12, %rax
	jmp	.Lconstant_index_right.1
.Lconstant_index_right.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	constant_index_right, .-constant_index_right
	.globl	constant_index_into
	.type	constant_index_into, @function
constant_index_into:
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
	addq	$12, %rbx
	addq	$12, %rbx
	movq	%rbx, %rax
	jmp	.Lconstant_index_into.1
.Lconstant_index_into.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	constant_index_into, .-constant_index_into
	.globl	constant_index_wide
	.type	constant_index_wide, @function
constant_index_wide:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	$1073741824, %rax
	movq	-8(%rbp), %rcx
	leaq	(%rcx,%rax,4), %rax
	jmp	.Lconstant_index_wide.1
.Lconstant_index_wide.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	constant_index_wide, .-constant_index_wide
	.bss
	.globl	defined
	.type	defined, @object
	.size	defined, 4
	.p2align 2
defined:
	.zero	4
	.section	.note.GNU-stack,"",@progbits
