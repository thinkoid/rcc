	.file	"g13-read-across-call.c"
	.text
	.globl	f
	.type	f, @function
f:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	addl	$100, %eax
	jmp	.Lf.1
.Lf.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	f, .-f
	.globl	sum
	.type	sum, @function
sum:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	subq	$24, %rsp
	movl	%edi, %ebx
	movl	%ebx, %eax
	movl	%ebx, %ecx
	addl	$1, %ebx
	movl	%ecx, -12(%rbp)
	movq	%rax, -24(%rbp)
	movl	-12(%rbp), %edi
	call	f@PLT
	movq	-24(%rbp), %rcx
	addl	%eax, %ecx
	movl	%ecx, %eax
	jmp	.Lsum.1
.Lsum.1:
	leaq	-8(%rbp), %rsp
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	sum, .-sum
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	$5, -4(%rbp)
	movl	-4(%rbp), %edi
	call	sum@PLT
	jmp	.Lmain.1
.Lmain.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	main, .-main
	.section	.note.GNU-stack,"",@progbits
