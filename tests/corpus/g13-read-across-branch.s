	.file	"g13-read-across-branch.c"
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
	pushq	%rbx
	.cfi_offset %rbx, -24
	movl	%edi, %ebx
	movl	%esi, -12(%rbp)
	movq	%rbx, -24(%rbp)
	movl	-12(%rbp), %eax
	cmpl	$0, %eax
	je	.Lpick.2
	movl	%ebx, %eax
	addl	$1, %ebx
	jmp	.Lpick.3
.Lpick.2:
	movl	$2, %eax
.Lpick.3:
	movq	-24(%rbp), %rcx
	addl	%eax, %ecx
	movl	%ecx, %eax
	jmp	.Lpick.1
.Lpick.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	pick, .-pick
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movl	$5, -4(%rbp)
	movl	$1, -8(%rbp)
	movl	-4(%rbp), %edi
	movl	-8(%rbp), %esi
	call	pick@PLT
	imull	$16, %eax, %eax
	movq	%rax, -16(%rbp)
	movl	$5, -20(%rbp)
	movl	$0, -24(%rbp)
	movl	-20(%rbp), %edi
	movl	-24(%rbp), %esi
	call	pick@PLT
	movq	-16(%rbp), %rcx
	addl	%eax, %ecx
	movl	%ecx, %eax
	jmp	.Lmain.1
.Lmain.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	main, .-main
	.section	.note.GNU-stack,"",@progbits
