	.file	"global-braces.c"
	.text
	.data
.Ls0:
	.string	"Jan"
.Ls1:
	.string	"Feb"
.Ls2:
	.string	"abc"
	.data
	.globl	counted
	.type	counted, @object
	.size	counted, 12
	.p2align 2
counted:
	.long	1
	.long	2
	.long	3
	.globl	padded
	.type	padded, @object
	.size	padded, 20
	.p2align 2
padded:
	.long	1
	.long	2
	.zero	12
	.globl	table
	.type	table, @object
	.size	table, 24
	.p2align 2
table:
	.long	1
	.long	2
	.long	3
	.long	4
	.long	5
	.long	6
	.globl	partly
	.type	partly, @object
	.size	partly, 24
	.p2align 2
partly:
	.long	1
	.zero	8
	.long	4
	.long	5
	.zero	4
	.globl	rows
	.type	rows, @object
	.size	rows, 24
	.p2align 2
rows:
	.long	1
	.long	2
	.long	3
	.long	4
	.zero	8
	.globl	narrow
	.type	narrow, @object
	.size	narrow, 1
narrow:
	.byte	-1
	.globl	bytes
	.type	bytes, @object
	.size	bytes, 1
bytes:
	.byte	255
	.globl	shorts
	.type	shorts, @object
	.size	shorts, 2
	.p2align 1
shorts:
	.short	-3
	.globl	longs
	.type	longs, @object
	.size	longs, 8
	.p2align 3
longs:
	.quad	-1
	.globl	ulongs
	.type	ulongs, @object
	.size	ulongs, 8
	.p2align 3
ulongs:
	.quad	18446744073709551615
	.globl	names
	.type	names, @object
	.size	names, 16
	.p2align 3
names:
	.quad	.Ls0
	.quad	.Ls1
	.globl	pointers
	.type	pointers, @object
	.size	pointers, 32
	.p2align 3
pointers:
	.quad	n
	.quad	a+8
	.quad	on_signal
	.quad	0
	.globl	months
	.type	months, @object
	.size	months, 8
months:
	.string	"Jan"
	.string	"Feb"
	.globl	letters
	.type	letters, @object
	.size	letters, 8
letters:
	.byte	74
	.byte	97
	.byte	110
	.zero	1
	.string	"Feb"
	.globl	words
	.type	words, @object
	.size	words, 6
words:
	.ascii	"abc"
	.zero	3
	.globl	scalar
	.type	scalar, @object
	.size	scalar, 4
	.p2align 2
scalar:
	.long	5
	.globl	string
	.type	string, @object
	.size	string, 8
	.p2align 3
string:
	.quad	.Ls2
	.globl	zero
	.type	zero, @object
	.size	zero, 4
	.p2align 2
zero:
	.zero	4
	.globl	empty
	.type	empty, @object
	.size	empty, 12
	.p2align 2
empty:
	.zero	12
	.bss
	.globl	n
	.type	n, @object
	.size	n, 4
	.p2align 2
n:
	.zero	4
	.globl	a
	.type	a, @object
	.size	a, 16
	.p2align 2
a:
	.zero	16
	.section	.note.GNU-stack,"",@progbits
