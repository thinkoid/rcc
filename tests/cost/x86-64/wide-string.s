	.file	"wide-string.c"
	.text
	.globl	greeting
	.type	greeting, @function
greeting:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	leaq	.Ls0(%rip), %rax
	jmp	.Lgreeting.1
.Lgreeting.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	greeting, .-greeting
	.globl	joined
	.type	joined, @function
joined:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	leaq	.Ls1(%rip), %rax
	jmp	.Ljoined.1
.Ljoined.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	joined, .-joined
	.globl	wide_constants
	.type	wide_constants, @function
wide_constants:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	$128865, %rax
	jmp	.Lwide_constants.1
.Lwide_constants.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	wide_constants, .-wide_constants
	.globl	second
	.type	second, @function
second:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	.Ls2+4(%rip), %eax
	jmp	.Lsecond.1
.Lsecond.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	second, .-second
	.data
	.p2align 2
.Ls0:
	.long	104, 233, 0
	.p2align 2
.Ls1:
	.long	233, 33, 0
	.p2align 2
.Ls2:
	.long	120, 121, 122, 0
	.data
	.globl	padded
	.type	padded, @object
	.size	padded, 32
	.p2align 4
padded:
	.long	97, 98, 0
	.zero	20
	.globl	code_units
	.type	code_units, @object
	.size	code_units, 8
	.p2align 1
code_units:
	.short	120, 55357, 56832, 0
	.section	.note.GNU-stack,"",@progbits
