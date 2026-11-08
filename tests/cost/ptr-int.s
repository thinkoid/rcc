	.file	"ptr-int.c"
	.text
	.globl	to_long
	.type	to_long, @function
to_long:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	jmp	.L1
.L1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	to_long, .-to_long
	.globl	from_long
	.type	from_long, @function
from_long:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	jmp	.L2
.L2:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	from_long, .-from_long
	.globl	from_int
	.type	from_int, @function
from_int:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	movslq	%eax, %rax
	jmp	.L3
.L3:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	from_int, .-from_int
	.section	.note.GNU-stack,"",@progbits
