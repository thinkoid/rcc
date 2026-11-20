	.file	"main-envp.c"
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
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	movl	$0, -28(%rbp)
	cmpl	$1, -4(%rbp)
	jne	.Lmain.3
	movq	-16(%rbp), %rax
	movslq	-4(%rbp), %rcx
	cmpq	$0, (%rax,%rcx,8)
	je	.Lmain.2
.Lmain.3:
	movl	$101, %eax
	jmp	.Lmain.1
.Lmain.2:
	movq	-24(%rbp), %rax
	movq	environ@GOTPCREL(%rip), %rcx
	cmpq	(%rcx), %rax
	je	.Lmain.4
	movl	$102, %eax
	jmp	.Lmain.1
.Lmain.4:
	.p2align 5
.Lmain.5:
	movq	-24(%rbp), %rax
	movslq	-28(%rbp), %rcx
	cmpq	$0, (%rax,%rcx,8)
	je	.Lmain.6
	addl	$1, -28(%rbp)
	jmp	.Lmain.5
.Lmain.6:
	movq	environ@GOTPCREL(%rip), %rax
	movq	(%rax), %rax
	movslq	-28(%rbp), %rcx
	cmpq	$0, (%rax,%rcx,8)
	je	.Lmain.7
	movl	$103, %eax
	jmp	.Lmain.1
.Lmain.7:
	movl	$0, %eax
.Lmain.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	main, .-main
	.section	.note.GNU-stack,"",@progbits
