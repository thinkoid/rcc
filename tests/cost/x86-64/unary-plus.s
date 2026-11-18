	.file	"unary-plus.c"
	.text
	.globl	signed_char
	.type	signed_char, @function
signed_char:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movb	%dil, -1(%rbp)
	movsbl	-1(%rbp), %eax
	jmp	.Lsigned_char.1
.Lsigned_char.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	signed_char, .-signed_char
	.globl	plain_char
	.type	plain_char, @function
plain_char:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movb	%dil, -1(%rbp)
	movsbl	-1(%rbp), %eax
	jmp	.Lplain_char.1
.Lplain_char.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	plain_char, .-plain_char
	.globl	unsigned_short
	.type	unsigned_short, @function
unsigned_short:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movw	%di, -2(%rbp)
	movzwl	-2(%rbp), %eax
	jmp	.Lunsigned_short.1
.Lunsigned_short.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	unsigned_short, .-unsigned_short
	.globl	int_
	.type	int_, @function
int_:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	jmp	.Lint_.1
.Lint_.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	int_, .-int_
	.globl	long_
	.type	long_, @function
long_:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	jmp	.Llong_.1
.Llong_.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	long_, .-long_
	.globl	register_
	.type	register_, @function
register_:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movl	%edi, %ebx
	movl	%ebx, %eax
	jmp	.Lregister_.1
.Lregister_.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	register_, .-register_
	.globl	operand
	.type	operand, @function
operand:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movb	%sil, -5(%rbp)
	movl	-4(%rbp), %eax
	movsbl	-5(%rbp), %ecx
	addl	%ecx, %eax
	jmp	.Loperand.1
.Loperand.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	operand, .-operand
	.section	.note.GNU-stack,"",@progbits
