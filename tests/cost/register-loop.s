	.file	"register-loop.c"
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
	pushq	%rbx
	.cfi_offset %rbx, -24
	pushq	%r12
	.cfi_offset %r12, -32
	pushq	%r13
	.cfi_offset %r13, -40
	movl	%edi, %ebx
	movl	$0, %r12d
	movl	$0, %r13d
.Lsum.2:
	cmpl	%ebx, %r13d
	jge	.Lsum.4
	movl	%r12d, %eax
	addl	%r13d, %eax
	movl	%eax, %r12d
.Lsum.3:
	movl	%r13d, %eax
	addl	$1, %eax
	movl	%eax, %r13d
	jmp	.Lsum.2
.Lsum.4:
	movl	%r12d, %eax
	jmp	.Lsum.1
.Lsum.1:
	popq	%r13
	popq	%r12
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	sum, .-sum
	.section	.note.GNU-stack,"",@progbits
