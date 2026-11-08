	.file	"select.c"
	.text
	.globl	select
	.type	select, @function
select:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	movl	-8(%rbp), %ecx
	cmpl	%ecx, %eax
	jge	.L2
	movl	-4(%rbp), %eax
	jmp	.L3
.L2:
	movl	-8(%rbp), %eax
.L3:
	jmp	.L1
.L1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	select, .-select
	.section	.note.GNU-stack,"",@progbits
