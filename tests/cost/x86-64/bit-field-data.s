	.file	"bit-field-data.c"
	.text
	.data
	.globl	job
	.type	job, @object
	.size	job, 16
	.p2align 3
job:
	.long	1
	.byte	3
	.byte	0
	.byte	2
	.byte	26
	.quad	0
	.globl	small
	.type	small, @object
	.size	small, 8
	.p2align 3
small:
	.byte	237
	.byte	100
	.byte	255
	.byte	255
	.byte	255
	.byte	255
	.byte	127
	.zero	1
	.globl	file
	.type	file, @object
	.size	file, 16
	.p2align 3
file:
	.long	7
	.byte	255
	.byte	255
	.byte	255
	.byte	120
	.quad	9
	.globl	again
	.type	again, @object
	.size	again, 8
	.p2align 3
again:
	.byte	248
	.byte	128
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.zero	1
	.globl	one
	.type	one, @object
	.size	one, 4
	.p2align 2
one:
	.byte	5
	.zero	3
	.globl	skip
	.type	skip, @object
	.size	skip, 8
	.p2align 2
skip:
	.byte	120
	.zero	3
	.byte	2
	.byte	99
	.zero	2
	.section	.note.GNU-stack,"",@progbits
