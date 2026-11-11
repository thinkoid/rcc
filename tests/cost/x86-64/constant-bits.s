	.file	"constant-bits.c"
	.text
	.globl	add_bits
	.type	add_bits, @function
add_bits:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	addl	$4095, %eax
	addl	$4096, %eax
	addl	$5000, %eax
	addl	$-5000, %eax
	addl	$-2147483647, %eax
	addl	$-2147483646, %eax
	jmp	.Ladd_bits.1
.Ladd_bits.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	add_bits, .-add_bits
	.globl	and_bits
	.type	and_bits, @function
and_bits:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	andl	$255, %eax
	andl	$5, %eax
	jmp	.Land_bits.1
.Land_bits.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	and_bits, .-and_bits
	.globl	long_bits
	.type	long_bits, @function
long_bits:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	addq	$-100000, %rax
	movabsq	$281470681743360, %rcx
	addq	%rcx, %rax
	movabsq	$-281470681808896, %rcx
	addq	%rcx, %rax
	jmp	.Llong_bits.1
.Llong_bits.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	long_bits, .-long_bits
	.section	.note.GNU-stack,"",@progbits
