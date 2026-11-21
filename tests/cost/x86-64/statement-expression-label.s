	.file	"statement-expression-label.c"
	.text
	.globl	counted
	.type	counted, @function
counted:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	movq	%rax, -16(%rbp)
	movl	$0, -20(%rbp)
.Lcounted.2:
	movl	-8(%rbp), %eax
	movl	%eax, %ecx
	subl	$1, %eax
	movl	%eax, -8(%rbp)
	cmpl	$0, %ecx
	jle	.Lcounted.3
	addl	$1, -20(%rbp)
	jmp	.Lcounted.2
.Lcounted.3:
	movl	-20(%rbp), %eax
	addl	-16(%rbp), %eax
	jmp	.Lcounted.1
.Lcounted.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	counted, .-counted
	.section	.note.GNU-stack,"",@progbits
