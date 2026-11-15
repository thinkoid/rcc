	.file	"global-address.c"
	.text
	.data
.Ls0:
	.string	"abc"
.Ls1:
	.string	"abc"
	.data
	.globl	p
	.type	p, @object
	.size	p, 8
	.p2align 3
p:
	.quad	n
	.globl	element
	.type	element, @object
	.size	element, 8
	.p2align 3
element:
	.quad	a+12
	.globl	decay
	.type	decay, @object
	.size	decay, 8
	.p2align 3
decay:
	.quad	a
	.globl	sum
	.type	sum, @object
	.size	sum, 8
	.p2align 3
sum:
	.quad	a+8
	.globl	difference
	.type	difference, @object
	.size	difference, 8
	.p2align 3
difference:
	.quad	a+4
	.globl	cell
	.type	cell, @object
	.size	cell, 8
	.p2align 3
cell:
	.quad	m+24
	.globl	row
	.type	row, @object
	.size	row, 8
	.p2align 3
row:
	.quad	m+16
	.globl	bytes
	.type	bytes, @object
	.size	bytes, 8
	.p2align 3
bytes:
	.quad	n+1
	.globl	handler
	.type	handler, @object
	.size	handler, 8
	.p2align 3
handler:
	.quad	on_signal
	.globl	handler_address
	.type	handler_address, @object
	.size	handler_address, 8
	.p2align 3
handler_address:
	.quad	on_signal
	.globl	string
	.type	string, @object
	.size	string, 8
	.p2align 3
string:
	.quad	.Ls0
	.globl	suffix
	.type	suffix, @object
	.size	suffix, 8
	.p2align 3
suffix:
	.quad	.Ls1+1
	.globl	out
	.type	out, @object
	.size	out, 8
	.p2align 3
out:
	.quad	stdout
	.globl	put
	.type	put, @object
	.size	put, 8
	.p2align 3
put:
	.quad	putchar
	.globl	fixed
	.type	fixed, @object
	.size	fixed, 8
	.p2align 3
fixed:
	.quad	4096
	.globl	null
	.type	null, @object
	.size	null, 8
	.p2align 3
null:
	.quad	0
	.globl	void_null
	.type	void_null, @object
	.size	void_null, 8
	.p2align 3
void_null:
	.quad	0
	.globl	where
	.type	where, @object
	.size	where, 8
	.p2align 3
where:
	.quad	n
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
	.p2align 4
a:
	.zero	16
	.globl	m
	.type	m, @object
	.size	m, 48
	.p2align 4
m:
	.zero	48
	.section	.note.GNU-stack,"",@progbits
