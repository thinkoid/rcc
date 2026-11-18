	.file	"widen.c"
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
	movsbq	-1(%rbp), %rax
	jmp	.Lsigned_char.1
.Lsigned_char.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	signed_char, .-signed_char
	.globl	short_
	.type	short_, @function
short_:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movw	%di, -2(%rbp)
	movswq	-2(%rbp), %rax
	jmp	.Lshort_.1
.Lshort_.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	short_, .-short_
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
	movslq	-4(%rbp), %rax
	jmp	.Lint_.1
.Lint_.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	int_, .-int_
	.globl	char_
	.type	char_, @function
char_:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movb	%dil, -1(%rbp)
	movsbq	-1(%rbp), %rax
	jmp	.Lchar_.1
.Lchar_.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	char_, .-char_
	.globl	unsigned_char
	.type	unsigned_char, @function
unsigned_char:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movb	%dil, -1(%rbp)
	movzbl	-1(%rbp), %eax
	jmp	.Lunsigned_char.1
.Lunsigned_char.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	unsigned_char, .-unsigned_char
	.globl	defined_
	.type	defined_, @function
defined_:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movslq	defined(%rip), %rax
	jmp	.Ldefined_.1
.Ldefined_.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	defined_, .-defined_
	.globl	declared_
	.type	declared_, @function
declared_:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	declared@GOTPCREL(%rip), %rax
	movslq	(%rax), %rax
	jmp	.Ldeclared_.1
.Ldeclared_.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	declared_, .-declared_
	.globl	pointer
	.type	pointer, @function
pointer:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movswq	(%rax), %rax
	jmp	.Lpointer.1
.Lpointer.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	pointer, .-pointer
	.globl	element
	.type	element, @function
element:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movslq	8(%rax), %rax
	jmp	.Lelement.1
.Lelement.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	element, .-element
	.globl	indexed
	.type	indexed, @function
indexed:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movb	%sil, -9(%rbp)
	movq	-8(%rbp), %rax
	movsbq	-9(%rbp), %rcx
	movl	(%rax,%rcx,4), %eax
	jmp	.Lindexed.1
.Lindexed.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	indexed, .-indexed
	.globl	promoted_char
	.type	promoted_char, @function
promoted_char:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movb	%dil, -1(%rbp)
	movsbq	-1(%rbp), %rax
	jmp	.Lpromoted_char.1
.Lpromoted_char.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	promoted_char, .-promoted_char
	.globl	promoted_unsigned
	.type	promoted_unsigned, @function
promoted_unsigned:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movw	%di, -2(%rbp)
	movzwl	-2(%rbp), %eax
	jmp	.Lpromoted_unsigned.1
.Lpromoted_unsigned.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	promoted_unsigned, .-promoted_unsigned
	.globl	unsigned_promoted
	.type	unsigned_promoted, @function
unsigned_promoted:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movb	%dil, -1(%rbp)
	movsbl	-1(%rbp), %eax
	jmp	.Lunsigned_promoted.1
.Lunsigned_promoted.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	unsigned_promoted, .-unsigned_promoted
	.globl	promoted_signed
	.type	promoted_signed, @function
promoted_signed:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movb	%dil, -1(%rbp)
	movsbq	-1(%rbp), %rax
	jmp	.Lpromoted_signed.1
.Lpromoted_signed.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	promoted_signed, .-promoted_signed
	.globl	unsigned_twice
	.type	unsigned_twice, @function
unsigned_twice:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movb	%dil, -1(%rbp)
	movzbl	-1(%rbp), %eax
	jmp	.Lunsigned_twice.1
.Lunsigned_twice.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	unsigned_twice, .-unsigned_twice
	.globl	unsigned_called
	.type	unsigned_called, @function
unsigned_called:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	call	narrow@PLT
	movsbl	%al, %eax
	movl	%eax, %eax
	jmp	.Lunsigned_called.1
.Lunsigned_called.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	unsigned_called, .-unsigned_called
	.globl	unsigned_register
	.type	unsigned_register, @function
unsigned_register:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movb	%dil, -9(%rbp)
	movsbl	-9(%rbp), %ebx
	movl	%ebx, %eax
	jmp	.Lunsigned_register.1
.Lunsigned_register.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	unsigned_register, .-unsigned_register
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
	movswl	-2(%rbp), %eax
	jmp	.Lunsigned_short.1
.Lunsigned_short.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	unsigned_short, .-unsigned_short
	.globl	unsigned_int_called
	.type	unsigned_int_called, @function
unsigned_int_called:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	call	whole@PLT
	movl	%eax, %eax
	jmp	.Lunsigned_int_called.1
.Lunsigned_int_called.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	unsigned_int_called, .-unsigned_int_called
	.bss
	.globl	defined
	.type	defined, @object
	.size	defined, 4
	.p2align 2
defined:
	.zero	4
	.section	.note.GNU-stack,"",@progbits
