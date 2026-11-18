	.file	"header-lookup.c"
	.text
	.type	hash, @function
hash:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	$0, -12(%rbp)
	.p2align 5
.Lhash.2:
	movq	-8(%rbp), %rax
	movsbl	(%rax), %eax
	cmpl	$0, %eax
	je	.Lhash.4
	movq	-8(%rbp), %rax
	movsbl	(%rax), %eax
	imull	$31, -12(%rbp), %ecx
	addl	%ecx, %eax
	movl	%eax, -12(%rbp)
	addq	$1, -8(%rbp)
	jmp	.Lhash.2
.Lhash.4:
	movl	-12(%rbp), %eax
	movl	$101, %ecx
	xorl	%edx, %edx
	divl	%ecx
	movl	%edx, %eax
	jmp	.Lhash.1
.Lhash.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	hash, .-hash
	.type	lookup, @function
lookup:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$48, %rsp
	movq	%rdi, -8(%rbp)
	leaq	hashtab(%rip), %rax
	movq	%rax, -24(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -32(%rbp)
	movq	-32(%rbp), %rdi
	call	hash@PLT
	movl	%eax, %eax
	movq	-24(%rbp), %rcx
	movq	(%rcx,%rax,8), %rax
	movq	%rax, -16(%rbp)
	.p2align 5
.Llookup.2:
	cmpq	$0, -16(%rbp)
	je	.Llookup.4
	movq	-8(%rbp), %rax
	movq	%rax, -40(%rbp)
	movq	-16(%rbp), %rax
	movq	8(%rax), %rax
	movq	%rax, -48(%rbp)
	movq	-40(%rbp), %rdi
	movq	-48(%rbp), %rsi
	call	strcmp@PLT
	cmpl	$0, %eax
	jne	.Llookup.5
	movq	-16(%rbp), %rax
	jmp	.Llookup.1
.Llookup.5:
	movq	-16(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, -16(%rbp)
	jmp	.Llookup.2
.Llookup.4:
	movq	$0, %rax
	jmp	.Llookup.1
.Llookup.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	lookup, .-lookup
	.type	install, @function
install:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$96, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -32(%rbp)
	movq	-32(%rbp), %rdi
	call	lookup@PLT
	movq	%rax, -24(%rbp)
	cmpq	$0, -24(%rbp)
	jne	.Linstall.2
	movq	$24, -40(%rbp)
	movq	-40(%rbp), %rdi
	call	malloc@PLT
	movq	%rax, -24(%rbp)
	cmpq	$0, -24(%rbp)
	je	.Linstall.4
	movq	-24(%rbp), %rax
	movq	%rax, -48(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -56(%rbp)
	movq	-56(%rbp), %rdi
	call	strdup@PLT
	movq	-48(%rbp), %rcx
	movq	%rax, 8(%rcx)
	cmpq	$0, %rax
	jne	.Linstall.3
.Linstall.4:
	movq	$0, %rax
	jmp	.Linstall.1
.Linstall.3:
	movq	-8(%rbp), %rax
	movq	%rax, -72(%rbp)
	movq	-72(%rbp), %rdi
	call	hash@PLT
	movl	%eax, -60(%rbp)
	movq	-24(%rbp), %rax
	leaq	hashtab(%rip), %rcx
	movl	-60(%rbp), %edx
	movq	(%rcx,%rdx,8), %rcx
	movq	%rcx, (%rax)
	leaq	hashtab(%rip), %rax
	movl	-60(%rbp), %ecx
	movq	-24(%rbp), %rdx
	movq	%rdx, (%rax,%rcx,8)
	jmp	.Linstall.5
.Linstall.2:
	movq	-24(%rbp), %rax
	movq	16(%rax), %rax
	movq	%rax, -80(%rbp)
	movq	-80(%rbp), %rdi
	call	free@PLT
.Linstall.5:
	movq	-24(%rbp), %rax
	movq	%rax, -88(%rbp)
	movq	-16(%rbp), %rax
	movq	%rax, -96(%rbp)
	movq	-96(%rbp), %rdi
	call	strdup@PLT
	movq	-88(%rbp), %rcx
	movq	%rax, 16(%rcx)
	cmpq	$0, %rax
	jne	.Linstall.6
	movq	$0, %rax
	jmp	.Linstall.1
.Linstall.6:
	movq	-24(%rbp), %rax
	jmp	.Linstall.1
.Linstall.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	install, .-install
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$224, %rsp
	leaq	.Ls0(%rip), %rax
	movq	%rax, -8(%rbp)
	leaq	.Ls1(%rip), %rax
	movq	%rax, -16(%rbp)
	movq	-8(%rbp), %rdi
	movq	-16(%rbp), %rsi
	call	install@PLT
	leaq	.Ls2(%rip), %rax
	movq	%rax, -24(%rbp)
	leaq	.Ls3(%rip), %rax
	movq	%rax, -32(%rbp)
	movq	-24(%rbp), %rdi
	movq	-32(%rbp), %rsi
	call	install@PLT
	leaq	.Ls4(%rip), %rax
	movq	%rax, -40(%rbp)
	leaq	.Ls5(%rip), %rax
	movq	%rax, -48(%rbp)
	movq	-40(%rbp), %rdi
	movq	-48(%rbp), %rsi
	call	install@PLT
	leaq	.Ls6(%rip), %rax
	movq	%rax, -56(%rbp)
	leaq	.Ls7(%rip), %rax
	movq	%rax, -64(%rbp)
	movq	-56(%rbp), %rdi
	movq	-64(%rbp), %rsi
	call	install@PLT
	leaq	.Ls8(%rip), %rax
	movq	%rax, -72(%rbp)
	leaq	.Ls9(%rip), %rax
	movq	%rax, -80(%rbp)
	movq	-72(%rbp), %rdi
	movq	-80(%rbp), %rsi
	call	install@PLT
	leaq	.Ls10(%rip), %rax
	movq	%rax, -88(%rbp)
	leaq	.Ls11(%rip), %rax
	movq	%rax, -96(%rbp)
	movq	-88(%rbp), %rdi
	movq	-96(%rbp), %rsi
	call	install@PLT
	leaq	.Ls12(%rip), %rax
	movq	%rax, -104(%rbp)
	leaq	.Ls13(%rip), %rax
	movq	%rax, -112(%rbp)
	movq	-104(%rbp), %rdi
	movq	-112(%rbp), %rsi
	call	install@PLT
	leaq	.Ls14(%rip), %rax
	movq	%rax, -120(%rbp)
	leaq	.Ls15(%rip), %rax
	movq	%rax, -128(%rbp)
	movq	-120(%rbp), %rdi
	movq	-128(%rbp), %rsi
	call	install@PLT
	movl	$0, -132(%rbp)
	movl	$0, -136(%rbp)
	.p2align 5
.Lmain.2:
	cmpl	$7, -136(%rbp)
	jge	.Lmain.4
	leaq	names(%rip), %rax
	movslq	-136(%rbp), %rcx
	movq	(%rax,%rcx,8), %rax
	movq	%rax, -152(%rbp)
	movq	-152(%rbp), %rdi
	call	lookup@PLT
	movq	%rax, -144(%rbp)
	cmpq	$0, -144(%rbp)
	jne	.Lmain.5
	movl	$101, %eax
	jmp	.Lmain.1
.Lmain.5:
	leaq	.Ls16(%rip), %rax
	movq	%rax, -160(%rbp)
	movq	-144(%rbp), %rax
	movq	8(%rax), %rax
	movq	%rax, -168(%rbp)
	movq	-144(%rbp), %rax
	movq	16(%rax), %rax
	movq	%rax, -176(%rbp)
	movq	-160(%rbp), %rdi
	movq	-168(%rbp), %rsi
	movq	-176(%rbp), %rdx
	movl	$0, %eax
	call	printf@PLT
	movq	-144(%rbp), %rax
	movq	16(%rax), %rax
	movq	%rax, -184(%rbp)
	movq	-184(%rbp), %rdi
	call	strlen@PLT
	addl	%eax, -132(%rbp)
	addl	$1, -136(%rbp)
	jmp	.Lmain.2
.Lmain.4:
	leaq	.Ls17(%rip), %rax
	movq	%rax, -192(%rbp)
	movq	-192(%rbp), %rdi
	call	lookup@PLT
	cmpq	$0, %rax
	je	.Lmain.6
	movl	$102, %eax
	jmp	.Lmain.1
.Lmain.6:
	leaq	.Ls18(%rip), %rax
	movq	%rax, -208(%rbp)
	movq	-208(%rbp), %rdi
	call	lookup@PLT
	movq	16(%rax), %rax
	movq	%rax, -200(%rbp)
	leaq	.Ls19(%rip), %rax
	movq	%rax, -216(%rbp)
	movq	-200(%rbp), %rdi
	movq	-216(%rbp), %rsi
	call	strcmp@PLT
	cmpl	$0, %eax
	je	.Lmain.7
	movl	$103, %eax
	jmp	.Lmain.1
.Lmain.7:
	movl	$0, -220(%rbp)
	movl	$0, -224(%rbp)
	.p2align 5
.Lmain.8:
	cmpl	$101, -224(%rbp)
	jge	.Lmain.10
	leaq	hashtab(%rip), %rax
	movslq	-224(%rbp), %rcx
	cmpq	$0, (%rax,%rcx,8)
	je	.Lmain.11
	leaq	hashtab(%rip), %rax
	movslq	-224(%rbp), %rcx
	movq	(%rax,%rcx,8), %rax
	cmpq	$0, (%rax)
	je	.Lmain.11
	addl	$1, -220(%rbp)
.Lmain.11:
	addl	$1, -224(%rbp)
	jmp	.Lmain.8
.Lmain.10:
	movl	-132(%rbp), %eax
	imull	$10, -220(%rbp), %ecx
	addl	%ecx, %eax
	jmp	.Lmain.1
.Lmain.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	main, .-main
	.data
.Ls0:
	.string	"IN"
.Ls1:
	.string	"1"
.Ls2:
	.string	"OUT"
.Ls3:
	.string	"0"
.Ls4:
	.string	"MAX"
.Ls5:
	.string	"100"
.Ls6:
	.string	"EOF"
.Ls7:
	.string	"(-1)"
.Ls8:
	.string	"TAB"
.Ls9:
	.string	"\\t"
.Ls10:
	.string	"YES"
.Ls11:
	.string	"always"
.Ls12:
	.string	"NO"
.Ls13:
	.string	"nil"
.Ls14:
	.string	"IN"
.Ls15:
	.string	"one"
.Ls16:
	.string	"%s %s\n"
.Ls17:
	.string	"OTHER"
.Ls18:
	.string	"IN"
.Ls19:
	.string	"one"
.Ls20:
	.string	"IN"
.Ls21:
	.string	"OUT"
.Ls22:
	.string	"MAX"
.Ls23:
	.string	"EOF"
.Ls24:
	.string	"TAB"
.Ls25:
	.string	"YES"
.Ls26:
	.string	"NO"
	.data
	.type	names, @object
	.size	names, 56
	.p2align 4
names:
	.quad	.Ls20
	.quad	.Ls21
	.quad	.Ls22
	.quad	.Ls23
	.quad	.Ls24
	.quad	.Ls25
	.quad	.Ls26
	.bss
	.type	hashtab, @object
	.size	hashtab, 808
	.p2align 4
hashtab:
	.zero	808
	.section	.note.GNU-stack,"",@progbits
