	.file	"narrow.c"
	.text
	.globl	f
	.type	f, @function
f:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movb	%dil, -1(%rbp)
	movw	%si, -4(%rbp)
	movsbl	-1(%rbp), %eax
	movl	$4, %ecx
	cltd
	idivl	%ecx
	movswl	-4(%rbp), %ecx
	addl	%ecx, %eax
	movsbl	%al, %eax
	jmp	.L1
.L1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	f, .-f
	.globl	g
	.type	g, @function
g:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movb	%dil, -1(%rbp)
	movzbl	-1(%rbp), %eax
	addl	$1, %eax
	movzbl	%al, %eax
	jmp	.L2
.L2:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	g, .-g
	.globl	h
	.type	h, @function
h:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	movb	%al, -5(%rbp)
	movl	-4(%rbp), %eax
	movw	%ax, -8(%rbp)
	movsbl	-5(%rbp), %edi
	movswl	-8(%rbp), %esi
	call	f@PLT
	jmp	.L3
.L3:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	h, .-h
	.section	.note.GNU-stack,"",@progbits
