	.file	"global-data.c"
	.text
	.data
	.globl	c
	.type	c, @object
	.size	c, 1
c:
	.byte	120
	.globl	s
	.type	s, @object
	.size	s, 2
	.p2align 1
s:
	.short	-3
	.globl	i
	.type	i, @object
	.size	i, 4
	.p2align 2
i:
	.long	42
	.globl	l
	.type	l, @object
	.size	l, 8
	.p2align 3
l:
	.quad	4886718345
	.globl	ones
	.type	ones, @object
	.size	ones, 8
	.p2align 3
ones:
	.quad	18446744073709551615
	.globl	null
	.type	null, @object
	.size	null, 8
	.p2align 3
null:
	.quad	0
	.globl	zero
	.type	zero, @object
	.size	zero, 4
	.p2align 2
zero:
	.long	0
	.globl	later
	.type	later, @object
	.size	later, 4
	.p2align 2
later:
	.long	7
	.bss
	.globl	n
	.type	n, @object
	.size	n, 4
	.p2align 2
n:
	.zero	4
	.globl	table
	.type	table, @object
	.size	table, 12
	.p2align 2
table:
	.zero	12
	.globl	pair
	.type	pair, @object
	.size	pair, 16
	.p2align 4
pair:
	.zero	16
	.section	.note.GNU-stack,"",@progbits
