	.file	"operand.c"
	.text
	.globl	add
	.type	add, @function
add:
	.cfi_startproc
	sub	sp, sp, #64
	.cfi_def_cfa_offset 64
	stp	x29, x30, [sp]
	.cfi_offset w29, -64
	.cfi_offset w30, -56
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	x1, [x29, #24]
	str	w2, [x29, #32]
	ldr	w0, [x29, #16]
	str	w0, [x29, #36]
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #36]
	add	w0, w0, w1
	adrp	x16, defined
	ldr	w1, [x16, :lo12:defined]
	add	w0, w0, w1
	adrp	x1, :got:declared
	ldr	x1, [x1, :got_lo12:declared]
	ldr	w1, [x1]
	add	w0, w0, w1
	ldr	x1, [x29, #24]
	ldr	w1, [x1]
	add	w0, w0, w1
	add	x1, x29, #40
	ldr	w2, [x29, #32]
	ldr	w1, [x1, w2, sxtw #2]
	add	w0, w0, w1
	b	.Ladd.1
.Ladd.1:
	.cfi_def_cfa wsp, 64
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #64
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	add, .-add
	.globl	sub
	.type	sub, @function
sub:
	.cfi_startproc
	sub	sp, sp, #64
	.cfi_def_cfa_offset 64
	stp	x29, x30, [sp]
	.cfi_offset w29, -64
	.cfi_offset w30, -56
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	x1, [x29, #24]
	str	w2, [x29, #32]
	ldr	w0, [x29, #16]
	str	w0, [x29, #36]
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #36]
	sub	w0, w0, w1
	adrp	x16, defined
	ldr	w1, [x16, :lo12:defined]
	sub	w0, w0, w1
	adrp	x1, :got:declared
	ldr	x1, [x1, :got_lo12:declared]
	ldr	w1, [x1]
	sub	w0, w0, w1
	ldr	x1, [x29, #24]
	ldr	w1, [x1]
	sub	w0, w0, w1
	add	x1, x29, #40
	ldr	w2, [x29, #32]
	ldr	w1, [x1, w2, sxtw #2]
	sub	w0, w0, w1
	b	.Lsub.1
.Lsub.1:
	.cfi_def_cfa wsp, 64
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #64
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	sub, .-sub
	.globl	and
	.type	and, @function
and:
	.cfi_startproc
	sub	sp, sp, #64
	.cfi_def_cfa_offset 64
	stp	x29, x30, [sp]
	.cfi_offset w29, -64
	.cfi_offset w30, -56
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	x1, [x29, #24]
	str	w2, [x29, #32]
	ldr	w0, [x29, #16]
	str	w0, [x29, #36]
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #36]
	and	w0, w0, w1
	adrp	x16, defined
	ldr	w1, [x16, :lo12:defined]
	and	w0, w0, w1
	adrp	x1, :got:declared
	ldr	x1, [x1, :got_lo12:declared]
	ldr	w1, [x1]
	and	w0, w0, w1
	ldr	x1, [x29, #24]
	ldr	w1, [x1]
	and	w0, w0, w1
	add	x1, x29, #40
	ldr	w2, [x29, #32]
	ldr	w1, [x1, w2, sxtw #2]
	and	w0, w0, w1
	b	.Land.1
.Land.1:
	.cfi_def_cfa wsp, 64
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #64
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	and, .-and
	.globl	or
	.type	or, @function
or:
	.cfi_startproc
	sub	sp, sp, #64
	.cfi_def_cfa_offset 64
	stp	x29, x30, [sp]
	.cfi_offset w29, -64
	.cfi_offset w30, -56
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	x1, [x29, #24]
	str	w2, [x29, #32]
	ldr	w0, [x29, #16]
	str	w0, [x29, #36]
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #36]
	orr	w0, w0, w1
	adrp	x16, defined
	ldr	w1, [x16, :lo12:defined]
	orr	w0, w0, w1
	adrp	x1, :got:declared
	ldr	x1, [x1, :got_lo12:declared]
	ldr	w1, [x1]
	orr	w0, w0, w1
	ldr	x1, [x29, #24]
	ldr	w1, [x1]
	orr	w0, w0, w1
	add	x1, x29, #40
	ldr	w2, [x29, #32]
	ldr	w1, [x1, w2, sxtw #2]
	orr	w0, w0, w1
	b	.Lor.1
.Lor.1:
	.cfi_def_cfa wsp, 64
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #64
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	or, .-or
	.globl	xor
	.type	xor, @function
xor:
	.cfi_startproc
	sub	sp, sp, #64
	.cfi_def_cfa_offset 64
	stp	x29, x30, [sp]
	.cfi_offset w29, -64
	.cfi_offset w30, -56
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	x1, [x29, #24]
	str	w2, [x29, #32]
	ldr	w0, [x29, #16]
	str	w0, [x29, #36]
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #36]
	eor	w0, w0, w1
	adrp	x16, defined
	ldr	w1, [x16, :lo12:defined]
	eor	w0, w0, w1
	adrp	x1, :got:declared
	ldr	x1, [x1, :got_lo12:declared]
	ldr	w1, [x1]
	eor	w0, w0, w1
	ldr	x1, [x29, #24]
	ldr	w1, [x1]
	eor	w0, w0, w1
	add	x1, x29, #40
	ldr	w2, [x29, #32]
	ldr	w1, [x1, w2, sxtw #2]
	eor	w0, w0, w1
	b	.Lxor.1
.Lxor.1:
	.cfi_def_cfa wsp, 64
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #64
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	xor, .-xor
	.globl	mul
	.type	mul, @function
mul:
	.cfi_startproc
	sub	sp, sp, #64
	.cfi_def_cfa_offset 64
	stp	x29, x30, [sp]
	.cfi_offset w29, -64
	.cfi_offset w30, -56
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	x1, [x29, #24]
	str	w2, [x29, #32]
	ldr	w0, [x29, #16]
	str	w0, [x29, #36]
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #36]
	mul	w0, w0, w1
	adrp	x16, defined
	ldr	w1, [x16, :lo12:defined]
	mul	w0, w0, w1
	adrp	x1, :got:declared
	ldr	x1, [x1, :got_lo12:declared]
	ldr	w1, [x1]
	mul	w0, w0, w1
	ldr	x1, [x29, #24]
	ldr	w1, [x1]
	mul	w0, w0, w1
	add	x1, x29, #40
	ldr	w2, [x29, #32]
	ldr	w1, [x1, w2, sxtw #2]
	mul	w0, w0, w1
	b	.Lmul.1
.Lmul.1:
	.cfi_def_cfa wsp, 64
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #64
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	mul, .-mul
	.globl	divide
	.type	divide, @function
divide:
	.cfi_startproc
	sub	sp, sp, #64
	.cfi_def_cfa_offset 64
	stp	x29, x30, [sp]
	.cfi_offset w29, -64
	.cfi_offset w30, -56
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	x1, [x29, #24]
	str	w2, [x29, #32]
	ldr	w0, [x29, #16]
	str	w0, [x29, #36]
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #36]
	sdiv	w0, w0, w1
	adrp	x16, defined
	ldr	w1, [x16, :lo12:defined]
	sdiv	w0, w0, w1
	adrp	x1, :got:declared
	ldr	x1, [x1, :got_lo12:declared]
	ldr	w1, [x1]
	sdiv	w0, w0, w1
	ldr	x1, [x29, #24]
	ldr	w1, [x1]
	sdiv	w0, w0, w1
	add	x1, x29, #40
	ldr	w2, [x29, #32]
	ldr	w1, [x1, w2, sxtw #2]
	sdiv	w0, w0, w1
	b	.Ldivide.1
.Ldivide.1:
	.cfi_def_cfa wsp, 64
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #64
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	divide, .-divide
	.globl	modulo
	.type	modulo, @function
modulo:
	.cfi_startproc
	sub	sp, sp, #64
	.cfi_def_cfa_offset 64
	stp	x29, x30, [sp]
	.cfi_offset w29, -64
	.cfi_offset w30, -56
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	x1, [x29, #24]
	str	w2, [x29, #32]
	ldr	w0, [x29, #16]
	str	w0, [x29, #36]
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #36]
	sdiv	w2, w0, w1
	msub	w0, w2, w1, w0
	adrp	x16, defined
	ldr	w1, [x16, :lo12:defined]
	sdiv	w2, w0, w1
	msub	w0, w2, w1, w0
	adrp	x1, :got:declared
	ldr	x1, [x1, :got_lo12:declared]
	ldr	w1, [x1]
	sdiv	w2, w0, w1
	msub	w0, w2, w1, w0
	ldr	x1, [x29, #24]
	ldr	w1, [x1]
	sdiv	w2, w0, w1
	msub	w0, w2, w1, w0
	add	x1, x29, #40
	ldr	w2, [x29, #32]
	ldr	w1, [x1, w2, sxtw #2]
	sdiv	w2, w0, w1
	msub	w0, w2, w1, w0
	b	.Lmodulo.1
.Lmodulo.1:
	.cfi_def_cfa wsp, 64
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #64
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	modulo, .-modulo
	.globl	less
	.type	less, @function
less:
	.cfi_startproc
	sub	sp, sp, #64
	.cfi_def_cfa_offset 64
	stp	x29, x30, [sp]
	.cfi_offset w29, -64
	.cfi_offset w30, -56
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	x1, [x29, #24]
	str	w2, [x29, #32]
	ldr	w0, [x29, #16]
	str	w0, [x29, #36]
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #36]
	cmp	w0, w1
	cset	w0, lt
	ldr	w1, [x29, #16]
	adrp	x16, defined
	ldr	w2, [x16, :lo12:defined]
	cmp	w1, w2
	cset	w1, lt
	add	w0, w0, w1
	ldr	w1, [x29, #16]
	adrp	x2, :got:declared
	ldr	x2, [x2, :got_lo12:declared]
	ldr	w2, [x2]
	cmp	w1, w2
	cset	w1, lt
	add	w0, w0, w1
	ldr	w1, [x29, #16]
	ldr	x2, [x29, #24]
	ldr	w2, [x2]
	cmp	w1, w2
	cset	w1, lt
	add	w0, w0, w1
	ldr	w1, [x29, #16]
	add	x2, x29, #40
	ldr	w3, [x29, #32]
	ldr	w2, [x2, w3, sxtw #2]
	cmp	w1, w2
	cset	w1, lt
	add	w0, w0, w1
	b	.Lless.1
.Lless.1:
	.cfi_def_cfa wsp, 64
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #64
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	less, .-less
	.globl	test
	.type	test, @function
test:
	.cfi_startproc
	sub	sp, sp, #64
	.cfi_def_cfa_offset 64
	stp	x29, x30, [sp]
	.cfi_offset w29, -64
	.cfi_offset w30, -56
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	x1, [x29, #24]
	str	w2, [x29, #32]
	ldr	w0, [x29, #16]
	str	w0, [x29, #36]
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #36]
	and	w0, w0, w1
	cbz	w0, .Ltest.2
	ldr	w0, [x29, #16]
	adrp	x16, defined
	ldr	w1, [x16, :lo12:defined]
	and	w0, w0, w1
	cbz	w0, .Ltest.2
	ldr	w0, [x29, #16]
	adrp	x1, :got:declared
	ldr	x1, [x1, :got_lo12:declared]
	ldr	w1, [x1]
	and	w0, w0, w1
	cbz	w0, .Ltest.2
	ldr	w0, [x29, #16]
	ldr	x1, [x29, #24]
	ldr	w1, [x1]
	and	w0, w0, w1
	cbz	w0, .Ltest.2
	ldr	w0, [x29, #16]
	add	x1, x29, #40
	ldr	w2, [x29, #32]
	ldr	w1, [x1, w2, sxtw #2]
	and	w0, w0, w1
	cbz	w0, .Ltest.2
	mov	w0, #1
	b	.Ltest.1
.Ltest.2:
	mov	w0, #0
	b	.Ltest.1
.Ltest.1:
	.cfi_def_cfa wsp, 64
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #64
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	test, .-test
	.globl	divide_busy
	.type	divide_busy, @function
divide_busy:
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
	str	x2, [x29, #24]
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #20]
	ldr	x2, [x29, #24]
	ldr	w2, [x2]
	sdiv	w1, w1, w2
	add	w0, w0, w1
	b	.Ldivide_busy.1
.Ldivide_busy.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	divide_busy, .-divide_busy
	.globl	less_constant
	.type	less_constant, @function
less_constant:
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
	cmp	w0, #5
	b.ge	.Lless_constant.2
	mov	w0, #1
	b	.Lless_constant.1
.Lless_constant.2:
	mov	w0, #0
	b	.Lless_constant.1
.Lless_constant.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	less_constant, .-less_constant
	.globl	nonzero
	.type	nonzero, @function
nonzero:
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
	cbz	w0, .Lnonzero.2
	mov	w0, #1
	b	.Lnonzero.1
.Lnonzero.2:
	mov	w0, #0
	b	.Lnonzero.1
.Lnonzero.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	nonzero, .-nonzero
	.globl	nonzero_char
	.type	nonzero_char, @function
nonzero_char:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	strb	w0, [x29, #16]
	ldrb	w0, [x29, #16]
	cbz	w0, .Lnonzero_char.2
	mov	w0, #1
	b	.Lnonzero_char.1
.Lnonzero_char.2:
	mov	w0, #0
	b	.Lnonzero_char.1
.Lnonzero_char.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	nonzero_char, .-nonzero_char
	.globl	bit
	.type	bit, @function
bit:
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
	cbz	w0, .Lbit.2
	mov	w0, #1
	b	.Lbit.1
.Lbit.2:
	mov	w0, #0
	b	.Lbit.1
.Lbit.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	bit, .-bit
	.globl	char_operand
	.type	char_operand, @function
char_operand:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	strb	w1, [x29, #20]
	ldr	w0, [x29, #16]
	ldrb	w1, [x29, #20]
	add	w0, w0, w1
	b	.Lchar_operand.1
.Lchar_operand.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	char_operand, .-char_operand
	.globl	widened
	.type	widened, @function
widened:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	w1, [x29, #24]
	ldr	x0, [x29, #16]
	ldrsw	x1, [x29, #24]
	add	x0, x0, x1
	b	.Lwidened.1
.Lwidened.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	widened, .-widened
	.globl	compound
	.type	compound, @function
compound:
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
	ldr	w0, [x29, #20]
	ldr	w1, [x29, #16]
	add	w1, w1, w0
	str	w1, [x29, #16]
	ldr	w0, [x29, #16]
	b	.Lcompound.1
.Lcompound.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	compound, .-compound
	.globl	compound_value
	.type	compound_value, @function
compound_value:
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
	ldr	w0, [x29, #20]
	ldr	w1, [x29, #16]
	add	w0, w1, w0
	str	w0, [x29, #16]
	b	.Lcompound_value.1
.Lcompound_value.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	compound_value, .-compound_value
	.globl	shift_count
	.type	shift_count, @function
shift_count:
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
	b	.Lshift_count.1
.Lshift_count.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	shift_count, .-shift_count
	.globl	times_four
	.type	times_four, @function
times_four:
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
	mov	w1, #4
	mul	w0, w0, w1
	b	.Ltimes_four.1
.Ltimes_four.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	times_four, .-times_four
	.globl	before_call
	.type	before_call, @function
before_call:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, defined
	ldr	w0, [x16, :lo12:defined]
	str	x0, [x29, #16]
	bl	f
	ldr	x1, [x29, #16]
	cmp	w1, w0
	b.ge	.Lbefore_call.2
	mov	w0, #1
	b	.Lbefore_call.1
.Lbefore_call.2:
	mov	w0, #0
	b	.Lbefore_call.1
.Lbefore_call.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	before_call, .-before_call
	.globl	constant_left
	.type	constant_left, @function
constant_left:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	wzr, [x29, #20]
	ldr	w0, [x29, #16]
	cmp	w0, #5
	b.le	.Lconstant_left.2
	ldr	w0, [x29, #20]
	add	w0, w0, #1
	str	w0, [x29, #20]
.Lconstant_left.2:
	ldr	w0, [x29, #16]
	cmp	w0, #5
	b.lt	.Lconstant_left.3
	ldr	w0, [x29, #20]
	add	w0, w0, #2
	str	w0, [x29, #20]
.Lconstant_left.3:
	ldr	w0, [x29, #16]
	cmp	w0, #5
	b.ge	.Lconstant_left.4
	ldr	w0, [x29, #20]
	add	w0, w0, #4
	str	w0, [x29, #20]
.Lconstant_left.4:
	ldr	w0, [x29, #16]
	cmp	w0, #5
	b.gt	.Lconstant_left.5
	ldr	w0, [x29, #20]
	add	w0, w0, #8
	str	w0, [x29, #20]
.Lconstant_left.5:
	ldr	w0, [x29, #16]
	cmp	w0, #5
	b.ne	.Lconstant_left.6
	ldr	w0, [x29, #20]
	add	w0, w0, #16
	str	w0, [x29, #20]
.Lconstant_left.6:
	ldr	w0, [x29, #16]
	cmp	w0, #5
	b.eq	.Lconstant_left.7
	ldr	w0, [x29, #20]
	add	w0, w0, #32
	str	w0, [x29, #20]
.Lconstant_left.7:
	ldr	w0, [x29, #20]
	b	.Lconstant_left.1
.Lconstant_left.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	constant_left, .-constant_left
	.globl	constant_left_unsigned
	.type	constant_left_unsigned, @function
constant_left_unsigned:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	wzr, [x29, #20]
	ldr	w0, [x29, #16]
	cmp	w0, #5
	b.ls	.Lconstant_left_unsigned.2
	ldr	w0, [x29, #20]
	add	w0, w0, #1
	str	w0, [x29, #20]
.Lconstant_left_unsigned.2:
	ldr	w0, [x29, #16]
	cmp	w0, #5
	b.lo	.Lconstant_left_unsigned.3
	ldr	w0, [x29, #20]
	add	w0, w0, #2
	str	w0, [x29, #20]
.Lconstant_left_unsigned.3:
	ldr	w0, [x29, #16]
	cmp	w0, #5
	b.hs	.Lconstant_left_unsigned.4
	ldr	w0, [x29, #20]
	add	w0, w0, #4
	str	w0, [x29, #20]
.Lconstant_left_unsigned.4:
	ldr	w0, [x29, #16]
	cmp	w0, #5
	b.hi	.Lconstant_left_unsigned.5
	ldr	w0, [x29, #20]
	add	w0, w0, #8
	str	w0, [x29, #20]
.Lconstant_left_unsigned.5:
	ldr	w0, [x29, #16]
	cmp	w0, #5
	b.ne	.Lconstant_left_unsigned.6
	ldr	w0, [x29, #20]
	add	w0, w0, #16
	str	w0, [x29, #20]
.Lconstant_left_unsigned.6:
	ldr	w0, [x29, #16]
	cmp	w0, #5
	b.eq	.Lconstant_left_unsigned.7
	ldr	w0, [x29, #20]
	add	w0, w0, #32
	str	w0, [x29, #20]
.Lconstant_left_unsigned.7:
	ldr	w0, [x29, #20]
	b	.Lconstant_left_unsigned.1
.Lconstant_left_unsigned.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	constant_left_unsigned, .-constant_left_unsigned
	.globl	constant_left_register
	.type	constant_left_register, @function
constant_left_register:
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
	cmp	w19, #5
	b.le	.Lconstant_left_register.2
	mov	w0, #1
	b	.Lconstant_left_register.1
.Lconstant_left_register.2:
	mov	w0, #0
	b	.Lconstant_left_register.1
.Lconstant_left_register.1:
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
	.size	constant_left_register, .-constant_left_register
	.globl	constant_left_value
	.type	constant_left_value, @function
constant_left_value:
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
	cmp	w0, #5
	cset	w0, gt
	b	.Lconstant_left_value.1
.Lconstant_left_value.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	constant_left_value, .-constant_left_value
	.globl	null_left
	.type	null_left, @function
null_left:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	ldr	x0, [x29, #16]
	cmp	x0, #0
	b.ne	.Lnull_left.2
	mov	w0, #1
	b	.Lnull_left.1
.Lnull_left.2:
	mov	w0, #0
	b	.Lnull_left.1
.Lnull_left.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	null_left, .-null_left
	.globl	pointer_operand
	.type	pointer_operand, @function
pointer_operand:
	.cfi_startproc
	sub	sp, sp, #64
	.cfi_def_cfa_offset 64
	stp	x29, x30, [sp]
	.cfi_offset w29, -64
	.cfi_offset w30, -56
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	x1, [x29, #24]
	str	x2, [x29, #32]
	str	x3, [x29, #40]
	str	x4, [x29, #48]
	ldr	x0, [x29, #16]
	ldr	x1, [x29, #24]
	sub	x0, x0, x1
	ldr	x1, [x29, #32]
	ldr	x2, [x29, #40]
	sub	x1, x1, x2
	mov	x2, #4
	sdiv	x1, x1, x2
	add	x0, x0, x1
	ldr	x1, [x29, #16]
	ldr	x2, [x29, #48]
	add	x1, x1, x2
	ldr	x2, [x29, #24]
	sub	x1, x1, x2
	add	x0, x0, x1
	ldr	x1, [x29, #32]
	ldr	x2, [x29, #48]
	add	x1, x1, x2, lsl #2
	ldr	x2, [x29, #40]
	sub	x1, x1, x2
	mov	x2, #4
	sdiv	x1, x1, x2
	add	x0, x0, x1
	b	.Lpointer_operand.1
.Lpointer_operand.1:
	.cfi_def_cfa wsp, 64
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #64
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	pointer_operand, .-pointer_operand
	.globl	signedness
	.type	signedness, @function
signedness:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	w1, [x29, #20]
	str	x2, [x29, #24]
	str	w3, [x29, #32]
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #20]
	add	w0, w0, w1
	ldr	w1, [x29, #16]
	ldr	w2, [x29, #20]
	cmp	w1, w2
	cset	w1, lo
	add	w0, w0, w1
	ldr	w1, [x29, #16]
	ldr	w2, [x29, #20]
	mul	w1, w1, w2
	add	w0, w0, w1
	ldr	w1, [x29, #16]
	ldr	w2, [x29, #20]
	udiv	w1, w1, w2
	add	w0, w0, w1
	ldr	x1, [x29, #24]
	ldrsw	x2, [x29, #32]
	add	x1, x1, x2
	add	x0, x0, x1
	b	.Lsignedness.1
.Lsignedness.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	signedness, .-signedness
	.globl	comparison_value
	.type	comparison_value, @function
comparison_value:
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
	str	w2, [x29, #24]
	str	w3, [x29, #28]
	ldr	w0, [x29, #16]
	cmp	w0, #5
	cset	w0, lt
	ldr	w1, [x29, #20]
	cmp	w1, #5
	cset	w1, hs
	add	w0, w0, w1
	ldr	w1, [x29, #16]
	ldr	w2, [x29, #24]
	cmp	w1, w2
	cset	w1, lt
	add	w0, w0, w1
	ldr	w1, [x29, #20]
	ldr	w2, [x29, #28]
	cmp	w1, w2
	cset	w1, hi
	add	w0, w0, w1
	b	.Lcomparison_value.1
.Lcomparison_value.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	comparison_value, .-comparison_value
	.globl	by_constant
	.type	by_constant, @function
by_constant:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	x1, [x29, #24]
	ldr	w0, [x29, #16]
	mov	w1, #4
	mul	w0, w0, w1
	ldr	w1, [x29, #16]
	mov	w2, #-3
	mul	w1, w1, w2
	add	w0, w0, w1
	ldr	w1, [x29, #16]
	mov	w2, #4
	mul	w1, w1, w2
	add	w0, w0, w1
	sxtw	x0, w0
	ldr	x1, [x29, #24]
	mov	x2, #1000
	mul	x1, x1, x2
	add	x0, x0, x1
	b	.Lby_constant.1
.Lby_constant.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	by_constant, .-by_constant
	.globl	commuted
	.type	commuted, @function
commuted:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	strb	w1, [x29, #20]
	ldr	w0, [x29, #16]
	add	w0, w0, #5
	ldr	w1, [x29, #16]
	mov	w2, #4
	mul	w1, w1, w2
	sub	w0, w0, w1
	ldr	w1, [x29, #16]
	and	w1, w1, #0x7
	sub	w0, w0, w1
	ldr	w1, [x29, #16]
	orr	w1, w1, #0x7
	sub	w0, w0, w1
	ldr	w1, [x29, #16]
	eor	w1, w1, #0x7
	sub	w0, w0, w1
	ldrb	w1, [x29, #20]
	add	w1, w1, #5
	sub	w0, w0, w1
	b	.Lcommuted.1
.Lcommuted.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	commuted, .-commuted
	.globl	commuted_right
	.type	commuted_right, @function
commuted_right:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	strb	w1, [x29, #20]
	ldr	w0, [x29, #16]
	add	w0, w0, #5
	ldr	w1, [x29, #16]
	mov	w2, #4
	mul	w1, w1, w2
	sub	w0, w0, w1
	ldr	w1, [x29, #16]
	and	w1, w1, #0x7
	sub	w0, w0, w1
	ldr	w1, [x29, #16]
	orr	w1, w1, #0x7
	sub	w0, w0, w1
	ldr	w1, [x29, #16]
	eor	w1, w1, #0x7
	sub	w0, w0, w1
	ldrb	w1, [x29, #20]
	add	w1, w1, #5
	sub	w0, w0, w1
	b	.Lcommuted_right.1
.Lcommuted_right.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	commuted_right, .-commuted_right
	.globl	commuted_register
	.type	commuted_register, @function
commuted_register:
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
	add	w0, w19, #5
	mov	w1, #4
	mul	w1, w19, w1
	sub	w0, w0, w1
	and	w1, w19, #0x7
	sub	w0, w0, w1
	orr	w1, w19, #0x7
	sub	w0, w0, w1
	eor	w1, w19, #0x7
	sub	w0, w0, w1
	b	.Lcommuted_register.1
.Lcommuted_register.1:
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
	.size	commuted_register, .-commuted_register
	.globl	commuted_call
	.type	commuted_call, @function
commuted_call:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	bl	f
	add	w0, w0, #5
	b	.Lcommuted_call.1
.Lcommuted_call.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	commuted_call, .-commuted_call
	.globl	commuted_into
	.type	commuted_into, @function
commuted_into:
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
	mov	w0, #4
	mul	w19, w19, w0
	add	w19, w19, #5
	mov	w0, w19
	b	.Lcommuted_into.1
.Lcommuted_into.1:
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
	.size	commuted_into, .-commuted_into
	.globl	commuted_bit
	.type	commuted_bit, @function
commuted_bit:
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
	cbz	w0, .Lcommuted_bit.2
	mov	w0, #1
	b	.Lcommuted_bit.1
.Lcommuted_bit.2:
	mov	w0, #0
	b	.Lcommuted_bit.1
.Lcommuted_bit.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	commuted_bit, .-commuted_bit
	.globl	commuted_wide
	.type	commuted_wide, @function
commuted_wide:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	ldr	x0, [x29, #16]
	mov	x1, #4294967296
	add	x0, x0, x1
	b	.Lcommuted_wide.1
.Lcommuted_wide.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	commuted_wide, .-commuted_wide
	.globl	commuted_char_register
	.type	commuted_char_register, @function
commuted_char_register:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	stp	x19, x20, [x29, #16]
	.cfi_offset w19, -32
	.cfi_offset w20, -24
	strb	w0, [x29, #32]
	ldrb	w19, [x29, #32]
	add	w20, w19, #5
	mov	w0, w20
	b	.Lcommuted_char_register.1
.Lcommuted_char_register.1:
	.cfi_def_cfa wsp, 48
	ldp	x19, x20, [x29, #16]
	.cfi_restore w19
	.cfi_restore w20
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	commuted_char_register, .-commuted_char_register
	.globl	not_commuted
	.type	not_commuted, @function
not_commuted:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	mov	w0, #5
	ldr	w1, [x29, #16]
	sub	w0, w0, w1
	mov	w1, #40
	ldr	w2, [x29, #16]
	sdiv	w1, w1, w2
	add	w0, w0, w1
	mov	w1, #1
	ldr	w2, [x29, #16]
	lsl	w1, w1, w2
	add	w0, w0, w1
	b	.Lnot_commuted.1
.Lnot_commuted.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	not_commuted, .-not_commuted
	.globl	index_sub
	.type	index_sub, @function
index_sub:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	x1, [x29, #24]
	ldr	x0, [x29, #16]
	ldr	x1, [x29, #24]
	sub	x0, x0, x1, lsl #2
	b	.Lindex_sub.1
.Lindex_sub.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	index_sub, .-index_sub
	.globl	index_row
	.type	index_row, @function
index_row:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	x1, [x29, #24]
	ldr	x0, [x29, #16]
	ldr	x1, [x29, #24]
	mov	x2, #12
	mul	x1, x1, x2
	add	x0, x0, x1
	b	.Lindex_row.1
.Lindex_row.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	index_row, .-index_row
	.globl	index_first
	.type	index_first, @function
index_first:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	x1, [x29, #24]
	ldr	x0, [x29, #24]
	ldr	x1, [x29, #16]
	mov	x2, #12
	mul	x0, x0, x2
	add	x0, x0, x1
	b	.Lindex_first.1
.Lindex_first.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	index_first, .-index_first
	.globl	index_compound
	.type	index_compound, @function
index_compound:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	x1, [x29, #24]
	ldr	x0, [x29, #24]
	ldr	x1, [x29, #16]
	sub	x1, x1, x0, lsl #2
	str	x1, [x29, #16]
	ldr	x0, [x29, #16]
	b	.Lindex_compound.1
.Lindex_compound.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	index_compound, .-index_compound
	.globl	index_int
	.type	index_int, @function
index_int:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	w1, [x29, #24]
	ldr	x0, [x29, #16]
	ldr	w1, [x29, #24]
	sub	x0, x0, w1, sxtw #2
	b	.Lindex_int.1
.Lindex_int.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	index_int, .-index_int
	.globl	index_scaled
	.type	index_scaled, @function
index_scaled:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	x1, [x29, #24]
	ldr	x0, [x29, #16]
	ldr	x1, [x29, #24]
	add	x0, x0, x1, lsl #2
	b	.Lindex_scaled.1
.Lindex_scaled.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	index_scaled, .-index_scaled
	.globl	widen_signedness
	.type	widen_signedness, @function
widen_signedness:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	ldrsw	x0, [x29, #16]
	b	.Lwiden_signedness.1
.Lwiden_signedness.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	widen_signedness, .-widen_signedness
	.globl	widen_unsigned
	.type	widen_unsigned, @function
widen_unsigned:
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
	b	.Lwiden_unsigned.1
.Lwiden_unsigned.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	widen_unsigned, .-widen_unsigned
	.globl	widen_narrowed
	.type	widen_narrowed, @function
widen_narrowed:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	ldr	x0, [x29, #16]
	sxtw	x0, w0
	b	.Lwiden_narrowed.1
.Lwiden_narrowed.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	widen_narrowed, .-widen_narrowed
	.globl	compare_register
	.type	compare_register, @function
compare_register:
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
	ldr	w19, [x29, #28]
	ldr	w0, [x29, #24]
	cmp	w0, w19
	b.ge	.Lcompare_register.2
	ldr	w0, [x29, #24]
	cmp	w0, w19
	cset	w0, ne
	b	.Lcompare_register.1
.Lcompare_register.2:
	ldr	w0, [x29, #24]
	add	w1, w19, #1
	cmp	w0, w1
	cset	w0, eq
	b	.Lcompare_register.1
.Lcompare_register.1:
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
	.size	compare_register, .-compare_register
	.globl	constant_index
	.type	constant_index, @function
constant_index:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	ldr	x0, [x29, #16]
	add	x0, x0, #12
	b	.Lconstant_index.1
.Lconstant_index.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	constant_index, .-constant_index
	.globl	constant_index_right
	.type	constant_index_right, @function
constant_index_right:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	ldr	x0, [x29, #16]
	add	x0, x0, #12
	b	.Lconstant_index_right.1
.Lconstant_index_right.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	constant_index_right, .-constant_index_right
	.globl	constant_index_into
	.type	constant_index_into, @function
constant_index_into:
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
	str	x0, [x29, #24]
	ldr	x19, [x29, #24]
	add	x19, x19, #12
	add	x19, x19, #12
	mov	x0, x19
	b	.Lconstant_index_into.1
.Lconstant_index_into.1:
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
	.size	constant_index_into, .-constant_index_into
	.globl	constant_index_wide
	.type	constant_index_wide, @function
constant_index_wide:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	mov	x0, #4294967296
	ldr	x1, [x29, #16]
	add	x0, x0, x1
	b	.Lconstant_index_wide.1
.Lconstant_index_wide.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	constant_index_wide, .-constant_index_wide
	.globl	widen_char
	.type	widen_char, @function
widen_char:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	strb	w0, [x29, #16]
	ldrb	w0, [x29, #16]
	b	.Lwiden_char.1
.Lwiden_char.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	widen_char, .-widen_char
	.globl	widen_byte
	.type	widen_byte, @function
widen_byte:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	strb	w0, [x29, #16]
	ldrsb	x0, [x29, #16]
	b	.Lwiden_byte.1
.Lwiden_byte.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	widen_byte, .-widen_byte
	.bss
	.globl	defined
	.type	defined, @object
	.size	defined, 4
	.p2align 2
defined:
	.zero	4
	.section	.note.GNU-stack,"",@progbits
