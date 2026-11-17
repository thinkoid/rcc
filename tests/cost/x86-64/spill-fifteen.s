	.file	"spill-fifteen.c"
	.text
	.globl	spill_fifteen
	.type	spill_fifteen, @function
spill_fifteen:
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
	movl	16(%rbp), %r9d
	movl	24(%rbp), %r10d
	movl	-4(%rbp), %r11d
	movq	%rax, -32(%rbp)
	movl	-8(%rbp), %eax
	movq	%rcx, -40(%rbp)
	movl	-12(%rbp), %ecx
	movq	%rdx, -48(%rbp)
	movl	-16(%rbp), %edx
	movq	%rsi, -56(%rbp)
	movl	-20(%rbp), %esi
	movq	%rdi, -64(%rbp)
	movl	-24(%rbp), %edi
	movq	%r8, -72(%rbp)
	movl	16(%rbp), %r8d
	movq	%r9, -80(%rbp)
	movl	24(%rbp), %r9d
	subl	-4(%rbp), %r9d
	subl	%r9d, %r8d
	subl	%r8d, %edi
	subl	%edi, %esi
	subl	%esi, %edx
	subl	%edx, %ecx
	subl	%ecx, %eax
	subl	%eax, %r11d
	subl	%r11d, %r10d
	movq	-80(%rbp), %rcx
	subl	%r10d, %ecx
	movq	-72(%rbp), %rdx
	subl	%ecx, %edx
	movq	-64(%rbp), %rcx
	subl	%edx, %ecx
	movq	-56(%rbp), %rdx
	subl	%ecx, %edx
	movq	-48(%rbp), %rcx
	subl	%edx, %ecx
	movq	-40(%rbp), %rdx
	subl	%ecx, %edx
	movq	-32(%rbp), %rax
	subl	%edx, %eax
	jmp	.Lspill_fifteen.1
.Lspill_fifteen.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	spill_fifteen, .-spill_fifteen
	.section	.note.GNU-stack,"",@progbits
