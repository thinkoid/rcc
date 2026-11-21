	.file	"global-designators.c"
	.text
	.data
.Ls0:
	.string	"Jan"
.Ls1:
	.string	"Dec"
	.data
	.globl	table
	.type	table, @object
	.size	table, 24
	.p2align 2
table:
	.zero	4
	.long	2
	.long	3
	.zero	4
	.long	5
	.zero	4
	.globl	sparse
	.type	sparse, @object
	.size	sparse, 40
	.p2align 2
sparse:
	.zero	36
	.long	1
	.globl	again
	.type	again, @object
	.size	again, 12
	.p2align 2
again:
	.long	9
	.long	2
	.long	3
	.globl	corner
	.type	corner, @object
	.size	corner, 8
	.p2align 2
corner:
	.zero	4
	.long	1
	.globl	line
	.type	line, @object
	.size	line, 16
	.p2align 2
line:
	.zero	4
	.long	2
	.long	3
	.long	4
	.globl	points
	.type	points, @object
	.size	points, 24
	.p2align 2
points:
	.zero	4
	.long	6
	.long	4
	.long	5
	.zero	8
	.globl	lines
	.type	lines, @object
	.size	lines, 32
	.p2align 2
lines:
	.zero	12
	.long	8
	.long	9
	.zero	4
	.long	7
	.zero	4
	.globl	number
	.type	number, @object
	.size	number, 4
	.p2align 2
number:
	.byte	120
	.zero	3
	.globl	named
	.type	named, @object
	.size	named, 8
	.p2align 2
named:
	.byte	97
	.byte	88
	.byte	89
	.zero	1
	.long	5
	.globl	months
	.type	months, @object
	.size	months, 96
	.p2align 3
months:
	.quad	.Ls0
	.zero	80
	.quad	.Ls1
	.section	.note.GNU-stack,"",@progbits
