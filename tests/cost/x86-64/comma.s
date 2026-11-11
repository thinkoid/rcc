	.file	"comma.c"
	.text
	.globl	sum
	.type	sum, @function
sum:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	$0, -16(%rbp)
	movl	$0, -8(%rbp)
	movl	$0, -12(%rbp)
	.p2align 5
.Lsum.2:
	movl	-8(%rbp), %eax
	movl	-4(%rbp), %ecx
	cmpl	%ecx, %eax
	jge	.Lsum.4
	movl	-12(%rbp), %eax
	addl	%eax, -16(%rbp)
.Lsum.3:
	addl	$1, -8(%rbp)
	addl	$2, -12(%rbp)
	jmp	.Lsum.2
.Lsum.4:
	movl	-16(%rbp), %eax
	jmp	.Lsum.1
.Lsum.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	sum, .-sum
	.globl	last
	.type	last, @function
last:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	addl	$1, -4(%rbp)
	movl	-8(%rbp), %eax
	jmp	.Llast.1
.Llast.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	last, .-last
	.globl	test
	.type	test, @function
test:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	%edx, -12(%rbp)
	movl	-8(%rbp), %eax
	movl	%eax, -4(%rbp)
	movl	-12(%rbp), %eax
	cmpl	$0, %eax
	je	.Ltest.2
	movl	-4(%rbp), %eax
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
	.section	.note.GNU-stack,"",@progbits
