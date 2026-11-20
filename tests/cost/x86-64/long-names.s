	.file	"long-names.c"
	.text
	.globl	sum
	.type	sum, @function
sum:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	$1, -4(%rbp)
	movl	$2, -8(%rbp)
	movl	significant_characters_of_an_external_identifier_counted_beyond_63_a(%rip), %eax
	addl	significant_characters_of_an_external_identifier_counted_beyond_63_b(%rip), %eax
	addl	-4(%rbp), %eax
	addl	-8(%rbp), %eax
	jmp	.Lsum.1
.Lsum.1:
	leave
	.cfi_def_cfa %rsp, 8
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
