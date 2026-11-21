	.file	"array-local-string.c"
	.text
	.globl	exact
	.type	exact, @function
exact:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movzbl	.Ls0(%rip), %eax
	movb	%al, -4(%rbp)
	movzbl	.Ls0+1(%rip), %eax
	movb	%al, -3(%rbp)
	movzbl	.Ls0+2(%rip), %eax
	movb	%al, -2(%rbp)
	movzbl	.Ls0+3(%rip), %eax
	movb	%al, -1(%rbp)
	leaq	-4(%rbp), %rax
	movq	%rax, -16(%rbp)
	movq	-16(%rbp), %rdi
	call	use@PLT
.Lexact.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	exact, .-exact
	.globl	unsized
	.type	unsized, @function
unsized:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	leaq	.Ls1(%rip), %rax
	movq	$-17, %rdx
.Lunsized.2:
	movzbl	17(%rax,%rdx,1), %ecx
	movb	%cl, (%rbp,%rdx,1)
	addq	$1, %rdx
	jne	.Lunsized.2
	leaq	-17(%rbp), %rax
	movq	%rax, -32(%rbp)
	movq	-32(%rbp), %rdi
	call	use@PLT
.Lunsized.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	unsized, .-unsized
	.globl	padded
	.type	padded, @function
padded:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$112, %rsp
	movzbl	.Ls2(%rip), %eax
	movb	%al, -100(%rbp)
	movzbl	.Ls2+1(%rip), %eax
	movb	%al, -99(%rbp)
	movzbl	.Ls2+2(%rip), %eax
	movb	%al, -98(%rbp)
	movzbl	.Ls2+3(%rip), %eax
	movb	%al, -97(%rbp)
	movq	$-96, %rax
.Lpadded.2:
	movb	$0, (%rbp,%rax,1)
	addq	$1, %rax
	jne	.Lpadded.2
	leaq	-100(%rbp), %rax
	movq	%rax, -112(%rbp)
	movq	-112(%rbp), %rdi
	call	use@PLT
.Lpadded.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	padded, .-padded
	.globl	braced
	.type	braced, @function
braced:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movzbl	.Ls3(%rip), %eax
	movb	%al, -4(%rbp)
	movzbl	.Ls3+1(%rip), %eax
	movb	%al, -3(%rbp)
	movzbl	.Ls3+2(%rip), %eax
	movb	%al, -2(%rbp)
	movzbl	.Ls3+3(%rip), %eax
	movb	%al, -1(%rbp)
	leaq	-4(%rbp), %rax
	movq	%rax, -16(%rbp)
	movq	-16(%rbp), %rdi
	call	use@PLT
.Lbraced.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	braced, .-braced
	.globl	hundred
	.type	hundred, @function
hundred:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$112, %rsp
	leaq	.Ls4(%rip), %rax
	movq	$-100, %rdx
.Lhundred.2:
	movzbl	100(%rax,%rdx,1), %ecx
	movb	%cl, (%rbp,%rdx,1)
	addq	$1, %rdx
	jne	.Lhundred.2
	leaq	-100(%rbp), %rax
	movq	%rax, -112(%rbp)
	movq	-112(%rbp), %rdi
	call	use@PLT
.Lhundred.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	hundred, .-hundred
	.data
.Ls0:
	.string	"abc"
.Ls1:
	.string	"a longer message"
.Ls2:
	.string	"abc"
.Ls3:
	.string	"abc"
.Ls4:
	.string	"012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678"
	.section	.note.GNU-stack,"",@progbits
