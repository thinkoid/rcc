	.file	"header-itoa.c"
	.text
	.globl	reverse
	.type	reverse, @function
reverse:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movl	$0, -16(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -32(%rbp)
	movq	-32(%rbp), %rdi
	call	strlen@PLT
	subq	$1, %rax
	movl	%eax, -20(%rbp)
	.p2align 5
.Lreverse.2:
	movl	-16(%rbp), %eax
	cmpl	-20(%rbp), %eax
	jge	.Lreverse.4
	movq	-8(%rbp), %rax
	movslq	-16(%rbp), %rcx
	movsbl	(%rax,%rcx,1), %eax
	movl	%eax, -12(%rbp)
	movq	-8(%rbp), %rax
	movslq	-16(%rbp), %rcx
	movq	-8(%rbp), %rdx
	movslq	-20(%rbp), %rsi
	movsbl	(%rdx,%rsi,1), %edx
	movb	%dl, (%rax,%rcx,1)
	movq	-8(%rbp), %rax
	movslq	-20(%rbp), %rcx
	movl	-12(%rbp), %edx
	movb	%dl, (%rax,%rcx,1)
	addl	$1, -16(%rbp)
	subl	$1, -20(%rbp)
	jmp	.Lreverse.2
.Lreverse.4:
.Lreverse.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	reverse, .-reverse
	.globl	itoa
	.type	itoa, @function
itoa:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -24(%rbp)
	cmpl	$0, %eax
	jge	.Litoa.2
	movl	-4(%rbp), %eax
	negl	%eax
	movl	%eax, -4(%rbp)
.Litoa.2:
	movl	$0, -20(%rbp)
	.p2align 5
.Litoa.3:
	movq	-16(%rbp), %rcx
	movl	-20(%rbp), %esi
	movl	%esi, %edi
	addl	$1, %esi
	movl	%esi, -20(%rbp)
	movslq	%edi, %rdi
	movl	-4(%rbp), %eax
	movl	$10, %esi
	cltd
	idivl	%esi
	addl	$48, %edx
	movb	%dl, (%rcx,%rdi,1)
	movl	$10, %ecx
	movl	-4(%rbp), %eax
	cltd
	idivl	%ecx
	movl	%eax, -4(%rbp)
	cmpl	$0, %eax
	jg	.Litoa.3
	cmpl	$0, -24(%rbp)
	jge	.Litoa.6
	movq	-16(%rbp), %rax
	movl	-20(%rbp), %ecx
	movl	%ecx, %edx
	addl	$1, %ecx
	movl	%ecx, -20(%rbp)
	movslq	%edx, %rdx
	movb	$45, (%rax,%rdx,1)
.Litoa.6:
	movq	-16(%rbp), %rax
	movslq	-20(%rbp), %rcx
	movb	$0, (%rax,%rcx,1)
	movq	-16(%rbp), %rax
	movq	%rax, -32(%rbp)
	movq	-32(%rbp), %rdi
	call	reverse@PLT
.Litoa.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	itoa, .-itoa
	.globl	trim
	.type	trim, @function
trim:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -24(%rbp)
	movq	-24(%rbp), %rdi
	call	strlen@PLT
	subq	$1, %rax
	movl	%eax, -12(%rbp)
	.p2align 5
.Ltrim.2:
	cmpl	$0, -12(%rbp)
	jl	.Ltrim.4
	movq	-8(%rbp), %rax
	movslq	-12(%rbp), %rcx
	movsbl	(%rax,%rcx,1), %eax
	cmpl	$32, %eax
	je	.Ltrim.5
	movq	-8(%rbp), %rax
	movslq	-12(%rbp), %rcx
	movsbl	(%rax,%rcx,1), %eax
	cmpl	$9, %eax
	je	.Ltrim.5
	movq	-8(%rbp), %rax
	movslq	-12(%rbp), %rcx
	movsbl	(%rax,%rcx,1), %eax
	cmpl	$10, %eax
	jne	.Ltrim.4
.Ltrim.5:
	subl	$1, -12(%rbp)
	jmp	.Ltrim.2
.Ltrim.4:
	movq	-8(%rbp), %rax
	movl	-12(%rbp), %ecx
	addl	$1, %ecx
	movslq	%ecx, %rcx
	movb	$0, (%rax,%rcx,1)
	movl	-12(%rbp), %eax
	jmp	.Ltrim.1
.Ltrim.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	trim, .-trim
	.globl	count_digits
	.type	count_digits, @function
count_digits:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	$0, -16(%rbp)
	movl	$0, -12(%rbp)
	.p2align 5
.Lcount_digits.2:
	movq	-8(%rbp), %rax
	movslq	-12(%rbp), %rcx
	movsbl	(%rax,%rcx,1), %eax
	cmpl	$0, %eax
	je	.Lcount_digits.4
	movq	-8(%rbp), %rax
	movslq	-12(%rbp), %rcx
	movsbl	(%rax,%rcx,1), %eax
	cmpl	$48, %eax
	jl	.Lcount_digits.3
	movq	-8(%rbp), %rax
	movslq	-12(%rbp), %rcx
	movsbl	(%rax,%rcx,1), %eax
	cmpl	$57, %eax
	jg	.Lcount_digits.3
	addl	$1, -16(%rbp)
.Lcount_digits.3:
	addl	$1, -12(%rbp)
	jmp	.Lcount_digits.2
.Lcount_digits.4:
	movl	-16(%rbp), %eax
	jmp	.Lcount_digits.1
.Lcount_digits.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	count_digits, .-count_digits
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$400, %rsp
	movl	$0, -52(%rbp)
	leaq	-16(%rbp), %rax
	movq	%rax, -64(%rbp)
	movl	-52(%rbp), %edi
	movq	-64(%rbp), %rsi
	call	itoa@PLT
	leaq	-16(%rbp), %rax
	movq	%rax, -72(%rbp)
	leaq	.Ls0(%rip), %rax
	movq	%rax, -80(%rbp)
	movq	-72(%rbp), %rdi
	movq	-80(%rbp), %rsi
	call	strcmp@PLT
	cmpl	$0, %eax
	je	.Lmain.2
	movl	$1, %eax
	jmp	.Lmain.1
.Lmain.2:
	movl	$7, -84(%rbp)
	leaq	-16(%rbp), %rax
	movq	%rax, -96(%rbp)
	movl	-84(%rbp), %edi
	movq	-96(%rbp), %rsi
	call	itoa@PLT
	leaq	-16(%rbp), %rax
	movq	%rax, -104(%rbp)
	leaq	.Ls1(%rip), %rax
	movq	%rax, -112(%rbp)
	movq	-104(%rbp), %rdi
	movq	-112(%rbp), %rsi
	call	strcmp@PLT
	cmpl	$0, %eax
	je	.Lmain.3
	movl	$2, %eax
	jmp	.Lmain.1
.Lmain.3:
	movl	$-7, -116(%rbp)
	leaq	-16(%rbp), %rax
	movq	%rax, -128(%rbp)
	movl	-116(%rbp), %edi
	movq	-128(%rbp), %rsi
	call	itoa@PLT
	leaq	-16(%rbp), %rax
	movq	%rax, -136(%rbp)
	leaq	.Ls2(%rip), %rax
	movq	%rax, -144(%rbp)
	movq	-136(%rbp), %rdi
	movq	-144(%rbp), %rsi
	call	strcmp@PLT
	cmpl	$0, %eax
	je	.Lmain.4
	movl	$3, %eax
	jmp	.Lmain.1
.Lmain.4:
	movl	$1234, -148(%rbp)
	leaq	-16(%rbp), %rax
	movq	%rax, -160(%rbp)
	movl	-148(%rbp), %edi
	movq	-160(%rbp), %rsi
	call	itoa@PLT
	leaq	-16(%rbp), %rax
	movq	%rax, -168(%rbp)
	leaq	.Ls3(%rip), %rax
	movq	%rax, -176(%rbp)
	movq	-168(%rbp), %rdi
	movq	-176(%rbp), %rsi
	call	strcmp@PLT
	cmpl	$0, %eax
	je	.Lmain.5
	movl	$4, %eax
	jmp	.Lmain.1
.Lmain.5:
	movl	$-98765, -180(%rbp)
	leaq	-32(%rbp), %rax
	movq	%rax, -192(%rbp)
	movl	-180(%rbp), %edi
	movq	-192(%rbp), %rsi
	call	itoa@PLT
	leaq	-32(%rbp), %rax
	movq	%rax, -200(%rbp)
	leaq	.Ls4(%rip), %rax
	movq	%rax, -208(%rbp)
	movq	-200(%rbp), %rdi
	movq	-208(%rbp), %rsi
	call	strcmp@PLT
	cmpl	$0, %eax
	je	.Lmain.6
	movl	$5, %eax
	jmp	.Lmain.1
.Lmain.6:
	movl	$2147483647, -212(%rbp)
	leaq	-48(%rbp), %rax
	movq	%rax, -224(%rbp)
	movl	-212(%rbp), %edi
	movq	-224(%rbp), %rsi
	call	itoa@PLT
	leaq	-48(%rbp), %rax
	movq	%rax, -232(%rbp)
	leaq	.Ls5(%rip), %rax
	movq	%rax, -240(%rbp)
	movq	-232(%rbp), %rdi
	movq	-240(%rbp), %rsi
	call	strcmp@PLT
	cmpl	$0, %eax
	je	.Lmain.7
	movl	$6, %eax
	jmp	.Lmain.1
.Lmain.7:
	leaq	abc(%rip), %rax
	movq	%rax, -248(%rbp)
	movq	-248(%rbp), %rdi
	call	reverse@PLT
	leaq	abc(%rip), %rax
	movq	%rax, -256(%rbp)
	leaq	.Ls6(%rip), %rax
	movq	%rax, -264(%rbp)
	movq	-256(%rbp), %rdi
	movq	-264(%rbp), %rsi
	call	strcmp@PLT
	cmpl	$0, %eax
	je	.Lmain.8
	movl	$7, %eax
	jmp	.Lmain.1
.Lmain.8:
	leaq	empty(%rip), %rax
	movq	%rax, -272(%rbp)
	movq	-272(%rbp), %rdi
	call	reverse@PLT
	leaq	empty(%rip), %rax
	movq	%rax, -280(%rbp)
	leaq	.Ls7(%rip), %rax
	movq	%rax, -288(%rbp)
	movq	-280(%rbp), %rdi
	movq	-288(%rbp), %rsi
	call	strcmp@PLT
	cmpl	$0, %eax
	je	.Lmain.9
	movl	$8, %eax
	jmp	.Lmain.1
.Lmain.9:
	leaq	word(%rip), %rax
	movq	%rax, -296(%rbp)
	movq	-296(%rbp), %rdi
	call	trim@PLT
	cmpl	$4, %eax
	jne	.Lmain.11
	leaq	word(%rip), %rax
	movq	%rax, -304(%rbp)
	leaq	.Ls8(%rip), %rax
	movq	%rax, -312(%rbp)
	movq	-304(%rbp), %rdi
	movq	-312(%rbp), %rsi
	call	strcmp@PLT
	cmpl	$0, %eax
	je	.Lmain.10
.Lmain.11:
	movl	$9, %eax
	jmp	.Lmain.1
.Lmain.10:
	leaq	blanks(%rip), %rax
	movq	%rax, -320(%rbp)
	movq	-320(%rbp), %rdi
	call	trim@PLT
	cmpl	$-1, %eax
	jne	.Lmain.13
	leaq	blanks(%rip), %rax
	movq	%rax, -328(%rbp)
	leaq	.Ls9(%rip), %rax
	movq	%rax, -336(%rbp)
	movq	-328(%rbp), %rdi
	movq	-336(%rbp), %rsi
	call	strcmp@PLT
	cmpl	$0, %eax
	je	.Lmain.12
.Lmain.13:
	movl	$10, %eax
	jmp	.Lmain.1
.Lmain.12:
	leaq	x(%rip), %rax
	movq	%rax, -344(%rbp)
	movq	-344(%rbp), %rdi
	call	trim@PLT
	cmpl	$0, %eax
	jne	.Lmain.15
	leaq	x(%rip), %rax
	movq	%rax, -352(%rbp)
	leaq	.Ls10(%rip), %rax
	movq	%rax, -360(%rbp)
	movq	-352(%rbp), %rdi
	movq	-360(%rbp), %rsi
	call	strcmp@PLT
	cmpl	$0, %eax
	je	.Lmain.14
.Lmain.15:
	movl	$11, %eax
	jmp	.Lmain.1
.Lmain.14:
	leaq	digits(%rip), %rax
	movq	%rax, -368(%rbp)
	movq	-368(%rbp), %rdi
	call	count_digits@PLT
	cmpl	$6, %eax
	je	.Lmain.16
	movl	$12, %eax
	jmp	.Lmain.1
.Lmain.16:
	leaq	.Ls11(%rip), %rax
	movq	%rax, -376(%rbp)
	leaq	-32(%rbp), %rax
	movq	%rax, -384(%rbp)
	leaq	-48(%rbp), %rax
	movq	%rax, -392(%rbp)
	leaq	word(%rip), %rax
	movq	%rax, -400(%rbp)
	movq	-376(%rbp), %rdi
	movq	-384(%rbp), %rsi
	movq	-392(%rbp), %rdx
	movq	-400(%rbp), %rcx
	movl	$0, %eax
	call	printf@PLT
	movl	$100, %eax
	jmp	.Lmain.1
.Lmain.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	main, .-main
	.data
.Ls0:
	.string	"0"
.Ls1:
	.string	"7"
.Ls2:
	.string	"-7"
.Ls3:
	.string	"1234"
.Ls4:
	.string	"-98765"
.Ls5:
	.string	"2147483647"
.Ls6:
	.string	"cba"
.Ls7:
	.string	""
.Ls8:
	.string	"hello"
.Ls9:
	.string	""
.Ls10:
	.string	"x"
.Ls11:
	.string	"%s %s [%s]\n"
	.data
	.type	word, @object
	.size	word, 9
word:
	.string	"hello \t\n"
	.type	blanks, @object
	.size	blanks, 4
blanks:
	.string	"   "
	.type	x, @object
	.size	x, 2
x:
	.string	"x"
	.type	abc, @object
	.size	abc, 4
abc:
	.string	"abc"
	.type	empty, @object
	.size	empty, 1
empty:
	.string	""
	.type	digits, @object
	.size	digits, 10
digits:
	.string	"a1b22c333"
	.section	.note.GNU-stack,"",@progbits
