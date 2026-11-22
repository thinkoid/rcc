	.file	"bit-field-read.c"
	.text
	.globl	sbyte_lo
	.type	sbyte_lo, @function
sbyte_lo:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movsbl	(%rax), %eax
	shll	$29, %eax
	sarl	$29, %eax
	jmp	.Lsbyte_lo.1
.Lsbyte_lo.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	sbyte_lo, .-sbyte_lo
	.globl	sbyte_mid
	.type	sbyte_mid, @function
sbyte_mid:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movsbl	(%rax), %eax
	shll	$26, %eax
	sarl	$29, %eax
	jmp	.Lsbyte_mid.1
.Lsbyte_mid.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	sbyte_mid, .-sbyte_mid
	.globl	sbyte_top
	.type	sbyte_top, @function
sbyte_top:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movsbl	(%rax), %eax
	sarl	$6, %eax
	jmp	.Lsbyte_top.1
.Lsbyte_top.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	sbyte_top, .-sbyte_top
	.globl	ubyte_lo
	.type	ubyte_lo, @function
ubyte_lo:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movzbl	(%rax), %eax
	shll	$29, %eax
	shrl	$29, %eax
	jmp	.Lubyte_lo.1
.Lubyte_lo.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	ubyte_lo, .-ubyte_lo
	.globl	ubyte_mid
	.type	ubyte_mid, @function
ubyte_mid:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movzbl	(%rax), %eax
	shll	$26, %eax
	shrl	$29, %eax
	jmp	.Lubyte_mid.1
.Lubyte_mid.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	ubyte_mid, .-ubyte_mid
	.globl	ubyte_top
	.type	ubyte_top, @function
ubyte_top:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movzbl	(%rax), %eax
	shrl	$6, %eax
	jmp	.Lubyte_top.1
.Lubyte_top.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	ubyte_top, .-ubyte_top
	.globl	sword_lo
	.type	sword_lo, @function
sword_lo:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movl	(%rax), %eax
	shll	$12, %eax
	sarl	$12, %eax
	jmp	.Lsword_lo.1
.Lsword_lo.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	sword_lo, .-sword_lo
	.globl	sword_mid
	.type	sword_mid, @function
sword_mid:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movl	4(%rax), %eax
	shll	$11, %eax
	sarl	$23, %eax
	jmp	.Lsword_mid.1
.Lsword_mid.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	sword_mid, .-sword_mid
	.globl	sword_top
	.type	sword_top, @function
sword_top:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movl	8(%rax), %eax
	sarl	$15, %eax
	jmp	.Lsword_top.1
.Lsword_top.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	sword_top, .-sword_top
	.globl	uword_lo
	.type	uword_lo, @function
uword_lo:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movl	(%rax), %eax
	shll	$12, %eax
	shrl	$12, %eax
	jmp	.Luword_lo.1
.Luword_lo.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	uword_lo, .-uword_lo
	.globl	uword_mid
	.type	uword_mid, @function
uword_mid:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movl	4(%rax), %eax
	shll	$11, %eax
	shrl	$23, %eax
	jmp	.Luword_mid.1
.Luword_mid.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	uword_mid, .-uword_mid
	.globl	uword_top
	.type	uword_top, @function
uword_top:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movl	8(%rax), %eax
	shrl	$15, %eax
	jmp	.Luword_top.1
.Luword_top.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	uword_top, .-uword_top
	.globl	slong_lo
	.type	slong_lo, @function
slong_lo:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	(%rax), %rax
	shlq	$24, %rax
	sarq	$24, %rax
	jmp	.Lslong_lo.1
.Lslong_lo.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	slong_lo, .-slong_lo
	.globl	slong_mid
	.type	slong_mid, @function
slong_mid:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	8(%rax), %rax
	shlq	$14, %rax
	sarq	$34, %rax
	jmp	.Lslong_mid.1
.Lslong_mid.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	slong_mid, .-slong_mid
	.globl	slong_top
	.type	slong_top, @function
slong_top:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	16(%rax), %rax
	sarq	$20, %rax
	jmp	.Lslong_top.1
.Lslong_top.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	slong_top, .-slong_top
	.globl	ulong_lo
	.type	ulong_lo, @function
ulong_lo:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	(%rax), %rax
	shlq	$24, %rax
	shrq	$24, %rax
	jmp	.Lulong_lo.1
.Lulong_lo.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	ulong_lo, .-ulong_lo
	.globl	ulong_mid
	.type	ulong_mid, @function
ulong_mid:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	8(%rax), %rax
	shlq	$14, %rax
	shrq	$34, %rax
	jmp	.Lulong_mid.1
.Lulong_mid.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	ulong_mid, .-ulong_mid
	.globl	ulong_top
	.type	ulong_top, @function
ulong_top:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	16(%rax), %rax
	shrq	$20, %rax
	jmp	.Lulong_top.1
.Lulong_top.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	ulong_top, .-ulong_top
	.globl	flags_b
	.type	flags_b, @function
flags_b:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movzbl	(%rax), %eax
	shll	$30, %eax
	shrl	$31, %eax
	jmp	.Lflags_b.1
.Lflags_b.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	flags_b, .-flags_b
	.section	.note.GNU-stack,"",@progbits
