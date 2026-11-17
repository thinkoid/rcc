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
	movl	-4(%rbp), %eax
	imull	$4, %eax, %eax
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
	.bss
	.globl	defined
	.type	defined, @object
	.size	defined, 4
	.p2align 2
defined:
	.zero	4
	.section	.note.GNU-stack,"",@progbits
