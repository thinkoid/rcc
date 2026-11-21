	.file	"bool.c"
	.text
	.globl	from_int
	.type	from_int, @function
from_int:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, i
	ldr	w0, [x16, :lo12:i]
	cmp	w0, #0
	cset	w0, ne
	adrp	x16, b
	strb	w0, [x16, :lo12:b]
.Lfrom_int.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	from_int, .-from_int
	.globl	from_long
	.type	from_long, @function
from_long:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, l
	ldr	x0, [x16, :lo12:l]
	cmp	x0, #0
	cset	w0, ne
	adrp	x16, b
	strb	w0, [x16, :lo12:b]
.Lfrom_long.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	from_long, .-from_long
	.globl	from_pointer
	.type	from_pointer, @function
from_pointer:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, p
	ldr	x0, [x16, :lo12:p]
	cmp	x0, #0
	cset	w0, ne
	adrp	x16, b
	strb	w0, [x16, :lo12:b]
.Lfrom_pointer.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	from_pointer, .-from_pointer
	.globl	from_constant
	.type	from_constant, @function
from_constant:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	mov	w0, #1
	adrp	x16, b
	strb	w0, [x16, :lo12:b]
.Lfrom_constant.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	from_constant, .-from_constant
	.globl	from_comparison
	.type	from_comparison, @function
from_comparison:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, i
	ldrsw	x0, [x16, :lo12:i]
	adrp	x16, l
	ldr	x1, [x16, :lo12:l]
	cmp	x0, x1
	cset	w0, lt
	adrp	x16, b
	strb	w0, [x16, :lo12:b]
.Lfrom_comparison.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	from_comparison, .-from_comparison
	.globl	copied
	.type	copied, @function
copied:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, c
	ldrb	w0, [x16, :lo12:c]
	adrp	x16, b
	strb	w0, [x16, :lo12:b]
.Lcopied.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	copied, .-copied
	.globl	passed
	.type	passed, @function
passed:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, c
	ldrb	w0, [x16, :lo12:c]
	strb	w0, [x29, #16]
	ldrb	w0, [x29, #16]
	bl	take
.Lpassed.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	passed, .-passed
	.globl	returned
	.type	returned, @function
returned:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, c
	ldrb	w0, [x16, :lo12:c]
	b	.Lreturned.1
.Lreturned.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	returned, .-returned
	.globl	read
	.type	read, @function
read:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, b
	ldrb	w0, [x16, :lo12:b]
	add	w0, w0, #1
	b	.Lread.1
.Lread.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	read, .-read
	.globl	condition
	.type	condition, @function
condition:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, b
	ldrb	w0, [x16, :lo12:b]
	cbz	w0, .Lcondition.2
	mov	w0, #3
	b	.Lcondition.1
.Lcondition.2:
	mov	w0, #4
	b	.Lcondition.1
.Lcondition.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	condition, .-condition
	.globl	add
	.type	add, @function
add:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, b
	ldrb	w0, [x16, :lo12:b]
	adrp	x16, i
	ldr	w1, [x16, :lo12:i]
	add	w0, w0, w1
	cmp	w0, #0
	cset	w0, ne
	adrp	x16, b
	strb	w0, [x16, :lo12:b]
.Ladd.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	add, .-add
	.globl	sub
	.type	sub, @function
sub:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, b
	ldrb	w0, [x16, :lo12:b]
	adrp	x16, i
	ldr	w1, [x16, :lo12:i]
	sub	w0, w0, w1
	cmp	w0, #0
	cset	w0, ne
	adrp	x16, b
	strb	w0, [x16, :lo12:b]
.Lsub.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	sub, .-sub
	.globl	and
	.type	and, @function
and:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, b
	ldrb	w0, [x16, :lo12:b]
	adrp	x16, i
	ldr	w1, [x16, :lo12:i]
	and	w0, w0, w1
	cmp	w0, #0
	cset	w0, ne
	adrp	x16, b
	strb	w0, [x16, :lo12:b]
.Land.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	and, .-and
	.globl	or
	.type	or, @function
or:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, b
	ldrb	w0, [x16, :lo12:b]
	adrp	x16, i
	ldr	w1, [x16, :lo12:i]
	orr	w0, w0, w1
	cmp	w0, #0
	cset	w0, ne
	adrp	x16, b
	strb	w0, [x16, :lo12:b]
.Lor.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	or, .-or
	.globl	xor
	.type	xor, @function
xor:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, b
	ldrb	w0, [x16, :lo12:b]
	adrp	x16, i
	ldr	w1, [x16, :lo12:i]
	eor	w0, w0, w1
	cmp	w0, #0
	cset	w0, ne
	adrp	x16, b
	strb	w0, [x16, :lo12:b]
.Lxor.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	xor, .-xor
	.globl	shl
	.type	shl, @function
shl:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, b
	ldrb	w0, [x16, :lo12:b]
	adrp	x16, i
	ldr	w1, [x16, :lo12:i]
	lsl	w0, w0, w1
	cmp	w0, #0
	cset	w0, ne
	adrp	x16, b
	strb	w0, [x16, :lo12:b]
.Lshl.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	shl, .-shl
	.globl	shr
	.type	shr, @function
shr:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, b
	ldrb	w0, [x16, :lo12:b]
	adrp	x16, i
	ldr	w1, [x16, :lo12:i]
	asr	w0, w0, w1
	cmp	w0, #0
	cset	w0, ne
	adrp	x16, b
	strb	w0, [x16, :lo12:b]
.Lshr.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	shr, .-shr
	.globl	mul
	.type	mul, @function
mul:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, b
	ldrb	w0, [x16, :lo12:b]
	adrp	x16, i
	ldr	w1, [x16, :lo12:i]
	mul	w0, w0, w1
	cmp	w0, #0
	cset	w0, ne
	adrp	x16, b
	strb	w0, [x16, :lo12:b]
.Lmul.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	mul, .-mul
	.globl	div
	.type	div, @function
div:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, b
	ldrb	w0, [x16, :lo12:b]
	adrp	x16, i
	ldr	w1, [x16, :lo12:i]
	sdiv	w0, w0, w1
	cmp	w0, #0
	cset	w0, ne
	adrp	x16, b
	strb	w0, [x16, :lo12:b]
.Ldiv.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	div, .-div
	.globl	rem
	.type	rem, @function
rem:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, b
	ldrb	w0, [x16, :lo12:b]
	adrp	x16, i
	ldr	w1, [x16, :lo12:i]
	sdiv	w2, w0, w1
	msub	w0, w2, w1, w0
	cmp	w0, #0
	cset	w0, ne
	adrp	x16, b
	strb	w0, [x16, :lo12:b]
.Lrem.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	rem, .-rem
	.globl	pre_inc
	.type	pre_inc, @function
pre_inc:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, b
	ldrb	w0, [x16, :lo12:b]
	add	w0, w0, #1
	cmp	w0, #0
	cset	w0, ne
	adrp	x16, b
	strb	w0, [x16, :lo12:b]
	uxtb	w0, w0
	b	.Lpre_inc.1
.Lpre_inc.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	pre_inc, .-pre_inc
	.globl	pre_dec
	.type	pre_dec, @function
pre_dec:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, b
	ldrb	w0, [x16, :lo12:b]
	sub	w0, w0, #1
	cmp	w0, #0
	cset	w0, ne
	adrp	x16, b
	strb	w0, [x16, :lo12:b]
	uxtb	w0, w0
	b	.Lpre_dec.1
.Lpre_dec.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	pre_dec, .-pre_dec
	.globl	post_inc
	.type	post_inc, @function
post_inc:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, b
	ldrb	w0, [x16, :lo12:b]
	strb	w0, [x29, #16]
	ldrb	w0, [x29, #16]
	add	w0, w0, #1
	cmp	w0, #0
	cset	w0, ne
	adrp	x16, b
	strb	w0, [x16, :lo12:b]
	ldrb	w0, [x29, #16]
	b	.Lpost_inc.1
.Lpost_inc.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	post_inc, .-post_inc
	.globl	post_dec
	.type	post_dec, @function
post_dec:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, b
	ldrb	w0, [x16, :lo12:b]
	strb	w0, [x29, #16]
	ldrb	w0, [x29, #16]
	sub	w0, w0, #1
	cmp	w0, #0
	cset	w0, ne
	adrp	x16, b
	strb	w0, [x16, :lo12:b]
	ldrb	w0, [x29, #16]
	b	.Lpost_dec.1
.Lpost_dec.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
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
