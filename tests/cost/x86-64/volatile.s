	.file	"volatile.c"
	.text
	.globl	peek
	.type	peek, @function
peek:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	$1, -12(%rbp)
	movl	-12(%rbp), %eax
	movq	-8(%rbp), %rax
	movl	(%rax), %eax
	movq	-8(%rbp), %rax
	movl	(%rax), %eax
	movq	-8(%rbp), %rcx
	movl	(%rcx), %ecx
	addl	%ecx, %eax
	jmp	.Lpeek.1
.Lpeek.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	peek, .-peek
	.globl	poke
	.type	poke, @function
poke:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movb	$65, (%rax)
.Lpoke.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	poke, .-poke
	.globl	spin
	.type	spin, @function
spin:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movl	%edi, -12(%rbp)
	movl	$0, %ebx
	.p2align 5
.Lspin.2:
	movl	-12(%rbp), %eax
	cmpl	%eax, %ebx
	jge	.Lspin.3
	addl	$1, %ebx
	jmp	.Lspin.2
.Lspin.3:
	movl	%ebx, %eax
	jmp	.Lspin.1
.Lspin.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	spin, .-spin
	.section	.note.GNU-stack,"",@progbits
