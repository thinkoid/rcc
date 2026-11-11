	.file	"cast.c"
	.text
	.globl	signed_
	.type	signed_, @function
signed_:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	movb	%al, -5(%rbp)
	movsbl	-5(%rbp), %eax
	movl	%eax, -12(%rbp)
	movl	-12(%rbp), %eax
	movslq	%eax, %rax
	movq	%rax, -24(%rbp)
	movq	-24(%rbp), %rax
	jmp	.Lsigned_.1
.Lsigned_.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	signed_, .-signed_
	.globl	unsigned_
	.type	unsigned_, @function
unsigned_:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	movb	%al, -5(%rbp)
	movzbl	-5(%rbp), %eax
	movl	%eax, -12(%rbp)
	movl	-12(%rbp), %eax
	jmp	.Lunsigned_.1
.Lunsigned_.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	unsigned_, .-unsigned_
	.globl	same
	.type	same, @function
same:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -8(%rbp)
	movl	-8(%rbp), %eax
	jmp	.Lsame.1
.Lsame.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	same, .-same
	.section	.note.GNU-stack,"",@progbits
