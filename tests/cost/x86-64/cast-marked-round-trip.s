	.file	"cast-marked-round-trip.c"
	.text
	.globl	through_void
	.type	through_void, @function
through_void:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, plain(%rip)
	movq	plain(%rip), %rax
	movq	%rax, -8(%rbp)
.Lthrough_void.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	through_void, .-through_void
	.globl	through_integer
	.type	through_integer, @function
through_integer:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, plain_result(%rip)
.Lthrough_integer.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	through_integer, .-through_integer
	.globl	same
	.type	same, @function
same:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, marked(%rip)
.Lsame.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	same, .-same
	.bss
	.globl	plain
	.type	plain, @object
	.size	plain, 8
	.p2align 3
plain:
	.zero	8
	.globl	plain_result
	.type	plain_result, @object
	.size	plain_result, 8
	.p2align 3
plain_result:
	.zero	8
	.globl	marked
	.type	marked, @object
	.size	marked, 8
	.p2align 3
marked:
	.zero	8
	.section	.note.GNU-stack,"",@progbits
