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
	movq	%r10, -88(%rbp)
	movl	-4(%rbp), %r10d
	subl	%r10d, %r9d
	subl	%r9d, %r8d
	subl	%r8d, %edi
	subl	%edi, %esi
	subl	%esi, %edx
	subl	%edx, %ecx
	subl	%ecx, %eax
	subl	%eax, %r11d
	movq	-88(%rbp), %rax
	subl	%r11d, %eax
	movq	-80(%rbp), %rcx
	subl	%eax, %ecx
	movq	-72(%rbp), %rax
	subl	%ecx, %eax
	movq	-64(%rbp), %rcx
	subl	%eax, %ecx
	movq	-56(%rbp), %rax
	subl	%ecx, %eax
	movq	-48(%rbp), %rcx
	subl	%eax, %ecx
	movq	-40(%rbp), %rax
	subl	%ecx, %eax
	movq	-32(%rbp), %rcx
	subl	%eax, %ecx
	movl	%ecx, %eax
	jmp	.Lspill_fifteen.1
.Lspill_fifteen.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	spill_fifteen, .-spill_fifteen
	.section	.note.GNU-stack,"",@progbits
