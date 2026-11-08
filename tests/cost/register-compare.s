	.file	"register-compare.c"
	.text
	.globl	same
	.type	same, @function
same:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movl	%edi, -12(%rbp)
	movl	-12(%rbp), %eax
	movl	%eax, %ebx
	cmpl	%ebx, %ebx
	sete	%al
	movzbl	%al, %eax
	jmp	.L1
.L1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	same, .-same
	.section	.note.GNU-stack,"",@progbits
