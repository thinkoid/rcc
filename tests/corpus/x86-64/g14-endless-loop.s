	.file	"g14-endless-loop.c"
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
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -8(%rbp)
	.p2align 5
.Lmain.2:
	movl	-8(%rbp), %eax
	movl	$2, %ecx
	xorl	%edx, %edx
	divl	%ecx
	cmpl	$0, %edx
	je	.Lmain.3
	addl	$2, -8(%rbp)
	jmp	.Lmain.2
.Lmain.3:
	movl	$0, %eax
	jmp	.Lmain.1
.Lmain.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	main, .-main
	.section	.note.GNU-stack,"",@progbits
