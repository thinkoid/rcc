	.file	"ptr-null.c"
	.text
	.globl	test
	.type	test, @function
test:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	cmpq	$0, %rax
	je	.Ltest.2
	movl	$1, %eax
	jmp	.Ltest.1
.Ltest.2:
	movl	$0, %eax
	jmp	.Ltest.1
.Ltest.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	test, .-test
	.globl	equal
	.type	equal, @function
equal:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	cmpq	$0, %rax
	sete	%al
	movzbl	%al, %eax
	jmp	.Lequal.1
.Lequal.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	equal, .-equal
	.section	.note.GNU-stack,"",@progbits
