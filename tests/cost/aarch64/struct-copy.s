	.file	"struct-copy.c"
	.text
	.globl	through
	.type	through, @function
through:
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
	ldr	w17, [x1]
	str	w17, [x0]
	ldr	w17, [x1, #4]
	str	w17, [x0, #4]
.Lthrough.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	through, .-through
	.globl	globals
	.type	globals, @function
globals:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, corner
	ldr	w17, [x16, :lo12:corner]
	adrp	x16, origin
	str	w17, [x16, :lo12:origin]
	adrp	x16, corner+4
	ldr	w17, [x16, :lo12:corner+4]
	adrp	x16, origin+4
	str	w17, [x16, :lo12:origin+4]
	adrp	x0, first
	add	x0, x0, :lo12:first
	adrp	x1, second
	add	x1, x1, :lo12:second
	ldr	x17, [x1]
	str	x17, [x0]
	ldr	x17, [x1, #8]
	str	x17, [x0, #8]
	ldr	x17, [x1, #16]
	str	x17, [x0, #16]
.Lglobals.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	globals, .-globals
	.globl	local
	.type	local, @function
local:
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
	ldrb	w17, [x0]
	strb	w17, [x29, #24]
	ldrb	w17, [x0, #1]
	strb	w17, [x29, #25]
	ldrb	w17, [x0, #2]
	strb	w17, [x29, #26]
	adrp	x0, three
	add	x0, x0, :lo12:three
	ldrb	w17, [x29, #24]
	strb	w17, [x0]
	ldrb	w17, [x29, #25]
	strb	w17, [x0, #1]
	ldrb	w17, [x29, #26]
	strb	w17, [x0, #2]
	ldrb	w0, [x29, #26]
	b	.Llocal.1
.Llocal.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	local, .-local
	.globl	member
	.type	member, @function
member:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	x1, [x29, #24]
	str	w2, [x29, #32]
	ldr	x0, [x29, #16]
	ldr	x1, [x29, #24]
	ldr	w2, [x29, #32]
	add	x1, x1, w2, sxtw #3
	ldr	w17, [x1]
	str	w17, [x0, #8]
	ldr	w17, [x1, #4]
	str	w17, [x0, #12]
	ldr	x0, [x29, #24]
	ldr	w1, [x29, #32]
	add	w1, w1, #1
	ldr	x2, [x29, #16]
	add	x0, x0, w1, sxtw #3
	ldr	w17, [x2]
	str	w17, [x0]
	ldr	w17, [x2, #4]
	str	w17, [x0, #4]
.Lmember.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	member, .-member
	.globl	unions
	.type	unions, @function
unions:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, other
	ldr	x17, [x16, :lo12:other]
	adrp	x16, one
	str	x17, [x16, :lo12:one]
.Lunions.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	unions, .-unions
	.globl	effects
	.type	effects, @function
effects:
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
	ldr	w17, [x0]
	adrp	x16, origin
	str	w17, [x16, :lo12:origin]
	ldr	w17, [x0, #4]
	adrp	x16, origin+4
	str	w17, [x16, :lo12:origin+4]
	str	wzr, [x29, #28]
.Leffects.2:
	ldr	w0, [x29, #28]
	ldr	w1, [x29, #24]
	cmp	w0, w1
	b.ge	.Leffects.4
	ldr	x0, [x29, #16]
	ldr	w1, [x29, #28]
	add	x0, x0, w1, sxtw #3
	ldr	w17, [x0]
	adrp	x16, corner
	str	w17, [x16, :lo12:corner]
	ldr	w17, [x0, #4]
	adrp	x16, corner+4
	str	w17, [x16, :lo12:corner+4]
	ldr	w0, [x29, #28]
	add	w0, w0, #1
	str	w0, [x29, #28]
	b	.Leffects.2
.Leffects.4:
	adrp	x16, corner
	ldr	w17, [x16, :lo12:corner]
	adrp	x16, origin
	str	w17, [x16, :lo12:origin]
	adrp	x16, corner+4
	ldr	w17, [x16, :lo12:corner+4]
	adrp	x16, origin+4
	str	w17, [x16, :lo12:origin+4]
	adrp	x16, origin
	ldr	w17, [x16, :lo12:origin]
	adrp	x16, corner
	str	w17, [x16, :lo12:corner]
	adrp	x16, origin+4
	ldr	w17, [x16, :lo12:origin+4]
	adrp	x16, corner+4
	str	w17, [x16, :lo12:corner+4]
	adrp	x16, corner
	ldr	w0, [x16, :lo12:corner]
	b	.Leffects.1
.Leffects.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	effects, .-effects
	.globl	codes
	.type	codes, @function
codes:
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
	add	x0, x0, x1, lsl #2
	ldrh	w17, [x0]
	strh	w17, [x29, #28]
	ldrh	w17, [x0, #2]
	strh	w17, [x29, #30]
	ldr	x0, [x29, #16]
	ldr	w1, [x29, #24]
	add	w1, w1, #1
	add	x0, x0, x1, lsl #2
	ldrh	w17, [x29, #28]
	strh	w17, [x0]
	ldrh	w17, [x29, #30]
	strh	w17, [x0, #2]
.Lcodes.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	codes, .-codes
	.globl	eight_units
	.type	eight_units, @function
eight_units:
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
	ldr	w17, [x1]
	str	w17, [x0]
	ldr	w17, [x1, #4]
	str	w17, [x0, #4]
	ldr	w17, [x1, #8]
	str	w17, [x0, #8]
	ldr	w17, [x1, #12]
	str	w17, [x0, #12]
	ldr	w17, [x1, #16]
	str	w17, [x0, #16]
	ldr	w17, [x1, #20]
	str	w17, [x0, #20]
	ldr	w17, [x1, #24]
	str	w17, [x0, #24]
	ldr	w17, [x1, #28]
	str	w17, [x0, #28]
.Leight_units.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	eight_units, .-eight_units
	.globl	nine_units
	.type	nine_units, @function
nine_units:
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
	mov	x16, #9
.Lnine_units.2:
	ldr	x17, [x1], #8
	str	x17, [x0], #8
	subs	x16, x16, #1
	b.ne	.Lnine_units.2
.Lnine_units.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	nine_units, .-nine_units
	.globl	nine_named
	.type	nine_named, @function
nine_named:
	.cfi_startproc
	sub	sp, sp, #96
	.cfi_def_cfa_offset 96
	stp	x29, x30, [sp]
	.cfi_offset w29, -96
	.cfi_offset w30, -88
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	ldr	x0, [x29, #16]
	add	x1, x29, #24
	mov	x16, #9
.Lnine_named.2:
	ldr	x17, [x0], #8
	str	x17, [x1], #8
	subs	x16, x16, #1
	b.ne	.Lnine_named.2
	adrp	x0, spare
	add	x0, x0, :lo12:spare
	add	x1, x29, #24
	mov	x16, #9
.Lnine_named.3:
	ldr	x17, [x1], #8
	str	x17, [x0], #8
	subs	x16, x16, #1
	b.ne	.Lnine_named.3
.Lnine_named.1:
	.cfi_def_cfa wsp, 96
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #96
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	nine_named, .-nine_named
	.globl	nine_indexed
	.type	nine_indexed, @function
nine_indexed:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	x1, [x29, #24]
	str	w2, [x29, #32]
	ldr	x0, [x29, #16]
	ldrsw	x1, [x29, #32]
	mov	x2, #72
	mul	x1, x1, x2
	add	x0, x0, x1
	ldr	x1, [x29, #24]
	ldrsw	x2, [x29, #32]
	mov	x3, #72
	mul	x2, x2, x3
	add	x1, x1, x2
	mov	x16, #9
.Lnine_indexed.2:
	ldr	x17, [x1], #8
	str	x17, [x0], #8
	subs	x16, x16, #1
	b.ne	.Lnine_indexed.2
.Lnine_indexed.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	nine_indexed, .-nine_indexed
	.globl	text_units
	.type	text_units, @function
text_units:
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
	mov	x16, #32
.Ltext_units.2:
	ldrb	w17, [x1], #1
	strb	w17, [x0], #1
	subs	x16, x16, #1
	b.ne	.Ltext_units.2
.Ltext_units.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	text_units, .-text_units
	.globl	held
	.type	held, @function
held:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	bl	next
	str	x0, [x29, #16]
	bl	last
	ldrh	w17, [x0]
	ldr	x1, [x29, #16]
	strh	w17, [x1]
	ldrh	w17, [x0, #2]
	strh	w17, [x1, #2]
.Lheld.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	held, .-held
	.globl	registers
	.type	registers, @function
registers:
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
	str	x0, [x29, #32]
	str	x1, [x29, #40]
	ldr	x19, [x29, #32]
	ldr	x20, [x29, #40]
	ldrh	w17, [x20]
	strh	w17, [x19]
	ldrh	w17, [x20, #2]
	strh	w17, [x19, #2]
.Lregisters.1:
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
	.size	registers, .-registers
	.globl	stepped
	.type	stepped, @function
stepped:
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
	str	x0, [x29, #32]
	str	x1, [x29, #40]
	ldr	x19, [x29, #32]
	ldr	x20, [x29, #40]
	ldrh	w17, [x20]
	strh	w17, [x19]
	ldrh	w17, [x20, #2]
	strh	w17, [x19, #2]
	add	x19, x19, #4
	add	x19, x19, #4
	ldrh	w17, [x20]
	strh	w17, [x19]
	ldrh	w17, [x20, #2]
	strh	w17, [x19, #2]
	add	x20, x20, #-4
.Lstepped.1:
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
	.size	stepped, .-stepped
	.globl	comma_effects
	.type	comma_effects, @function
comma_effects:
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
	str	wzr, [x29, #28]
.Lcomma_effects.2:
	ldr	w0, [x29, #28]
	ldr	w1, [x29, #24]
	cmp	w0, w1
	b.ge	.Lcomma_effects.4
	ldr	w0, [x29, #28]
	add	w0, w0, #1
	str	w0, [x29, #28]
	ldr	x0, [x29, #16]
	ldr	w1, [x29, #28]
	add	x0, x0, w1, sxtw #3
	ldr	w17, [x0]
	adrp	x16, corner
	str	w17, [x16, :lo12:corner]
	ldr	w17, [x0, #4]
	adrp	x16, corner+4
	str	w17, [x16, :lo12:corner+4]
	b	.Lcomma_effects.2
.Lcomma_effects.4:
	ldr	w0, [x29, #24]
	add	w0, w0, #1
	str	w0, [x29, #24]
	adrp	x16, corner
	ldr	w17, [x16, :lo12:corner]
	adrp	x16, origin
	str	w17, [x16, :lo12:origin]
	adrp	x16, corner+4
	ldr	w17, [x16, :lo12:corner+4]
	adrp	x16, origin+4
	str	w17, [x16, :lo12:origin+4]
	ldr	w0, [x29, #24]
	add	w0, w0, #1
	str	w0, [x29, #24]
	adrp	x16, origin
	ldr	w17, [x16, :lo12:origin]
	adrp	x16, corner
	str	w17, [x16, :lo12:corner]
	adrp	x16, origin+4
	ldr	w17, [x16, :lo12:origin+4]
	adrp	x16, corner+4
	str	w17, [x16, :lo12:corner+4]
	ldr	w0, [x29, #24]
	b	.Lcomma_effects.1
.Lcomma_effects.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	comma_effects, .-comma_effects
	.bss
	.globl	origin
	.type	origin, @object
	.size	origin, 8
	.p2align 2
origin:
	.zero	8
	.globl	corner
	.type	corner, @object
	.size	corner, 8
	.p2align 2
corner:
	.zero	8
	.globl	first
	.type	first, @object
	.size	first, 24
	.p2align 3
first:
	.zero	24
	.globl	second
	.type	second, @object
	.size	second, 24
	.p2align 3
second:
	.zero	24
	.globl	three
	.type	three, @object
	.size	three, 3
three:
	.zero	3
	.globl	one
	.type	one, @object
	.size	one, 8
	.p2align 3
one:
	.zero	8
	.globl	other
	.type	other, @object
	.size	other, 8
	.p2align 3
other:
	.zero	8
	.globl	spare
	.type	spare, @object
	.size	spare, 72
	.p2align 3
spare:
	.zero	72
	.section	.note.GNU-stack,"",@progbits
