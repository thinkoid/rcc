	.file	"while.c"
	.text
	.globl	count
	.type	count, @function
count:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	$0, -8(%rbp)
.Lcount.2:
	movl	-8(%rbp), %eax
	movl	-4(%rbp), %ecx
	cmpl	%ecx, %eax
	jge	.Lcount.3
	movl	-8(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -8(%rbp)
	jmp	.Lcount.2
.Lcount.3:
	movl	-8(%rbp), %eax
	jmp	.Lcount.1
.Lcount.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	count, .-count
	.section	.note.GNU-stack,"",@progbits
