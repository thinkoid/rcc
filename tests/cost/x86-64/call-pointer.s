	.file	"call-pointer.c"
	.text
	.globl	call
	.type	call, @function
call:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movl	%edx, -16(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -24(%rbp)
	movl	-16(%rbp), %eax
	movl	%eax, -28(%rbp)
	movl	-12(%rbp), %eax
	movl	%eax, -32(%rbp)
	movl	-28(%rbp), %edi
	movl	-32(%rbp), %esi
	call	*-24(%rbp)
	jmp	.Lcall.1
.Lcall.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	call, .-call
	.section	.note.GNU-stack,"",@progbits
