	.file	"loop-terminated.c"
	.text
	.globl	while_returns
	.type	while_returns, @function
while_returns:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
.Lwhile_returns.2:
	ldr	w0, [x29, #16]
	cbz	w0, .Lwhile_returns.3
	ldr	w0, [x29, #16]
	sub	w0, w0, #1
	str	w0, [x29, #16]
	ldr	w0, [x29, #16]
	cmp	w0, #5
	b.gt	.Lwhile_returns.3
	mov	w0, #3
	b	.Lwhile_returns.1
.Lwhile_returns.3:
	ldr	w0, [x29, #16]
	b	.Lwhile_returns.1
.Lwhile_returns.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	while_returns, .-while_returns
	.globl	while_breaks
	.type	while_breaks, @function
while_breaks:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
.Lwhile_breaks.2:
	ldr	w0, [x29, #16]
	cbz	w0, .Lwhile_breaks.3
	ldr	w0, [x29, #16]
	sub	w0, w0, #1
	str	w0, [x29, #16]
	b	.Lwhile_breaks.3
.Lwhile_breaks.3:
	ldr	w0, [x29, #16]
	b	.Lwhile_breaks.1
.Lwhile_breaks.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	while_breaks, .-while_breaks
	.globl	while_continues
	.type	while_continues, @function
while_continues:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
.Lwhile_continues.2:
	ldr	w0, [x29, #16]
	cbz	w0, .Lwhile_continues.3
	ldr	w0, [x29, #16]
	sub	w0, w0, #1
	str	w0, [x29, #16]
	ldr	w0, [x29, #16]
	and	w0, w0, #0x1
	cbnz	w0, .Lwhile_continues.2
	mov	w0, #3
	b	.Lwhile_continues.1
.Lwhile_continues.3:
	ldr	w0, [x29, #16]
	b	.Lwhile_continues.1
.Lwhile_continues.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	while_continues, .-while_continues
	.globl	while_falls
	.type	while_falls, @function
while_falls:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
.Lwhile_falls.2:
	ldr	w0, [x29, #16]
	cbz	w0, .Lwhile_falls.3
	ldr	w0, [x29, #16]
	sub	w0, w0, #1
	str	w0, [x29, #16]
	ldr	w0, [x29, #16]
	cmp	w0, #5
	b.le	.Lwhile_falls.4
	mov	w0, #3
	b	.Lwhile_falls.1
.Lwhile_falls.4:
	b	.Lwhile_falls.2
.Lwhile_falls.3:
	ldr	w0, [x29, #16]
	b	.Lwhile_falls.1
.Lwhile_falls.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	while_falls, .-while_falls
	.globl	for_returns
	.type	for_returns, @function
for_returns:
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
.Lfor_returns.2:
	ldr	w0, [x29, #20]
	ldr	w1, [x29, #16]
	cmp	w0, w1
	b.ge	.Lfor_returns.4
	ldr	w0, [x29, #20]
	cmp	w0, #5
	b.gt	.Lfor_returns.4
	mov	w0, #3
	b	.Lfor_returns.1
.Lfor_returns.4:
	ldr	w0, [x29, #16]
	b	.Lfor_returns.1
.Lfor_returns.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	for_returns, .-for_returns
	.globl	for_continues
	.type	for_continues, @function
for_continues:
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
.Lfor_continues.2:
	ldr	w0, [x29, #20]
	ldr	w1, [x29, #16]
	cmp	w0, w1
	b.ge	.Lfor_continues.4
	ldr	w0, [x29, #20]
	and	w0, w0, #0x1
	cbnz	w0, .Lfor_continues.3
	mov	w0, #3
	b	.Lfor_continues.1
.Lfor_continues.3:
	ldr	w0, [x29, #20]
	add	w0, w0, #1
	str	w0, [x29, #20]
	b	.Lfor_continues.2
.Lfor_continues.4:
	ldr	w0, [x29, #16]
	b	.Lfor_continues.1
.Lfor_continues.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	for_continues, .-for_continues
	.globl	for_falls
	.type	for_falls, @function
for_falls:
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
.Lfor_falls.2:
	ldr	w0, [x29, #20]
	ldr	w1, [x29, #16]
	cmp	w0, w1
	b.ge	.Lfor_falls.4
	ldr	w0, [x29, #20]
	cmp	w0, #5
	b.le	.Lfor_falls.5
	mov	w0, #3
	b	.Lfor_falls.1
.Lfor_falls.5:
.Lfor_falls.3:
	ldr	w0, [x29, #20]
	add	w0, w0, #1
	str	w0, [x29, #20]
	b	.Lfor_falls.2
.Lfor_falls.4:
	ldr	w0, [x29, #16]
	b	.Lfor_falls.1
.Lfor_falls.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	for_falls, .-for_falls
	.globl	for_ever
	.type	for_ever, @function
for_ever:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
.Lfor_ever.2:
	ldr	w0, [x29, #16]
	sub	w0, w0, #1
	str	w0, [x29, #16]
	ldr	w0, [x29, #16]
	cmp	w0, #0
	b.ne	.Lfor_ever.5
	mov	w0, #3
	b	.Lfor_ever.1
.Lfor_ever.5:
.Lfor_ever.3:
	b	.Lfor_ever.2
.Lfor_ever.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	for_ever, .-for_ever
	.globl	do_returns
	.type	do_returns, @function
do_returns:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
.Ldo_returns.2:
	ldr	w0, [x29, #16]
	sub	w0, w0, #1
	str	w0, [x29, #16]
	ldr	w0, [x29, #16]
	cmp	w0, #5
	b.gt	.Ldo_returns.4
	mov	w0, #3
	b	.Ldo_returns.1
.Ldo_returns.4:
	ldr	w0, [x29, #16]
	b	.Ldo_returns.1
.Ldo_returns.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	do_returns, .-do_returns
	.globl	do_continues
	.type	do_continues, @function
do_continues:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
.Ldo_continues.2:
	ldr	w0, [x29, #16]
	sub	w0, w0, #1
	str	w0, [x29, #16]
	ldr	w0, [x29, #16]
	and	w0, w0, #0x1
	cbnz	w0, .Ldo_continues.3
	mov	w0, #3
	b	.Ldo_continues.1
.Ldo_continues.3:
	ldr	w0, [x29, #16]
	cbnz	w0, .Ldo_continues.2
	ldr	w0, [x29, #16]
	b	.Ldo_continues.1
.Ldo_continues.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	do_continues, .-do_continues
	.globl	do_falls
	.type	do_falls, @function
do_falls:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
.Ldo_falls.2:
	ldr	w0, [x29, #16]
	sub	w0, w0, #1
	str	w0, [x29, #16]
	ldr	w0, [x29, #16]
	cmp	w0, #5
	b.le	.Ldo_falls.5
	mov	w0, #3
	b	.Ldo_falls.1
.Ldo_falls.5:
.Ldo_falls.3:
	ldr	w0, [x29, #16]
	cbnz	w0, .Ldo_falls.2
	ldr	w0, [x29, #16]
	b	.Ldo_falls.1
.Ldo_falls.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	do_falls, .-do_falls
	.globl	switch_returns
	.type	switch_returns, @function
switch_returns:
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
	cbz	w0, .Lswitch_returns.2
	ldr	w0, [x29, #20]
	cmp	w0, #1
	b.eq	.Lswitch_returns.3
	b	.Lswitch_returns.4
.Lswitch_returns.3:
	mov	w0, #1
	b	.Lswitch_returns.1
.Lswitch_returns.4:
	mov	w0, #2
	b	.Lswitch_returns.1
.Lswitch_returns.2:
	mov	w0, #3
	str	w0, [x29, #20]
	ldr	w0, [x29, #20]
	b	.Lswitch_returns.1
.Lswitch_returns.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	switch_returns, .-switch_returns
	.globl	switch_breaks
	.type	switch_breaks, @function
switch_breaks:
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
	cbz	w0, .Lswitch_breaks.2
	ldr	w0, [x29, #20]
	cmp	w0, #1
	b.eq	.Lswitch_breaks.3
	b	.Lswitch_breaks.4
.Lswitch_breaks.3:
	b	.Lswitch_breaks.5
.Lswitch_breaks.4:
	mov	w0, #2
	b	.Lswitch_breaks.1
.Lswitch_breaks.5:
	b	.Lswitch_breaks.6
.Lswitch_breaks.2:
	mov	w0, #3
	str	w0, [x29, #20]
.Lswitch_breaks.6:
	ldr	w0, [x29, #20]
	b	.Lswitch_breaks.1
.Lswitch_breaks.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	switch_breaks, .-switch_breaks
	.section	.note.GNU-stack,"",@progbits
