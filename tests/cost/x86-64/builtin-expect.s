	.file	"builtin-expect.c"
	.text
	.globl	pick
	.type	pick, @function
pick:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	cmpl	$0, -4(%rbp)
	setl	%al
	movzbl	%al, %eax
	cmpl	$0, %eax
	sete	%al
	movzbl	%al, %eax
	cmpl	$0, %eax
	sete	%al
	movzbl	%al, %eax
	cmpq	$0, %rax
	je	.Lpick.2
	movl	$0, %eax
	jmp	.Lpick.1
.Lpick.2:
	movl	-4(%rbp), %eax
	jmp	.Lpick.1
.Lpick.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	pick, .-pick
	.globl	wide
	.type	wide, @function
wide:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	cmpl	$0, %eax
	sete	%al
	movzbl	%al, %eax
	cmpl	$0, %eax
	sete	%al
	movzbl	%al, %eax
	jmp	.Lwide.1
.Lwide.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	wide, .-wide
	.section	.note.GNU-stack,"",@progbits
