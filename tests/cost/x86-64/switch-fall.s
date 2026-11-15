	.file	"switch-fall.c"
	.text
	.globl	score
	.type	score, @function
score:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	$0, -8(%rbp)
	movl	-4(%rbp), %eax
	subl	$1, %eax
	cmpl	$4, %eax
	ja	.Lscore.4
	leaq	.Lscore.8(%rip), %rcx
	jmp	*(%rcx,%rax,8)
	.data
	.p2align 3
.Lscore.8:
	.quad	.Lscore.2
	.quad	.Lscore.3
	.quad	.Lscore.4
	.quad	.Lscore.5
	.quad	.Lscore.6
	.text
.Lscore.2:
	addl	$1, -8(%rbp)
.Lscore.3:
	addl	$2, -8(%rbp)
.Lscore.4:
	addl	$4, -8(%rbp)
.Lscore.5:
	addl	$8, -8(%rbp)
	jmp	.Lscore.7
.Lscore.6:
	addl	$16, -8(%rbp)
.Lscore.7:
	movl	-8(%rbp), %eax
	jmp	.Lscore.1
.Lscore.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	score, .-score
	.section	.note.GNU-stack,"",@progbits
