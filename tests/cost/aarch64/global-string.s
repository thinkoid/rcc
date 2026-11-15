	.file	"global-string.c"
	.text
	.data
.Ls0:
	.string	"abc"
	.data
	.globl	whole
	.type	whole, @object
	.size	whole, 4
whole:
	.string	"abc"
	.globl	padded
	.type	padded, @object
	.size	padded, 8
padded:
	.string	"abc"
	.zero	4
	.globl	full
	.type	full, @object
	.size	full, 3
full:
	.ascii	"abc"
	.globl	braced
	.type	braced, @object
	.size	braced, 4
braced:
	.string	"abc"
	.globl	signed_
	.type	signed_, @object
	.size	signed_, 4
signed_:
	.string	"abc"
	.globl	unsigned_
	.type	unsigned_, @object
	.size	unsigned_, 4
unsigned_:
	.string	"abc"
	.globl	escaped
	.type	escaped, @object
	.size	escaped, 9
escaped:
	.string	"a\tb\n\"\\\001\377"
	.globl	pointer
	.type	pointer, @object
	.size	pointer, 8
	.p2align 3
pointer:
	.quad	.Ls0
	.globl	long_
	.type	long_, @object
	.size	long_, 22
long_:
	.string	"sixteen or more bytes"
	.section	.note.GNU-stack,"",@progbits
