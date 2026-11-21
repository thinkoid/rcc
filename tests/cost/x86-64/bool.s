	.file	"bool.c"
	.text
	.globl	from_int
	.type	from_int, @function
from_int:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	cmpl	$0, i(%rip)
	setne	%al
	movzbl	%al, %eax
	movb	%al, b(%rip)
.Lfrom_int.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	from_int, .-from_int
	.globl	from_long
	.type	from_long, @function
from_long:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	cmpq	$0, l(%rip)
	setne	%al
	movzbl	%al, %eax
	movb	%al, b(%rip)
.Lfrom_long.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	from_long, .-from_long
	.globl	from_pointer
	.type	from_pointer, @function
from_pointer:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	cmpq	$0, p(%rip)
	setne	%al
	movzbl	%al, %eax
	movb	%al, b(%rip)
.Lfrom_pointer.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	from_pointer, .-from_pointer
	.globl	from_constant
	.type	from_constant, @function
from_constant:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movb	$1, b(%rip)
.Lfrom_constant.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	from_constant, .-from_constant
	.globl	from_comparison
	.type	from_comparison, @function
from_comparison:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movslq	i(%rip), %rax
	cmpq	l(%rip), %rax
	setl	%al
	movzbl	%al, %eax
	movb	%al, b(%rip)
.Lfrom_comparison.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	from_comparison, .-from_comparison
	.globl	copied
	.type	copied, @function
copied:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movzbl	c(%rip), %eax
	movb	%al, b(%rip)
.Lcopied.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	copied, .-copied
	.globl	passed
	.type	passed, @function
passed:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movzbl	c(%rip), %eax
	movb	%al, -1(%rbp)
	movzbl	-1(%rbp), %edi
	call	take@PLT
.Lpassed.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	passed, .-passed
	.globl	returned
	.type	returned, @function
returned:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movzbl	c(%rip), %eax
	jmp	.Lreturned.1
.Lreturned.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	returned, .-returned
	.globl	read
	.type	read, @function
read:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movzbl	b(%rip), %eax
	addl	$1, %eax
	jmp	.Lread.1
.Lread.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	read, .-read
	.globl	condition
	.type	condition, @function
condition:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	cmpb	$0, b(%rip)
	je	.Lcondition.2
	movl	$3, %eax
	jmp	.Lcondition.1
.Lcondition.2:
	movl	$4, %eax
	jmp	.Lcondition.1
.Lcondition.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	condition, .-condition
	.globl	add
	.type	add, @function
add:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movzbl	b(%rip), %eax
	addl	i(%rip), %eax
	cmpl	$0, %eax
	setne	%al
	movzbl	%al, %eax
	movb	%al, b(%rip)
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
	movzbl	b(%rip), %eax
	subl	i(%rip), %eax
	cmpl	$0, %eax
	setne	%al
	movzbl	%al, %eax
	movb	%al, b(%rip)
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
	movzbl	b(%rip), %eax
	andl	i(%rip), %eax
	cmpl	$0, %eax
	setne	%al
	movzbl	%al, %eax
	movb	%al, b(%rip)
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
	movzbl	b(%rip), %eax
	orl	i(%rip), %eax
	cmpl	$0, %eax
	setne	%al
	movzbl	%al, %eax
	movb	%al, b(%rip)
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
	movzbl	b(%rip), %eax
	xorl	i(%rip), %eax
	cmpl	$0, %eax
	setne	%al
	movzbl	%al, %eax
	movb	%al, b(%rip)
.Lxor.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	xor, .-xor
	.globl	shl
	.type	shl, @function
shl:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movzbl	b(%rip), %eax
	movl	i(%rip), %ecx
	shll	%cl, %eax
	cmpl	$0, %eax
	setne	%al
	movzbl	%al, %eax
	movb	%al, b(%rip)
.Lshl.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	shl, .-shl
	.globl	shr
	.type	shr, @function
shr:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movzbl	b(%rip), %eax
	movl	i(%rip), %ecx
	sarl	%cl, %eax
	cmpl	$0, %eax
	setne	%al
	movzbl	%al, %eax
	movb	%al, b(%rip)
.Lshr.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	shr, .-shr
	.globl	mul
	.type	mul, @function
mul:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movzbl	b(%rip), %eax
	imull	i(%rip), %eax
	cmpl	$0, %eax
	setne	%al
	movzbl	%al, %eax
	movb	%al, b(%rip)
.Lmul.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	mul, .-mul
	.globl	div
	.type	div, @function
div:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movzbl	b(%rip), %eax
	cltd
	idivl	i(%rip)
	cmpl	$0, %eax
	setne	%al
	movzbl	%al, %eax
	movb	%al, b(%rip)
.Ldiv.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	div, .-div
	.globl	rem
	.type	rem, @function
rem:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movzbl	b(%rip), %eax
	cltd
	idivl	i(%rip)
	cmpl	$0, %edx
	setne	%dl
	movzbl	%dl, %edx
	movb	%dl, b(%rip)
.Lrem.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	rem, .-rem
	.globl	pre_inc
	.type	pre_inc, @function
pre_inc:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movzbl	b(%rip), %eax
	addl	$1, %eax
	cmpl	$0, %eax
	setne	%al
	movzbl	%al, %eax
	movb	%al, b(%rip)
	movzbl	%al, %eax
	jmp	.Lpre_inc.1
.Lpre_inc.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	pre_inc, .-pre_inc
	.globl	pre_dec
	.type	pre_dec, @function
pre_dec:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movzbl	b(%rip), %eax
	subl	$1, %eax
	cmpl	$0, %eax
	setne	%al
	movzbl	%al, %eax
	movb	%al, b(%rip)
	movzbl	%al, %eax
	jmp	.Lpre_dec.1
.Lpre_dec.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	pre_dec, .-pre_dec
	.globl	post_inc
	.type	post_inc, @function
post_inc:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movzbl	b(%rip), %eax
	movb	%al, -1(%rbp)
	movzbl	-1(%rbp), %eax
	addl	$1, %eax
	cmpl	$0, %eax
	setne	%al
	movzbl	%al, %eax
	movb	%al, b(%rip)
	movzbl	-1(%rbp), %eax
	jmp	.Lpost_inc.1
.Lpost_inc.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	post_inc, .-post_inc
	.globl	post_dec
	.type	post_dec, @function
post_dec:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movzbl	b(%rip), %eax
	movb	%al, -1(%rbp)
	movzbl	-1(%rbp), %eax
	subl	$1, %eax
	cmpl	$0, %eax
	setne	%al
	movzbl	%al, %eax
	movb	%al, b(%rip)
	movzbl	-1(%rbp), %eax
	jmp	.Lpost_dec.1
.Lpost_dec.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	post_dec, .-post_dec
	.bss
	.globl	b
	.type	b, @object
	.size	b, 1
b:
	.zero	1
	.globl	c
	.type	c, @object
	.size	c, 1
c:
	.zero	1
	.globl	i
	.type	i, @object
	.size	i, 4
	.p2align 2
i:
	.zero	4
	.globl	l
	.type	l, @object
	.size	l, 8
	.p2align 3
l:
	.zero	8
	.globl	p
	.type	p, @object
	.size	p, 8
	.p2align 3
p:
	.zero	8
	.section	.note.GNU-stack,"",@progbits
