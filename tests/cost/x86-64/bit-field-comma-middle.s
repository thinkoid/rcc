	.file	"bit-field-comma-middle.c"
	.text
	.globl	mark
	.type	mark, @function
mark:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	r(%rip), %eax
	movl	%eax, g(%rip)
	movq	-8(%rbp), %rax
	orb	$1, 1(%rax)
	movl	r(%rip), %eax
	movl	%eax, g(%rip)
.Lmark.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	mark, .-mark
	.globl	set
	.type	set, @function
set:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	r(%rip), %eax
	movl	%eax, g(%rip)
	movq	-8(%rbp), %rax
	movl	r(%rip), %ecx
	shll	$3, %ecx
	movq	-8(%rbp), %rdx
	movsbl	(%rdx), %edx
	andl	$-249, %edx
	orl	%edx, %ecx
	movb	%cl, (%rax)
	movl	r(%rip), %eax
	movl	%eax, g(%rip)
.Lset.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	set, .-set
	.bss
	.globl	g
	.type	g, @object
	.size	g, 4
	.p2align 2
g:
	.zero	4
	.globl	r
	.type	r, @object
	.size	r, 4
	.p2align 2
r:
	.zero	4
	.section	.note.GNU-stack,"",@progbits
