	.file	"condition-flags.c"
	.text
	.globl	add
	.type	add, @function
add:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	w1, [x29, #20]
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #20]
	add	w0, w0, w1
	cbz	w0, .Ladd.2
	mov	w0, #1
	b	.Ladd.1
.Ladd.2:
	mov	w0, #0
	b	.Ladd.1
.Ladd.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	add, .-add
	.globl	sub
	.type	sub, @function
sub:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	w1, [x29, #20]
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #20]
	sub	w0, w0, w1
	cbz	w0, .Lsub.2
	mov	w0, #1
	b	.Lsub.1
.Lsub.2:
	mov	w0, #0
	b	.Lsub.1
.Lsub.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	sub, .-sub
	.globl	or
	.type	or, @function
or:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	w1, [x29, #20]
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #20]
	orr	w0, w0, w1
	cbz	w0, .Lor.2
	mov	w0, #1
	b	.Lor.1
.Lor.2:
	mov	w0, #0
	b	.Lor.1
.Lor.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	or, .-or
	.globl	xor
	.type	xor, @function
xor:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	w1, [x29, #20]
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #20]
	eor	w0, w0, w1
	cbz	w0, .Lxor.2
	mov	w0, #1
	b	.Lxor.1
.Lxor.2:
	mov	w0, #0
	b	.Lxor.1
.Lxor.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	xor, .-xor
	.globl	neg
	.type	neg, @function
neg:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	ldr	w0, [x29, #16]
	neg	w0, w0
	cbz	w0, .Lneg.2
	mov	w0, #1
	b	.Lneg.1
.Lneg.2:
	mov	w0, #0
	b	.Lneg.1
.Lneg.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	neg, .-neg
	.globl	test_register
	.type	test_register, @function
test_register:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	w1, [x29, #20]
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #20]
	and	w0, w0, w1
	cbz	w0, .Ltest_register.2
	mov	w0, #1
	b	.Ltest_register.1
.Ltest_register.2:
	mov	w0, #0
	b	.Ltest_register.1
.Ltest_register.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	test_register, .-test_register
	.globl	test_immediate
	.type	test_immediate, @function
test_immediate:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	ldr	w0, [x29, #16]
	and	w0, w0, #0x1
	cbz	w0, .Ltest_immediate.2
	mov	w0, #1
	b	.Ltest_immediate.1
.Ltest_immediate.2:
	mov	w0, #0
	b	.Ltest_immediate.1
.Ltest_immediate.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	test_immediate, .-test_immediate
	.globl	test_variable
	.type	test_variable, @function
test_variable:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -16
	str	w0, [x29, #24]
	str	w1, [x29, #28]
	ldr	w19, [x29, #24]
	ldr	w0, [x29, #28]
	and	w0, w19, w0
	cbz	w0, .Ltest_variable.2
	mov	w0, #1
	b	.Ltest_variable.1
.Ltest_variable.2:
	mov	w0, #0
	b	.Ltest_variable.1
.Ltest_variable.1:
	.cfi_def_cfa wsp, 32
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	test_variable, .-test_variable
	.globl	add_to_local
	.type	add_to_local, @function
add_to_local:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	w1, [x29, #20]
	ldr	w0, [x29, #16]
	str	w0, [x29, #24]
	ldr	w0, [x29, #20]
	ldr	w1, [x29, #24]
	add	w1, w1, w0
	str	w1, [x29, #24]
	cbz	w1, .Ladd_to_local.2
	mov	w0, #1
	b	.Ladd_to_local.1
.Ladd_to_local.2:
	mov	w0, #0
	b	.Ladd_to_local.1
.Ladd_to_local.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	add_to_local, .-add_to_local
	.globl	add_to_char
	.type	add_to_char, @function
add_to_char:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	w1, [x29, #20]
	ldr	w0, [x29, #16]
	strb	w0, [x29, #24]
	ldr	w0, [x29, #20]
	ldrb	w1, [x29, #24]
	add	w1, w1, w0
	strb	w1, [x29, #24]
	tst	w1, #0xff
	b.eq	.Ladd_to_char.2
	mov	w0, #1
	b	.Ladd_to_char.1
.Ladd_to_char.2:
	mov	w0, #0
	b	.Ladd_to_char.1
.Ladd_to_char.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	add_to_char, .-add_to_char
	.globl	add_to_global
	.type	add_to_global, @function
add_to_global:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	ldr	w0, [x29, #16]
	adrp	x16, counter
	ldr	w1, [x16, :lo12:counter]
	add	w1, w1, w0
	adrp	x16, counter
	str	w1, [x16, :lo12:counter]
	cbz	w1, .Ladd_to_global.2
	mov	w0, #1
	b	.Ladd_to_global.1
.Ladd_to_global.2:
	mov	w0, #0
	b	.Ladd_to_global.1
.Ladd_to_global.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	add_to_global, .-add_to_global
	.globl	add_to_register
	.type	add_to_register, @function
add_to_register:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -16
	str	w0, [x29, #24]
	str	w1, [x29, #28]
	ldr	w19, [x29, #24]
	ldr	w0, [x29, #28]
	add	w19, w19, w0
	cbz	w19, .Ladd_to_register.2
	mov	w0, #1
	b	.Ladd_to_register.1
.Ladd_to_register.2:
	mov	w0, #0
	b	.Ladd_to_register.1
.Ladd_to_register.1:
	.cfi_def_cfa wsp, 32
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	add_to_register, .-add_to_register
	.globl	sub_from_local
	.type	sub_from_local, @function
sub_from_local:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	w1, [x29, #20]
	ldr	w0, [x29, #16]
	str	w0, [x29, #24]
	ldr	w0, [x29, #20]
	ldr	w1, [x29, #24]
	sub	w1, w1, w0
	str	w1, [x29, #24]
	cbz	w1, .Lsub_from_local.2
	mov	w0, #1
	b	.Lsub_from_local.1
.Lsub_from_local.2:
	mov	w0, #0
	b	.Lsub_from_local.1
.Lsub_from_local.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	sub_from_local, .-sub_from_local
	.globl	sub_from_char
	.type	sub_from_char, @function
sub_from_char:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	w1, [x29, #20]
	ldr	w0, [x29, #16]
	strb	w0, [x29, #24]
	ldr	w0, [x29, #20]
	ldrb	w1, [x29, #24]
	sub	w1, w1, w0
	strb	w1, [x29, #24]
	tst	w1, #0xff
	b.eq	.Lsub_from_char.2
	mov	w0, #1
	b	.Lsub_from_char.1
.Lsub_from_char.2:
	mov	w0, #0
	b	.Lsub_from_char.1
.Lsub_from_char.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	sub_from_char, .-sub_from_char
	.globl	sub_from_global
	.type	sub_from_global, @function
sub_from_global:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	ldr	w0, [x29, #16]
	adrp	x16, counter
	ldr	w1, [x16, :lo12:counter]
	sub	w1, w1, w0
	adrp	x16, counter
	str	w1, [x16, :lo12:counter]
	cbz	w1, .Lsub_from_global.2
	mov	w0, #1
	b	.Lsub_from_global.1
.Lsub_from_global.2:
	mov	w0, #0
	b	.Lsub_from_global.1
.Lsub_from_global.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	sub_from_global, .-sub_from_global
	.globl	sub_from_register
	.type	sub_from_register, @function
sub_from_register:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -16
	str	w0, [x29, #24]
	str	w1, [x29, #28]
	ldr	w19, [x29, #24]
	ldr	w0, [x29, #28]
	sub	w19, w19, w0
	cbz	w19, .Lsub_from_register.2
	mov	w0, #1
	b	.Lsub_from_register.1
.Lsub_from_register.2:
	mov	w0, #0
	b	.Lsub_from_register.1
.Lsub_from_register.1:
	.cfi_def_cfa wsp, 32
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	sub_from_register, .-sub_from_register
	.globl	and_to_local
	.type	and_to_local, @function
and_to_local:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	w1, [x29, #20]
	ldr	w0, [x29, #16]
	str	w0, [x29, #24]
	ldr	w0, [x29, #20]
	ldr	w1, [x29, #24]
	and	w1, w1, w0
	str	w1, [x29, #24]
	cbz	w1, .Land_to_local.2
	mov	w0, #1
	b	.Land_to_local.1
.Land_to_local.2:
	mov	w0, #0
	b	.Land_to_local.1
.Land_to_local.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	and_to_local, .-and_to_local
	.globl	and_to_char
	.type	and_to_char, @function
and_to_char:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	w1, [x29, #20]
	ldr	w0, [x29, #16]
	strb	w0, [x29, #24]
	ldr	w0, [x29, #20]
	ldrb	w1, [x29, #24]
	and	w1, w1, w0
	strb	w1, [x29, #24]
	tst	w1, #0xff
	b.eq	.Land_to_char.2
	mov	w0, #1
	b	.Land_to_char.1
.Land_to_char.2:
	mov	w0, #0
	b	.Land_to_char.1
.Land_to_char.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	and_to_char, .-and_to_char
	.globl	and_to_global
	.type	and_to_global, @function
and_to_global:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	ldr	w0, [x29, #16]
	adrp	x16, counter
	ldr	w1, [x16, :lo12:counter]
	and	w1, w1, w0
	adrp	x16, counter
	str	w1, [x16, :lo12:counter]
	cbz	w1, .Land_to_global.2
	mov	w0, #1
	b	.Land_to_global.1
.Land_to_global.2:
	mov	w0, #0
	b	.Land_to_global.1
.Land_to_global.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	and_to_global, .-and_to_global
	.globl	and_to_register
	.type	and_to_register, @function
and_to_register:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -16
	str	w0, [x29, #24]
	str	w1, [x29, #28]
	ldr	w19, [x29, #24]
	ldr	w0, [x29, #28]
	and	w19, w19, w0
	cbz	w19, .Land_to_register.2
	mov	w0, #1
	b	.Land_to_register.1
.Land_to_register.2:
	mov	w0, #0
	b	.Land_to_register.1
.Land_to_register.1:
	.cfi_def_cfa wsp, 32
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	and_to_register, .-and_to_register
	.globl	or_to_local
	.type	or_to_local, @function
or_to_local:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	w1, [x29, #20]
	ldr	w0, [x29, #16]
	str	w0, [x29, #24]
	ldr	w0, [x29, #20]
	ldr	w1, [x29, #24]
	orr	w1, w1, w0
	str	w1, [x29, #24]
	cbz	w1, .Lor_to_local.2
	mov	w0, #1
	b	.Lor_to_local.1
.Lor_to_local.2:
	mov	w0, #0
	b	.Lor_to_local.1
.Lor_to_local.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	or_to_local, .-or_to_local
	.globl	or_to_char
	.type	or_to_char, @function
or_to_char:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	w1, [x29, #20]
	ldr	w0, [x29, #16]
	strb	w0, [x29, #24]
	ldr	w0, [x29, #20]
	ldrb	w1, [x29, #24]
	orr	w1, w1, w0
	strb	w1, [x29, #24]
	tst	w1, #0xff
	b.eq	.Lor_to_char.2
	mov	w0, #1
	b	.Lor_to_char.1
.Lor_to_char.2:
	mov	w0, #0
	b	.Lor_to_char.1
.Lor_to_char.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	or_to_char, .-or_to_char
	.globl	or_to_global
	.type	or_to_global, @function
or_to_global:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	ldr	w0, [x29, #16]
	adrp	x16, counter
	ldr	w1, [x16, :lo12:counter]
	orr	w1, w1, w0
	adrp	x16, counter
	str	w1, [x16, :lo12:counter]
	cbz	w1, .Lor_to_global.2
	mov	w0, #1
	b	.Lor_to_global.1
.Lor_to_global.2:
	mov	w0, #0
	b	.Lor_to_global.1
.Lor_to_global.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	or_to_global, .-or_to_global
	.globl	or_to_register
	.type	or_to_register, @function
or_to_register:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -16
	str	w0, [x29, #24]
	str	w1, [x29, #28]
	ldr	w19, [x29, #24]
	ldr	w0, [x29, #28]
	orr	w19, w19, w0
	cbz	w19, .Lor_to_register.2
	mov	w0, #1
	b	.Lor_to_register.1
.Lor_to_register.2:
	mov	w0, #0
	b	.Lor_to_register.1
.Lor_to_register.1:
	.cfi_def_cfa wsp, 32
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	or_to_register, .-or_to_register
	.globl	xor_to_local
	.type	xor_to_local, @function
xor_to_local:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	w1, [x29, #20]
	ldr	w0, [x29, #16]
	str	w0, [x29, #24]
	ldr	w0, [x29, #20]
	ldr	w1, [x29, #24]
	eor	w1, w1, w0
	str	w1, [x29, #24]
	cbz	w1, .Lxor_to_local.2
	mov	w0, #1
	b	.Lxor_to_local.1
.Lxor_to_local.2:
	mov	w0, #0
	b	.Lxor_to_local.1
.Lxor_to_local.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	xor_to_local, .-xor_to_local
	.globl	xor_to_char
	.type	xor_to_char, @function
xor_to_char:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	w1, [x29, #20]
	ldr	w0, [x29, #16]
	strb	w0, [x29, #24]
	ldr	w0, [x29, #20]
	ldrb	w1, [x29, #24]
	eor	w1, w1, w0
	strb	w1, [x29, #24]
	tst	w1, #0xff
	b.eq	.Lxor_to_char.2
	mov	w0, #1
	b	.Lxor_to_char.1
.Lxor_to_char.2:
	mov	w0, #0
	b	.Lxor_to_char.1
.Lxor_to_char.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	xor_to_char, .-xor_to_char
	.globl	xor_to_global
	.type	xor_to_global, @function
xor_to_global:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	ldr	w0, [x29, #16]
	adrp	x16, counter
	ldr	w1, [x16, :lo12:counter]
	eor	w1, w1, w0
	adrp	x16, counter
	str	w1, [x16, :lo12:counter]
	cbz	w1, .Lxor_to_global.2
	mov	w0, #1
	b	.Lxor_to_global.1
.Lxor_to_global.2:
	mov	w0, #0
	b	.Lxor_to_global.1
.Lxor_to_global.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	xor_to_global, .-xor_to_global
	.globl	xor_to_register
	.type	xor_to_register, @function
xor_to_register:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -16
	str	w0, [x29, #24]
	str	w1, [x29, #28]
	ldr	w19, [x29, #24]
	ldr	w0, [x29, #28]
	eor	w19, w19, w0
	cbz	w19, .Lxor_to_register.2
	mov	w0, #1
	b	.Lxor_to_register.1
.Lxor_to_register.2:
	mov	w0, #0
	b	.Lxor_to_register.1
.Lxor_to_register.1:
	.cfi_def_cfa wsp, 32
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	xor_to_register, .-xor_to_register
	.globl	inc_local
	.type	inc_local, @function
inc_local:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	ldr	w0, [x29, #16]
	str	w0, [x29, #20]
	ldr	w0, [x29, #20]
	add	w0, w0, #1
	str	w0, [x29, #20]
	cbz	w0, .Linc_local.2
	mov	w0, #1
	b	.Linc_local.1
.Linc_local.2:
	mov	w0, #0
	b	.Linc_local.1
.Linc_local.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	inc_local, .-inc_local
	.globl	inc_char
	.type	inc_char, @function
inc_char:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	ldr	w0, [x29, #16]
	strb	w0, [x29, #20]
	ldrb	w0, [x29, #20]
	add	w0, w0, #1
	strb	w0, [x29, #20]
	tst	w0, #0xff
	b.eq	.Linc_char.2
	mov	w0, #1
	b	.Linc_char.1
.Linc_char.2:
	mov	w0, #0
	b	.Linc_char.1
.Linc_char.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	inc_char, .-inc_char
	.globl	inc_global
	.type	inc_global, @function
inc_global:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, counter
	ldr	w0, [x16, :lo12:counter]
	add	w0, w0, #1
	adrp	x16, counter
	str	w0, [x16, :lo12:counter]
	cbz	w0, .Linc_global.2
	mov	w0, #1
	b	.Linc_global.1
.Linc_global.2:
	mov	w0, #0
	b	.Linc_global.1
.Linc_global.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	inc_global, .-inc_global
	.globl	inc_register
	.type	inc_register, @function
inc_register:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -16
	str	w0, [x29, #24]
	ldr	w19, [x29, #24]
	add	w19, w19, #1
	cbz	w19, .Linc_register.2
	mov	w0, #1
	b	.Linc_register.1
.Linc_register.2:
	mov	w0, #0
	b	.Linc_register.1
.Linc_register.1:
	.cfi_def_cfa wsp, 32
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	inc_register, .-inc_register
	.globl	dec_local
	.type	dec_local, @function
dec_local:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	ldr	w0, [x29, #16]
	str	w0, [x29, #20]
	ldr	w0, [x29, #20]
	sub	w0, w0, #1
	str	w0, [x29, #20]
	cbz	w0, .Ldec_local.2
	mov	w0, #1
	b	.Ldec_local.1
.Ldec_local.2:
	mov	w0, #0
	b	.Ldec_local.1
.Ldec_local.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	dec_local, .-dec_local
	.globl	dec_char
	.type	dec_char, @function
dec_char:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	ldr	w0, [x29, #16]
	strb	w0, [x29, #20]
	ldrb	w0, [x29, #20]
	sub	w0, w0, #1
	strb	w0, [x29, #20]
	tst	w0, #0xff
	b.eq	.Ldec_char.2
	mov	w0, #1
	b	.Ldec_char.1
.Ldec_char.2:
	mov	w0, #0
	b	.Ldec_char.1
.Ldec_char.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	dec_char, .-dec_char
	.globl	dec_global
	.type	dec_global, @function
dec_global:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, counter
	ldr	w0, [x16, :lo12:counter]
	sub	w0, w0, #1
	adrp	x16, counter
	str	w0, [x16, :lo12:counter]
	cbz	w0, .Ldec_global.2
	mov	w0, #1
	b	.Ldec_global.1
.Ldec_global.2:
	mov	w0, #0
	b	.Ldec_global.1
.Ldec_global.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	dec_global, .-dec_global
	.globl	dec_register
	.type	dec_register, @function
dec_register:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -16
	str	w0, [x29, #24]
	ldr	w19, [x29, #24]
	sub	w19, w19, #1
	cbz	w19, .Ldec_register.2
	mov	w0, #1
	b	.Ldec_register.1
.Ldec_register.2:
	mov	w0, #0
	b	.Ldec_register.1
.Ldec_register.1:
	.cfi_def_cfa wsp, 32
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	dec_register, .-dec_register
	.globl	equal_zero
	.type	equal_zero, @function
equal_zero:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	w1, [x29, #20]
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #20]
	and	w0, w0, w1
	cmp	w0, #0
	b.ne	.Lequal_zero.2
	mov	w0, #1
	b	.Lequal_zero.1
.Lequal_zero.2:
	mov	w0, #0
	b	.Lequal_zero.1
.Lequal_zero.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	equal_zero, .-equal_zero
	.globl	unequal_zero
	.type	unequal_zero, @function
unequal_zero:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	w1, [x29, #20]
	ldr	w0, [x29, #16]
	str	w0, [x29, #24]
	ldr	w0, [x29, #20]
	ldr	w1, [x29, #24]
	sub	w1, w1, w0
	str	w1, [x29, #24]
	cmp	w1, #0
	b.eq	.Lunequal_zero.2
	mov	w0, #1
	b	.Lunequal_zero.1
.Lunequal_zero.2:
	mov	w0, #0
	b	.Lunequal_zero.1
.Lunequal_zero.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	unequal_zero, .-unequal_zero
	.globl	product
	.type	product, @function
product:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	w1, [x29, #20]
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #20]
	mul	w0, w0, w1
	cbz	w0, .Lproduct.2
	mov	w0, #1
	b	.Lproduct.1
.Lproduct.2:
	mov	w0, #0
	b	.Lproduct.1
.Lproduct.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	product, .-product
	.globl	shift
	.type	shift, @function
shift:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	w1, [x29, #20]
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #20]
	lsl	w0, w0, w1
	cbz	w0, .Lshift.2
	mov	w0, #1
	b	.Lshift.1
.Lshift.2:
	mov	w0, #0
	b	.Lshift.1
.Lshift.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	shift, .-shift
	.globl	postfix
	.type	postfix, @function
postfix:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	ldr	w0, [x29, #16]
	str	w0, [x29, #20]
	ldr	w0, [x29, #20]
	add	w1, w0, #1
	str	w1, [x29, #20]
	cbz	w0, .Lpostfix.2
	mov	w0, #1
	b	.Lpostfix.1
.Lpostfix.2:
	mov	w0, #0
	b	.Lpostfix.1
.Lpostfix.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	postfix, .-postfix
	.globl	narrow_register
	.type	narrow_register, @function
narrow_register:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -16
	str	w0, [x29, #24]
	ldr	w0, [x29, #24]
	uxtb	w19, w0
	add	w19, w19, #1
	uxtb	w19, w19
	cbz	w19, .Lnarrow_register.2
	mov	w0, #1
	b	.Lnarrow_register.1
.Lnarrow_register.2:
	mov	w0, #0
	b	.Lnarrow_register.1
.Lnarrow_register.1:
	.cfi_def_cfa wsp, 32
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	narrow_register, .-narrow_register
	.globl	negative
	.type	negative, @function
negative:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	w1, [x29, #20]
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #20]
	add	w0, w0, w1
	cmp	w0, #0
	b.ge	.Lnegative.2
	mov	w0, #1
	b	.Lnegative.1
.Lnegative.2:
	mov	w0, #0
	b	.Lnegative.1
.Lnegative.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
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
