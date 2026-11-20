	.file	"long-names.c"
	.text
	.globl	sum
	.type	sum, @function
sum:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	mov	w0, #1
	str	w0, [x29, #16]
	mov	w0, #2
	str	w0, [x29, #20]
	adrp	x16, significant_characters_of_an_external_identifier_counted_beyond_63_a
	ldr	w0, [x16, :lo12:significant_characters_of_an_external_identifier_counted_beyond_63_a]
	adrp	x16, significant_characters_of_an_external_identifier_counted_beyond_63_b
	ldr	w1, [x16, :lo12:significant_characters_of_an_external_identifier_counted_beyond_63_b]
	add	w0, w0, w1
	ldr	w1, [x29, #16]
	add	w0, w0, w1
	ldr	w1, [x29, #20]
	add	w0, w0, w1
	b	.Lsum.1
.Lsum.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	sum, .-sum
	.bss
	.globl	significant_characters_of_an_external_identifier_counted_beyond_63_a
	.type	significant_characters_of_an_external_identifier_counted_beyond_63_a, @object
	.size	significant_characters_of_an_external_identifier_counted_beyond_63_a, 4
	.p2align 2
significant_characters_of_an_external_identifier_counted_beyond_63_a:
	.zero	4
	.globl	significant_characters_of_an_external_identifier_counted_beyond_63_b
	.type	significant_characters_of_an_external_identifier_counted_beyond_63_b, @object
	.size	significant_characters_of_an_external_identifier_counted_beyond_63_b, 4
	.p2align 2
significant_characters_of_an_external_identifier_counted_beyond_63_b:
	.zero	4
	.section	.note.GNU-stack,"",@progbits
