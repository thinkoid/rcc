	.file	"floating-size.c"
	.text
	.globl	float_size
	.type	float_size, @function
float_size:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	$4, %rax
	jmp	.Lfloat_size.1
.Lfloat_size.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	float_size, .-float_size
	.globl	double_size
	.type	double_size, @function
double_size:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	$8, %rax
	jmp	.Ldouble_size.1
.Ldouble_size.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	double_size, .-double_size
	.globl	long_double_size
	.type	long_double_size, @function
long_double_size:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	$16, %rax
	jmp	.Llong_double_size.1
.Llong_double_size.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	long_double_size, .-long_double_size
	.globl	double_array_size
	.type	double_array_size, @function
double_array_size:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	$24, %rax
	jmp	.Ldouble_array_size.1
.Ldouble_array_size.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	double_array_size, .-double_array_size
	.globl	double_alignment
	.type	double_alignment, @function
double_alignment:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	$8, %rax
	jmp	.Ldouble_alignment.1
.Ldouble_alignment.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	double_alignment, .-double_alignment
	.globl	long_double_alignment
	.type	long_double_alignment, @function
long_double_alignment:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	$16, %rax
	jmp	.Llong_double_alignment.1
.Llong_double_alignment.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	long_double_alignment, .-long_double_alignment
	.section	.note.GNU-stack,"",@progbits
