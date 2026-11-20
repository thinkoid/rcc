	.file	"parenthesized-string.c"
	.text
	.data
	.globl	bare
	.type	bare, @object
	.size	bare, 4
bare:
	.string	"abc"
	.globl	listed
	.type	listed, @object
	.size	listed, 4
listed:
	.string	"abc"
	.section	.note.GNU-stack,"",@progbits
