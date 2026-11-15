	.file	"header-stdlib.c"
	.text
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$48, %rsp
	leaq	.Ls0(%rip), %rax
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rdi
	call	atoi@PLT
	cmpl	$42, %eax
	je	.Lmain.2
	movl	$1, %eax
	jmp	.Lmain.1
.Lmain.2:
	movl	$-7, -12(%rbp)
	movl	-12(%rbp), %edi
	call	abs@PLT
	cmpl	$7, %eax
	je	.Lmain.3
	movl	$2, %eax
	jmp	.Lmain.1
.Lmain.3:
	leaq	.Ls1(%rip), %rax
	movq	%rax, -24(%rbp)
	movq	$0, -32(%rbp)
	movl	$16, -36(%rbp)
	movq	-24(%rbp), %rdi
	movq	-32(%rbp), %rsi
	movl	-36(%rbp), %edx
	call	strtol@PLT
	cmpq	$-255, %rax
	je	.Lmain.4
	movl	$3, %eax
	jmp	.Lmain.1
.Lmain.4:
	movl	$0, %eax
	jmp	.Lmain.1
.Lmain.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	main, .-main
	.data
.Ls0:
	.string	"42"
.Ls1:
	.string	"-ff"
	.section	.note.GNU-stack,"",@progbits
