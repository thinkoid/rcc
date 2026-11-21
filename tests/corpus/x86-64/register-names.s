	.file	"register-names.c"
	.text
	.globl	h1
	.type	h1, @function
h1:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	$1, %eax
	jmp	.Lh1.1
.Lh1.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	h1, .-h1
	.globl	x0
	.type	x0, @function
x0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	addl	x1(%rip), %eax
	jmp	.Lx0.1
.Lx0.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	x0, .-x0
	.globl	w2
	.type	w2, @function
w2:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	imull	-8(%rbp), %eax
	jmp	.Lw2.1
.Lw2.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	w2, .-w2
	.globl	sp
	.type	sp, @function
sp:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	imull	$2, x1(%rip), %eax
	jmp	.Lsp.1
.Lsp.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	sp, .-sp
	.globl	Lr
	.type	Lr, @function
Lr:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	$7, %eax
	jmp	.LLr.1
.LLr.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	Lr, .-Lr
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	call	h1@PLT
	cmpl	$1, %eax
	je	.Lmain.2
	movl	$101, %eax
	jmp	.Lmain.1
.Lmain.2:
	movl	$2, -4(%rbp)
	movl	-4(%rbp), %edi
	call	x0@PLT
	cmpl	$7, %eax
	je	.Lmain.3
	movl	$102, %eax
	jmp	.Lmain.1
.Lmain.3:
	movl	$3, -8(%rbp)
	movl	$4, -12(%rbp)
	movl	-8(%rbp), %edi
	movl	-12(%rbp), %esi
	call	w2@PLT
	cmpl	$12, %eax
	je	.Lmain.4
	movl	$103, %eax
	jmp	.Lmain.1
.Lmain.4:
	movl	$10, x1(%rip)
	call	sp@PLT
	cmpl	$20, %eax
	je	.Lmain.5
	movl	$104, %eax
	jmp	.Lmain.1
.Lmain.5:
	call	Lr@PLT
	cmpl	$7, %eax
	je	.Lmain.6
	movl	$105, %eax
	jmp	.Lmain.1
.Lmain.6:
	movl	$42, %eax
	jmp	.Lmain.1
.Lmain.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	main, .-main
	.data
	.globl	x1
	.type	x1, @object
	.size	x1, 4
	.p2align 2
x1:
	.long	5
	.section	.note.GNU-stack,"",@progbits
