	.file	"spill.c"
	.text
	.globl	spill
	.type	spill, @function
spill:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	%edx, -12(%rbp)
	movl	%ecx, -16(%rbp)
	movl	%r8d, -20(%rbp)
	movl	%r9d, -24(%rbp)
	movl	-4(%rbp), %eax
	movl	-8(%rbp), %ecx
	movl	-12(%rbp), %edx
	movl	-16(%rbp), %esi
	movl	-20(%rbp), %edi
	movl	-24(%rbp), %r8d
	movl	-4(%rbp), %r9d
	movl	-8(%rbp), %r10d
	movl	-12(%rbp), %r11d
	movq	%rax, -32(%rbp)
	movl	-16(%rbp), %eax
	movq	%rcx, -40(%rbp)
	movl	-20(%rbp), %ecx
	subl	%ecx, %eax
	subl	%eax, %r11d
	subl	%r11d, %r10d
	subl	%r10d, %r9d
	subl	%r9d, %r8d
	subl	%r8d, %edi
	subl	%edi, %esi
	subl	%esi, %edx
	movq	-40(%rbp), %rax
	subl	%edx, %eax
	movq	-32(%rbp), %rcx
	subl	%eax, %ecx
	movl	%ecx, %eax
	jmp	.L1
.L1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	spill, .-spill
	.section	.note.GNU-stack,"",@progbits
