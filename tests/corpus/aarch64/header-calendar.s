	.file	"header-calendar.c"
	.text
	.globl	leap_year
	.type	leap_year, @function
leap_year:
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
	sdiv	w2, w0, w1
	msub	w0, w2, w1, w0
	cmp	w0, #0
	b.ne	.Lleap_year.5
	ldr	w0, [x29, #16]
	mov	w1, #100
	sdiv	w2, w0, w1
	msub	w0, w2, w1, w0
	cmp	w0, #0
	b.ne	.Lleap_year.4
.Lleap_year.5:
	ldr	w0, [x29, #16]
	mov	w1, #400
	sdiv	w2, w0, w1
	msub	w0, w2, w1, w0
	cmp	w0, #0
	b.ne	.Lleap_year.2
.Lleap_year.4:
	mov	w0, #1
	b	.Lleap_year.3
.Lleap_year.2:
	mov	w0, #0
.Lleap_year.3:
	b	.Lleap_year.1
.Lleap_year.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	leap_year, .-leap_year
	.globl	day_of_year
	.type	day_of_year, @function
day_of_year:
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
	str	w2, [x29, #24]
	ldr	w0, [x29, #16]
	str	w0, [x29, #36]
	ldr	w0, [x29, #36]
	bl	leap_year
	str	w0, [x29, #32]
	mov	w0, #1
	str	w0, [x29, #28]
.Lday_of_year.2:
	ldr	w0, [x29, #28]
	ldr	w1, [x29, #20]
	cmp	w0, w1
	b.ge	.Lday_of_year.4
	adrp	x0, daytab
	add	x0, x0, :lo12:daytab
	ldrsw	x1, [x29, #32]
	mov	x2, #13
	mul	x1, x1, x2
	add	x0, x0, x1
	ldr	w1, [x29, #28]
	ldrb	w0, [x0, w1, sxtw]
	ldr	w1, [x29, #24]
	add	w1, w1, w0
	str	w1, [x29, #24]
	ldr	w0, [x29, #28]
	add	w0, w0, #1
	str	w0, [x29, #28]
	b	.Lday_of_year.2
.Lday_of_year.4:
	ldr	w0, [x29, #24]
	b	.Lday_of_year.1
.Lday_of_year.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	day_of_year, .-day_of_year
	.globl	month_day
	.type	month_day, @function
month_day:
	.cfi_startproc
	sub	sp, sp, #64
	.cfi_def_cfa_offset 64
	stp	x29, x30, [sp]
	.cfi_offset w29, -64
	.cfi_offset w30, -56
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	w1, [x29, #20]
	str	x2, [x29, #24]
	str	x3, [x29, #32]
	ldr	w0, [x29, #16]
	str	w0, [x29, #48]
	ldr	w0, [x29, #48]
	bl	leap_year
	str	w0, [x29, #44]
	mov	w0, #1
	str	w0, [x29, #40]
.Lmonth_day.2:
	ldr	w0, [x29, #20]
	adrp	x1, daytab
	add	x1, x1, :lo12:daytab
	ldrsw	x2, [x29, #44]
	mov	x3, #13
	mul	x2, x2, x3
	add	x1, x1, x2
	ldr	w2, [x29, #40]
	ldrb	w1, [x1, w2, sxtw]
	cmp	w0, w1
	b.le	.Lmonth_day.4
	adrp	x0, daytab
	add	x0, x0, :lo12:daytab
	ldrsw	x1, [x29, #44]
	mov	x2, #13
	mul	x1, x1, x2
	add	x0, x0, x1
	ldr	w1, [x29, #40]
	ldrb	w0, [x0, w1, sxtw]
	ldr	w1, [x29, #20]
	sub	w1, w1, w0
	str	w1, [x29, #20]
	ldr	w0, [x29, #40]
	add	w0, w0, #1
	str	w0, [x29, #40]
	b	.Lmonth_day.2
.Lmonth_day.4:
	ldr	x0, [x29, #24]
	ldr	w1, [x29, #40]
	str	w1, [x0]
	ldr	x0, [x29, #32]
	ldr	w1, [x29, #20]
	str	w1, [x0]
.Lmonth_day.1:
	.cfi_def_cfa wsp, 64
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #64
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	month_day, .-month_day
	.globl	month_name
	.type	month_name, @function
month_name:
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
	cmp	w0, #1
	b.lt	.Lmonth_name.4
	ldr	w0, [x29, #16]
	cmp	w0, #12
	b.le	.Lmonth_name.2
.Lmonth_name.4:
	adrp	x16, name
	ldr	x0, [x16, :lo12:name]
	b	.Lmonth_name.3
.Lmonth_name.2:
	adrp	x0, name
	add	x0, x0, :lo12:name
	ldr	w1, [x29, #16]
	ldr	x0, [x0, w1, sxtw #3]
.Lmonth_name.3:
	b	.Lmonth_name.1
.Lmonth_name.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	month_name, .-month_name
	.globl	round_trip
	.type	round_trip, @function
round_trip:
	.cfi_startproc
	sub	sp, sp, #96
	.cfi_def_cfa_offset 96
	stp	x29, x30, [sp]
	.cfi_offset w29, -96
	.cfi_offset w30, -88
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	mov	w0, #1
	str	w0, [x29, #20]
.Lround_trip.2:
	ldr	w0, [x29, #20]
	str	x0, [x29, #32]
	ldr	w0, [x29, #16]
	str	w0, [x29, #40]
	ldr	w0, [x29, #40]
	bl	leap_year
	add	w0, w0, #365
	ldr	x1, [x29, #32]
	cmp	w1, w0
	b.gt	.Lround_trip.4
	ldr	w0, [x29, #16]
	str	w0, [x29, #44]
	ldr	w0, [x29, #20]
	str	w0, [x29, #48]
	add	x0, x29, #24
	str	x0, [x29, #56]
	add	x0, x29, #28
	str	x0, [x29, #64]
	ldr	w0, [x29, #44]
	ldr	w1, [x29, #48]
	ldr	x2, [x29, #56]
	ldr	x3, [x29, #64]
	bl	month_day
	ldr	w0, [x29, #16]
	str	w0, [x29, #72]
	ldr	w0, [x29, #24]
	str	w0, [x29, #76]
	ldr	w0, [x29, #28]
	str	w0, [x29, #80]
	ldr	w0, [x29, #72]
	ldr	w1, [x29, #76]
	ldr	w2, [x29, #80]
	bl	day_of_year
	ldr	w1, [x29, #20]
	cmp	w0, w1
	b.eq	.Lround_trip.5
	ldr	w0, [x29, #20]
	b	.Lround_trip.1
.Lround_trip.5:
	ldr	w0, [x29, #20]
	add	w0, w0, #1
	str	w0, [x29, #20]
	b	.Lround_trip.2
.Lround_trip.4:
	mov	w0, #0
	b	.Lround_trip.1
.Lround_trip.1:
	.cfi_def_cfa wsp, 96
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #96
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	round_trip, .-round_trip
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	sub	sp, sp, #160
	.cfi_def_cfa_offset 160
	stp	x29, x30, [sp]
	.cfi_offset w29, -160
	.cfi_offset w30, -152
	mov	x29, sp
	.cfi_def_cfa_register w29
	mov	w0, #1988
	str	w0, [x29, #24]
	ldr	w0, [x29, #24]
	bl	round_trip
	cmp	w0, #0
	b.ne	.Lmain.3
	mov	w0, #2026
	str	w0, [x29, #28]
	ldr	w0, [x29, #28]
	bl	round_trip
	cmp	w0, #0
	b.eq	.Lmain.2
.Lmain.3:
	mov	w0, #1
	b	.Lmain.1
.Lmain.2:
	str	wzr, [x29, #32]
	ldr	w0, [x29, #32]
	bl	month_name
	adrp	x16, name
	ldr	x1, [x16, :lo12:name]
	cmp	x0, x1
	b.ne	.Lmain.5
	mov	w0, #13
	str	w0, [x29, #36]
	ldr	w0, [x29, #36]
	bl	month_name
	adrp	x16, name
	ldr	x1, [x16, :lo12:name]
	cmp	x0, x1
	b.eq	.Lmain.4
.Lmain.5:
	mov	w0, #2
	b	.Lmain.1
.Lmain.4:
	mov	w0, #1988
	str	w0, [x29, #40]
	mov	w0, #60
	str	w0, [x29, #44]
	add	x0, x29, #16
	str	x0, [x29, #48]
	add	x0, x29, #20
	str	x0, [x29, #56]
	ldr	w0, [x29, #40]
	ldr	w1, [x29, #44]
	ldr	x2, [x29, #48]
	ldr	x3, [x29, #56]
	bl	month_day
	adrp	x0, .Ls0
	add	x0, x0, :lo12:.Ls0
	str	x0, [x29, #64]
	ldr	w0, [x29, #16]
	str	w0, [x29, #80]
	ldr	w0, [x29, #80]
	bl	month_name
	str	x0, [x29, #72]
	ldr	w0, [x29, #20]
	str	w0, [x29, #84]
	ldr	x0, [x29, #64]
	ldr	x1, [x29, #72]
	ldr	w2, [x29, #84]
	bl	printf
	mov	w0, #2026
	str	w0, [x29, #88]
	mov	w0, #365
	str	w0, [x29, #92]
	add	x0, x29, #16
	str	x0, [x29, #96]
	add	x0, x29, #20
	str	x0, [x29, #104]
	ldr	w0, [x29, #88]
	ldr	w1, [x29, #92]
	ldr	x2, [x29, #96]
	ldr	x3, [x29, #104]
	bl	month_day
	adrp	x0, .Ls1
	add	x0, x0, :lo12:.Ls1
	str	x0, [x29, #112]
	mov	w0, #365
	str	w0, [x29, #120]
	mov	w0, #2026
	str	w0, [x29, #124]
	ldr	w0, [x29, #16]
	str	w0, [x29, #136]
	ldr	w0, [x29, #136]
	bl	month_name
	str	x0, [x29, #128]
	ldr	w0, [x29, #20]
	str	w0, [x29, #140]
	ldr	x0, [x29, #112]
	ldr	w1, [x29, #120]
	ldr	w2, [x29, #124]
	ldr	x3, [x29, #128]
	ldr	w4, [x29, #140]
	bl	printf
	mov	w0, #1988
	str	w0, [x29, #144]
	mov	w0, #3
	str	w0, [x29, #148]
	mov	w0, #1
	str	w0, [x29, #152]
	ldr	w0, [x29, #144]
	ldr	w1, [x29, #148]
	ldr	w2, [x29, #152]
	bl	day_of_year
	b	.Lmain.1
.Lmain.1:
	.cfi_def_cfa wsp, 160
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #160
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	main, .-main
	.data
.Ls0:
	.string	"day 60 of 1988 is %s %d\n"
.Ls1:
	.string	"day %d of %d is %s %d\n"
.Ls2:
	.string	"Illegal month"
.Ls3:
	.string	"January"
.Ls4:
	.string	"February"
.Ls5:
	.string	"March"
.Ls6:
	.string	"April"
.Ls7:
	.string	"May"
.Ls8:
	.string	"June"
.Ls9:
	.string	"July"
.Ls10:
	.string	"August"
.Ls11:
	.string	"September"
.Ls12:
	.string	"October"
.Ls13:
	.string	"November"
.Ls14:
	.string	"December"
	.data
	.globl	daytab
	.type	daytab, @object
	.size	daytab, 26
daytab:
	.byte	0
	.byte	31
	.byte	28
	.byte	31
	.byte	30
	.byte	31
	.byte	30
	.byte	31
	.byte	31
	.byte	30
	.byte	31
	.byte	30
	.byte	31
	.byte	0
	.byte	31
	.byte	29
	.byte	31
	.byte	30
	.byte	31
	.byte	30
	.byte	31
	.byte	31
	.byte	30
	.byte	31
	.byte	30
	.byte	31
	.globl	name
	.type	name, @object
	.size	name, 104
	.p2align 3
name:
	.quad	.Ls2
	.quad	.Ls3
	.quad	.Ls4
	.quad	.Ls5
	.quad	.Ls6
	.quad	.Ls7
	.quad	.Ls8
	.quad	.Ls9
	.quad	.Ls10
	.quad	.Ls11
	.quad	.Ls12
	.quad	.Ls13
	.quad	.Ls14
	.section	.note.GNU-stack,"",@progbits
