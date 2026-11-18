	.file	"struct-data.c"
	.text
	.data
	.globl	table
	.type	table, @object
	.size	table, 64
	.p2align 3
table:
	.short	0
	.short	4
	.short	8
	.short	32
	.quad	0
	.short	4
	.short	4
	.short	8
	.short	4
	.quad	fast
	.short	4
	.short	5
	.short	16
	.short	8
	.quad	fast
	.short	8
	.short	16
	.short	128
	.short	128
	.quad	slow
	.globl	codes
	.type	codes, @object
	.size	codes, 8
	.p2align 1
codes:
	.short	12
	.short	8
	.short	140
	.short	8
	.globl	names
	.type	names, @object
	.size	names, 24
	.p2align 2
names:
	.string	"one"
	.zero	4
	.long	1
	.ascii	"three"
	.zero	3
	.long	3
	.globl	segment
	.type	segment, @object
	.size	segment, 16
	.p2align 2
segment:
	.long	1
	.long	2
	.long	3
	.long	4
	.globl	partial
	.type	partial, @object
	.size	partial, 16
	.p2align 2
partial:
	.long	7
	.zero	12
	.globl	nested
	.type	nested, @object
	.size	nested, 32
	.p2align 3
nested:
	.long	1
	.zero	4
	.byte	97
	.zero	7
	.quad	2
	.short	3
	.zero	6
	.globl	either
	.type	either, @object
	.size	either, 8
	.p2align 3
either:
	.byte	120
	.zero	7
	.globl	second
	.type	second, @object
	.size	second, 8
	.p2align 3
second:
	.quad	segment+8
	.globl	third
	.type	third, @object
	.size	third, 8
	.p2align 3
third:
	.quad	table+32
	.section	.note.GNU-stack,"",@progbits
