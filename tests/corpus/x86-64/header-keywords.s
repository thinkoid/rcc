	.file	"header-keywords.c"
	.text
	.type	binsearch, @function
binsearch:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$64, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movl	%edx, -20(%rbp)
	movl	$0, -24(%rbp)
	movl	-20(%rbp), %eax
	subl	$1, %eax
	movl	%eax, -28(%rbp)
	.p2align 5
.Lbinsearch.2:
	movl	-24(%rbp), %eax
	cmpl	-28(%rbp), %eax
	jg	.Lbinsearch.3
	movl	-24(%rbp), %eax
	addl	-28(%rbp), %eax
	movl	$2, %ecx
	cltd
	idivl	%ecx
	movl	%eax, -32(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -48(%rbp)
	movq	-16(%rbp), %rax
	movslq	-32(%rbp), %rcx
	imulq	$16, %rcx, %rcx
	movq	(%rax,%rcx,1), %rax
	movq	%rax, -56(%rbp)
	movq	-48(%rbp), %rdi
	movq	-56(%rbp), %rsi
	call	strcmp@PLT
	movl	%eax, -36(%rbp)
	cmpl	$0, -36(%rbp)
	jge	.Lbinsearch.4
	movl	-32(%rbp), %eax
	subl	$1, %eax
	movl	%eax, -28(%rbp)
	jmp	.Lbinsearch.5
.Lbinsearch.4:
	cmpl	$0, -36(%rbp)
	jle	.Lbinsearch.6
	movl	-32(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -24(%rbp)
	jmp	.Lbinsearch.7
.Lbinsearch.6:
	movl	-32(%rbp), %eax
	jmp	.Lbinsearch.1
.Lbinsearch.7:
.Lbinsearch.5:
	jmp	.Lbinsearch.2
.Lbinsearch.3:
	movl	$-1, %eax
	jmp	.Lbinsearch.1
.Lbinsearch.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	binsearch, .-binsearch
	.type	getword, @function
getword:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -24(%rbp)
	.p2align 5
.Lgetword.2:
	call	__ctype_b_loc@PLT
	movq	(%rax), %rax
	movq	next(%rip), %rcx
	movzbl	(%rcx), %ecx
	movzwl	(%rax,%rcx,2), %eax
	testl	$8192, %eax
	je	.Lgetword.3
	addq	$1, next(%rip)
	jmp	.Lgetword.2
.Lgetword.3:
	movq	next(%rip), %rax
	movsbl	(%rax), %eax
	cmpl	$0, %eax
	jne	.Lgetword.4
	movl	$0, %eax
	jmp	.Lgetword.1
.Lgetword.4:
	movq	-24(%rbp), %rax
	movq	%rax, %rcx
	addq	$1, %rax
	movq	%rax, -24(%rbp)
	movq	next(%rip), %rax
	movsbl	(%rax), %eax
	movb	%al, (%rcx)
	call	__ctype_b_loc@PLT
	movq	(%rax), %rax
	movq	next(%rip), %rcx
	movq	%rcx, %rdx
	addq	$1, %rcx
	movq	%rcx, next(%rip)
	movzbl	(%rdx), %ecx
	movzwl	(%rax,%rcx,2), %eax
	testl	$1024, %eax
	jne	.Lgetword.5
	movq	-24(%rbp), %rax
	movb	$0, (%rax)
	movq	-8(%rbp), %rax
	movsbl	(%rax), %eax
	jmp	.Lgetword.1
.Lgetword.5:
	.p2align 5
.Lgetword.6:
	movl	-12(%rbp), %eax
	subl	$1, %eax
	movl	%eax, -12(%rbp)
	cmpl	$0, %eax
	jle	.Lgetword.8
	call	__ctype_b_loc@PLT
	movq	(%rax), %rax
	movq	next(%rip), %rcx
	movzbl	(%rcx), %ecx
	movzwl	(%rax,%rcx,2), %eax
	testl	$8, %eax
	je	.Lgetword.8
	movq	-24(%rbp), %rax
	movq	%rax, %rcx
	addq	$1, %rax
	movq	%rax, -24(%rbp)
	movq	next(%rip), %rax
	movsbl	(%rax), %eax
	movb	%al, (%rcx)
	addq	$1, next(%rip)
	jmp	.Lgetword.6
.Lgetword.8:
	movq	-24(%rbp), %rax
	movb	$0, (%rax)
	movq	-8(%rbp), %rax
	movsbl	(%rax), %eax
	jmp	.Lgetword.1
.Lgetword.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	getword, .-getword
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$160, %rsp
	movl	$0, %eax
	cmpl	$0, %eax
	je	.Lmain.2
	movl	$201, %eax
	jmp	.Lmain.1
.Lmain.2:
	cmpl	$0, keytab+504(%rip)
	jne	.Lmain.4
	movq	keytab+496(%rip), %rax
	movq	%rax, -48(%rbp)
	leaq	.Ls0(%rip), %rax
	movq	%rax, -56(%rbp)
	movq	-48(%rbp), %rdi
	movq	-56(%rbp), %rsi
	call	strcmp@PLT
	cmpl	$0, %eax
	je	.Lmain.3
.Lmain.4:
	movl	$202, %eax
	jmp	.Lmain.1
.Lmain.3:
	.p2align 5
.Lmain.5:
	leaq	-32(%rbp), %rax
	movq	%rax, -64(%rbp)
	movl	$32, -68(%rbp)
	movq	-64(%rbp), %rdi
	movl	-68(%rbp), %esi
	call	getword@PLT
	cmpl	$0, %eax
	je	.Lmain.6
	call	__ctype_b_loc@PLT
	movq	(%rax), %rax
	movzbl	-32(%rbp), %ecx
	movzwl	(%rax,%rcx,2), %eax
	testl	$1024, %eax
	je	.Lmain.7
	leaq	-32(%rbp), %rax
	movq	%rax, -80(%rbp)
	leaq	keytab(%rip), %rax
	movq	%rax, -88(%rbp)
	movl	$32, -92(%rbp)
	movq	-80(%rbp), %rdi
	movq	-88(%rbp), %rsi
	movl	-92(%rbp), %edx
	call	binsearch@PLT
	movl	%eax, -36(%rbp)
	cmpl	$0, %eax
	jl	.Lmain.8
	leaq	keytab(%rip), %rax
	movslq	-36(%rbp), %rcx
	imulq	$16, %rcx, %rcx
	addl	$1, 8(%rax,%rcx,1)
.Lmain.8:
.Lmain.7:
	jmp	.Lmain.5
.Lmain.6:
	movl	$0, -96(%rbp)
	movl	$0, -100(%rbp)
	movl	$0, -36(%rbp)
	.p2align 5
.Lmain.9:
	cmpl	$32, -36(%rbp)
	jge	.Lmain.11
	leaq	keytab(%rip), %rax
	movslq	-36(%rbp), %rcx
	imulq	$16, %rcx, %rcx
	cmpl	$0, 8(%rax,%rcx,1)
	jle	.Lmain.12
	leaq	.Ls1(%rip), %rax
	movq	%rax, -112(%rbp)
	leaq	keytab(%rip), %rax
	movslq	-36(%rbp), %rcx
	imulq	$16, %rcx, %rcx
	movl	8(%rax,%rcx,1), %eax
	movl	%eax, -116(%rbp)
	leaq	keytab(%rip), %rax
	movslq	-36(%rbp), %rcx
	imulq	$16, %rcx, %rcx
	movq	(%rax,%rcx,1), %rax
	movq	%rax, -128(%rbp)
	movq	-112(%rbp), %rdi
	movl	-116(%rbp), %esi
	movq	-128(%rbp), %rdx
	movl	$0, %eax
	call	printf@PLT
	leaq	keytab(%rip), %rax
	movslq	-36(%rbp), %rcx
	imulq	$16, %rcx, %rcx
	movl	8(%rax,%rcx,1), %eax
	addl	%eax, -96(%rbp)
	addl	$1, -100(%rbp)
.Lmain.12:
	addl	$1, -36(%rbp)
	jmp	.Lmain.9
.Lmain.11:
	leaq	.Ls2(%rip), %rax
	movq	%rax, -136(%rbp)
	leaq	keytab(%rip), %rax
	movq	%rax, -144(%rbp)
	movl	$32, -148(%rbp)
	movq	-136(%rbp), %rdi
	movq	-144(%rbp), %rsi
	movl	-148(%rbp), %edx
	call	binsearch@PLT
	cmpl	$-1, %eax
	je	.Lmain.13
	movl	$203, %eax
	jmp	.Lmain.1
.Lmain.13:
	movl	-96(%rbp), %eax
	addl	-100(%rbp), %eax
	jmp	.Lmain.1
.Lmain.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	main, .-main
	.data
.Ls0:
	.string	"while"
.Ls1:
	.string	"%4d %s\n"
.Ls2:
	.string	"main"
.Ls3:
	.string	"auto"
.Ls4:
	.string	"break"
.Ls5:
	.string	"case"
.Ls6:
	.string	"char"
.Ls7:
	.string	"const"
.Ls8:
	.string	"continue"
.Ls9:
	.string	"default"
.Ls10:
	.string	"do"
.Ls11:
	.string	"double"
.Ls12:
	.string	"else"
.Ls13:
	.string	"enum"
.Ls14:
	.string	"extern"
.Ls15:
	.string	"float"
.Ls16:
	.string	"for"
.Ls17:
	.string	"goto"
.Ls18:
	.string	"if"
.Ls19:
	.string	"int"
.Ls20:
	.string	"long"
.Ls21:
	.string	"register"
.Ls22:
	.string	"return"
.Ls23:
	.string	"short"
.Ls24:
	.string	"signed"
.Ls25:
	.string	"sizeof"
.Ls26:
	.string	"static"
.Ls27:
	.string	"struct"
.Ls28:
	.string	"switch"
.Ls29:
	.string	"typedef"
.Ls30:
	.string	"union"
.Ls31:
	.string	"unsigned"
.Ls32:
	.string	"void"
.Ls33:
	.string	"volatile"
.Ls34:
	.string	"while"
	.data
	.type	keytab, @object
	.size	keytab, 512
	.p2align 4
keytab:
	.quad	.Ls3
	.long	0
	.zero	4
	.quad	.Ls4
	.long	0
	.zero	4
	.quad	.Ls5
	.long	0
	.zero	4
	.quad	.Ls6
	.long	0
	.zero	4
	.quad	.Ls7
	.long	0
	.zero	4
	.quad	.Ls8
	.long	0
	.zero	4
	.quad	.Ls9
	.long	0
	.zero	4
	.quad	.Ls10
	.long	0
	.zero	4
	.quad	.Ls11
	.long	0
	.zero	4
	.quad	.Ls12
	.long	0
	.zero	4
	.quad	.Ls13
	.long	0
	.zero	4
	.quad	.Ls14
	.long	0
	.zero	4
	.quad	.Ls15
	.long	0
	.zero	4
	.quad	.Ls16
	.long	0
	.zero	4
	.quad	.Ls17
	.long	0
	.zero	4
	.quad	.Ls18
	.long	0
	.zero	4
	.quad	.Ls19
	.long	0
	.zero	4
	.quad	.Ls20
	.long	0
	.zero	4
	.quad	.Ls21
	.long	0
	.zero	4
	.quad	.Ls22
	.long	0
	.zero	4
	.quad	.Ls23
	.long	0
	.zero	4
	.quad	.Ls24
	.long	0
	.zero	4
	.quad	.Ls25
	.long	0
	.zero	4
	.quad	.Ls26
	.long	0
	.zero	4
	.quad	.Ls27
	.long	0
	.zero	4
	.quad	.Ls28
	.long	0
	.zero	4
	.quad	.Ls29
	.long	0
	.zero	4
	.quad	.Ls30
	.long	0
	.zero	4
	.quad	.Ls31
	.long	0
	.zero	4
	.quad	.Ls32
	.long	0
	.zero	4
	.quad	.Ls33
	.long	0
	.zero	4
	.quad	.Ls34
	.zero	8
	.type	text, @object
	.size	text, 275
	.p2align 4
text:
	.string	"static int count(const char *s)\n{\n        int n = 0;\n        while (*s != '\\0')\n                if (*s++ == ' ')\n                        n++;\n                else\n                        continue;\n        for (;;)\n                break;\n        return (int) sizeof n + n;\n}\n"
	.type	next, @object
	.size	next, 8
	.p2align 3
next:
	.quad	text
	.section	.note.GNU-stack,"",@progbits
