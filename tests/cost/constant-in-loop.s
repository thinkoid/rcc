	.file	"constant-in-loop.c"
	.text
	.globl	constant_in_loop
	.type	constant_in_loop, @function
constant_in_loop:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movl	$0, -16(%rbp)
.Lconstant_in_loop.2:
	movl	-16(%rbp), %eax
	movl	-12(%rbp), %ecx
	cmpl	%ecx, %eax
	jge	.Lconstant_in_loop.4
	movq	-8(%rbp), %rax
	movabsq	$81985529216486895, %rcx
	addq	%rcx, %rax
	movq	%rax, -8(%rbp)
.Lconstant_in_loop.3:
	movl	-16(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -16(%rbp)
	jmp	.Lconstant_in_loop.2
.Lconstant_in_loop.4:
	movq	-8(%rbp), %rax
	jmp	.Lconstant_in_loop.1
.Lconstant_in_loop.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	constant_in_loop, .-constant_in_loop
	.section	.note.GNU-stack,"",@progbits
