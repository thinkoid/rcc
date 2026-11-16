	.file	"header-stdout.c"
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
	subq	$64, %rsp
	movl	$0, -4(%rbp)
	.p2align 5
.Lmain.2:
	cmpl	$10, -4(%rbp)
	jge	.Lmain.4
	leaq	table(%rip), %rax
	movslq	-4(%rbp), %rcx
	movl	-4(%rbp), %edx
	imull	-4(%rbp), %edx
	movl	%edx, (%rax,%rcx,4)
	addl	$1, count(%rip)
.Lmain.3:
	addl	$1, -4(%rbp)
	jmp	.Lmain.2
.Lmain.4:
	movl	$0, -4(%rbp)
	.p2align 5
.Lmain.5:
	movl	-4(%rbp), %eax
	cmpl	count(%rip), %eax
	jge	.Lmain.7
	leaq	table(%rip), %rax
	movslq	-4(%rbp), %rcx
	movslq	(%rax,%rcx,4), %rax
	addq	%rax, total(%rip)
.Lmain.6:
	addl	$1, -4(%rbp)
	jmp	.Lmain.5
.Lmain.7:
	cmpq	$285, total(%rip)
	je	.Lmain.8
	movl	$1, %eax
	jmp	.Lmain.1
.Lmain.8:
	movq	stdout@GOTPCREL(%rip), %rax
	movq	(%rax), %rax
	movq	%rax, -16(%rbp)
	leaq	.Ls0(%rip), %rax
	movq	%rax, -24(%rbp)
	movl	count(%rip), %eax
	movl	%eax, -28(%rbp)
	movq	total(%rip), %rax
	movq	%rax, -40(%rbp)
	movq	-16(%rbp), %rdi
	movq	-24(%rbp), %rsi
	movl	-28(%rbp), %edx
	movq	-40(%rbp), %rcx
	movl	$0, %eax
	call	fprintf@PLT
	cmpl	$22, %eax
	je	.Lmain.9
	movl	$2, %eax
	jmp	.Lmain.1
.Lmain.9:
	leaq	.Ls1(%rip), %rax
	movq	%rax, -48(%rbp)
	movq	stdout@GOTPCREL(%rip), %rax
	movq	(%rax), %rax
	movq	%rax, -56(%rbp)
	movq	-48(%rbp), %rdi
	movq	-56(%rbp), %rsi
	call	fputs@PLT
	cmpl	$0, %eax
	jge	.Lmain.10
	movl	$3, %eax
	jmp	.Lmain.1
.Lmain.10:
	movl	table+12(%rip), %eax
	addl	count(%rip), %eax
	jmp	.Lmain.1
.Lmain.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	main, .-main
	.data
.Ls0:
	.string	"%d squares, total %ld\n"
.Ls1:
	.string	"done\n"
	.data
	.globl	total
	.type	total, @object
	.size	total, 8
	.p2align 3
total:
	.quad	0
	.bss
	.globl	count
	.type	count, @object
	.size	count, 4
	.p2align 2
count:
	.zero	4
	.globl	table
	.type	table, @object
	.size	table, 40
	.p2align 4
table:
	.zero	40
	.section	.note.GNU-stack,"",@progbits
