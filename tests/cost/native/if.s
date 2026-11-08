	.file	"if.c"
	.text
	.globl	choose
	.type	choose, @function
choose:
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
	movl	%eax, -12(%rbp)
	jmp	.L3
.L2:
	movl	-8(%rbp), %eax
	movl	%eax, -12(%rbp)
.L3:
	movl	-12(%rbp), %eax
	jmp	.L1
.L1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	choose, .-choose
	.section	.note.GNU-stack,"",@progbits
