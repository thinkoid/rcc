	.file	"statement-expression-order.c"
	.text
	.globl	mark
	.type	mark, @function
mark:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	adrp	x0, order
	adrp	x16, marks
	ldr	w1, [x16, :lo12:marks]
	add	w2, w1, #1
	adrp	x16, marks
	str	w2, [x16, :lo12:marks]
	add	x0, x0, w1, sxtw #2
	ldr	w1, [x29, #16]
	str	w1, [x0, :lo12:order]
	ldr	w0, [x29, #16]
	b	.Lmark.1
.Lmark.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	mark, .-mark
	.globl	three
	.type	three, @function
three:
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
	ldr	w0, [x29, #16]
	mov	w1, #100
	mul	w0, w0, w1
	ldr	w1, [x29, #20]
	mov	w2, #10
	mul	w1, w1, w2
	add	w0, w0, w1
	ldr	w1, [x29, #24]
	add	w0, w0, w1
	b	.Lthree.1
.Lthree.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	three, .-three
	.globl	in_order
	.type	in_order, @function
in_order:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	adrp	x16, marks
	ldr	w0, [x16, :lo12:marks]
	ldr	w1, [x29, #16]
	cmp	w0, w1
	b.eq	.Lin_order.2
	mov	w0, #0
	b	.Lin_order.1
.Lin_order.2:
	str	wzr, [x29, #20]
.Lin_order.3:
	ldr	w0, [x29, #20]
	ldr	w1, [x29, #16]
	cmp	w0, w1
	b.ge	.Lin_order.5
	adrp	x0, order
	ldr	w1, [x29, #20]
	add	x0, x0, w1, sxtw #2
	ldr	w0, [x0, :lo12:order]
	ldr	w1, [x29, #20]
	add	w1, w1, #1
	cmp	w0, w1
	b.eq	.Lin_order.6
	mov	w0, #0
	b	.Lin_order.1
.Lin_order.6:
	ldr	w0, [x29, #20]
	add	w0, w0, #1
	str	w0, [x29, #20]
	b	.Lin_order.3
.Lin_order.5:
	adrp	x16, marks
	str	wzr, [x16, :lo12:marks]
	mov	w0, #1
	b	.Lin_order.1
.Lin_order.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	in_order, .-in_order
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	sub	sp, sp, #240
	.cfi_def_cfa_offset 240
	stp	x29, x30, [sp]
	.cfi_offset w29, -240
	.cfi_offset w30, -232
	mov	x29, sp
	.cfi_def_cfa_register w29
	mov	w0, #1
	str	w0, [x29, #36]
	ldr	w0, [x29, #36]
	bl	mark
	str	x0, [x29, #40]
	mov	w0, #2
	str	w0, [x29, #48]
	ldr	w0, [x29, #48]
	bl	mark
	mov	w0, #3
	ldr	x1, [x29, #40]
	add	w1, w1, w0
	str	x1, [x29, #56]
	mov	w0, #3
	str	w0, [x29, #64]
	ldr	w0, [x29, #64]
	bl	mark
	ldr	x1, [x29, #56]
	add	w1, w1, w0
	str	w1, [x29, #32]
	mov	w0, #3
	str	w0, [x29, #68]
	ldr	w0, [x29, #68]
	bl	in_order
	cbz	w0, .Lmain.3
	ldr	w0, [x29, #32]
	cmp	w0, #7
	b.eq	.Lmain.2
.Lmain.3:
	mov	w0, #101
	b	.Lmain.1
.Lmain.2:
	mov	w0, #1
	str	w0, [x29, #72]
	ldr	w0, [x29, #72]
	bl	mark
	mov	w1, #10
	mul	w0, w0, w1
	str	x0, [x29, #80]
	mov	w0, #2
	str	w0, [x29, #92]
	ldr	w0, [x29, #92]
	bl	mark
	str	w0, [x29, #88]
	mov	w0, #3
	str	w0, [x29, #96]
	ldr	w0, [x29, #96]
	bl	mark
	ldr	w0, [x29, #88]
	ldr	x1, [x29, #80]
	add	w1, w1, w0
	str	w1, [x29, #32]
	mov	w0, #3
	str	w0, [x29, #100]
	ldr	w0, [x29, #100]
	bl	in_order
	cbz	w0, .Lmain.5
	ldr	w0, [x29, #32]
	cmp	w0, #12
	b.eq	.Lmain.4
.Lmain.5:
	mov	w0, #102
	b	.Lmain.1
.Lmain.4:
	mov	w0, #1
	str	w0, [x29, #108]
	ldr	w0, [x29, #108]
	bl	mark
	str	w0, [x29, #104]
	mov	w0, #2
	str	w0, [x29, #116]
	ldr	w0, [x29, #116]
	bl	mark
	mov	w0, #5
	str	w0, [x29, #112]
	mov	w0, #3
	str	w0, [x29, #124]
	ldr	w0, [x29, #124]
	bl	mark
	str	w0, [x29, #120]
	ldr	w0, [x29, #104]
	ldr	w1, [x29, #112]
	ldr	w2, [x29, #120]
	bl	three
	str	w0, [x29, #32]
	mov	w0, #3
	str	w0, [x29, #128]
	ldr	w0, [x29, #128]
	bl	in_order
	cbz	w0, .Lmain.7
	ldr	w0, [x29, #32]
	cmp	w0, #153
	b.eq	.Lmain.6
.Lmain.7:
	mov	w0, #103
	b	.Lmain.1
.Lmain.6:
	mov	w0, #1
	str	w0, [x29, #132]
	ldr	w0, [x29, #132]
	bl	mark
	str	x0, [x29, #136]
	mov	w0, #2
	str	w0, [x29, #148]
	ldr	w0, [x29, #148]
	bl	mark
	str	x0, [x29, #152]
	mov	w0, #3
	str	w0, [x29, #160]
	ldr	w0, [x29, #160]
	bl	mark
	mov	w0, #4
	ldr	x1, [x29, #152]
	add	w1, w1, w0
	str	w1, [x29, #144]
	mov	w0, #4
	str	w0, [x29, #164]
	ldr	w0, [x29, #164]
	bl	mark
	ldr	w0, [x29, #144]
	ldr	x1, [x29, #136]
	add	w1, w1, w0
	str	w1, [x29, #32]
	mov	w0, #4
	str	w0, [x29, #168]
	ldr	w0, [x29, #168]
	bl	in_order
	cbz	w0, .Lmain.9
	ldr	w0, [x29, #32]
	cmp	w0, #7
	b.eq	.Lmain.8
.Lmain.9:
	mov	w0, #104
	b	.Lmain.1
.Lmain.8:
	mov	w0, #1
	str	w0, [x29, #172]
	ldr	w0, [x29, #172]
	bl	mark
	add	x0, x29, w0, sxtw #2
	str	x0, [x29, #176]
	mov	w0, #2
	str	w0, [x29, #184]
	ldr	w0, [x29, #184]
	bl	mark
	mov	w0, #9
	ldr	x1, [x29, #176]
	str	w0, [x1, #16]
	mov	w0, #2
	str	w0, [x29, #188]
	ldr	w0, [x29, #188]
	bl	in_order
	cbz	w0, .Lmain.11
	ldr	w0, [x29, #20]
	cmp	w0, #9
	b.eq	.Lmain.10
.Lmain.11:
	mov	w0, #105
	b	.Lmain.1
.Lmain.10:
	mov	w0, #1
	str	w0, [x29, #192]
	ldr	w0, [x29, #192]
	bl	mark
	cbz	w0, .Lmain.12
	mov	w0, #2
	str	w0, [x29, #196]
	ldr	w0, [x29, #196]
	bl	mark
	mov	w0, #0
	cbz	w0, .Lmain.12
	mov	w0, #3
	str	w0, [x29, #200]
	ldr	w0, [x29, #200]
	bl	mark
	cbz	w0, .Lmain.12
	mov	w0, #1
	b	.Lmain.13
.Lmain.12:
	mov	w0, #0
.Lmain.13:
	str	w0, [x29, #32]
	mov	w0, #2
	str	w0, [x29, #204]
	ldr	w0, [x29, #204]
	bl	in_order
	cbz	w0, .Lmain.15
	ldr	w0, [x29, #32]
	cmp	w0, #0
	b.eq	.Lmain.14
.Lmain.15:
	mov	w0, #106
	b	.Lmain.1
.Lmain.14:
	mov	w0, #1
	str	w0, [x29, #208]
	ldr	w0, [x29, #208]
	bl	mark
	cbz	w0, .Lmain.16
	mov	w0, #2
	str	w0, [x29, #212]
	ldr	w0, [x29, #212]
	bl	mark
	mov	w0, #6
	b	.Lmain.17
.Lmain.16:
	mov	w0, #9
	str	w0, [x29, #216]
	ldr	w0, [x29, #216]
	bl	mark
.Lmain.17:
	str	w0, [x29, #32]
	mov	w0, #2
	str	w0, [x29, #220]
	ldr	w0, [x29, #220]
	bl	in_order
	cbz	w0, .Lmain.19
	ldr	w0, [x29, #32]
	cmp	w0, #6
	b.eq	.Lmain.18
.Lmain.19:
	mov	w0, #107
	b	.Lmain.1
.Lmain.18:
	mov	w0, #1
	str	w0, [x29, #224]
	ldr	w0, [x29, #224]
	bl	mark
	mov	w0, #2
	str	w0, [x29, #228]
	ldr	w0, [x29, #228]
	bl	mark
	cmp	w0, #2
	b.ne	.Lmain.20
	mov	w0, #3
	str	w0, [x29, #232]
	ldr	w0, [x29, #232]
	bl	mark
	str	w0, [x29, #32]
.Lmain.20:
	mov	w0, #3
	str	w0, [x29, #236]
	ldr	w0, [x29, #236]
	bl	in_order
	cbz	w0, .Lmain.22
	ldr	w0, [x29, #32]
	cmp	w0, #3
	b.eq	.Lmain.21
.Lmain.22:
	mov	w0, #108
	b	.Lmain.1
.Lmain.21:
	mov	w0, #42
	b	.Lmain.1
.Lmain.1:
	.cfi_def_cfa wsp, 240
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #240
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	main, .-main
	.bss
	.globl	order
	.type	order, @object
	.size	order, 64
	.p2align 2
order:
	.zero	64
	.globl	marks
	.type	marks, @object
	.size	marks, 4
	.p2align 2
marks:
	.zero	4
	.section	.note.GNU-stack,"",@progbits
