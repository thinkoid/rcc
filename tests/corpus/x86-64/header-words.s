	.file	"header-words.c"
	.text
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
	leaq	text(%rip), %rax
	movslq	at(%rip), %rcx
	movsbl	(%rax,%rcx,1), %eax
	cmpl	$0, %eax
	je	.Lgetword.3
	call	__ctype_b_loc@PLT
	movq	(%rax), %rax
	leaq	text(%rip), %rcx
	movslq	at(%rip), %rdx
	movzbl	(%rcx,%rdx,1), %ecx
	movzwl	(%rax,%rcx,2), %eax
	testl	$1024, %eax
	jne	.Lgetword.3
	addl	$1, at(%rip)
	jmp	.Lgetword.2
.Lgetword.3:
	leaq	text(%rip), %rax
	movslq	at(%rip), %rcx
	movsbl	(%rax,%rcx,1), %eax
	cmpl	$0, %eax
	jne	.Lgetword.4
	movl	$0, %eax
	jmp	.Lgetword.1
.Lgetword.4:
	.p2align 5
.Lgetword.5:
	call	__ctype_b_loc@PLT
	movq	(%rax), %rax
	leaq	text(%rip), %rcx
	movslq	at(%rip), %rdx
	movzbl	(%rcx,%rdx,1), %ecx
	movzwl	(%rax,%rcx,2), %eax
	testl	$1024, %eax
	je	.Lgetword.6
	movl	-12(%rbp), %eax
	subl	$1, %eax
	movl	%eax, -12(%rbp)
	cmpl	$0, %eax
	jle	.Lgetword.6
	movq	-24(%rbp), %rax
	movq	%rax, %rcx
	addq	$1, %rax
	movq	%rax, -24(%rbp)
	leaq	text(%rip), %rax
	movl	at(%rip), %edx
	movl	%edx, %esi
	addl	$1, %edx
	movl	%edx, at(%rip)
	movslq	%esi, %rsi
	movsbl	(%rax,%rsi,1), %eax
	movb	%al, (%rcx)
	jmp	.Lgetword.5
.Lgetword.6:
	movq	-24(%rbp), %rax
	movb	$0, (%rax)
	movl	$1, %eax
	jmp	.Lgetword.1
.Lgetword.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	getword, .-getword
	.type	addtree, @function
addtree:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$112, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	cmpq	$0, -8(%rbp)
	jne	.Laddtree.2
	movq	$32, -32(%rbp)
	movq	-32(%rbp), %rdi
	call	malloc@PLT
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -40(%rbp)
	movq	-16(%rbp), %rax
	movq	%rax, -48(%rbp)
	movq	-48(%rbp), %rdi
	call	strdup@PLT
	movq	-40(%rbp), %rcx
	movq	%rax, (%rcx)
	movq	-8(%rbp), %rax
	movl	$1, 8(%rax)
	movq	-8(%rbp), %rax
	movq	-8(%rbp), %rcx
	movq	$0, %rdx
	movq	%rdx, 24(%rcx)
	movq	%rdx, 16(%rax)
	jmp	.Laddtree.3
.Laddtree.2:
	movq	-16(%rbp), %rax
	movq	%rax, -56(%rbp)
	movq	-8(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, -64(%rbp)
	movq	-56(%rbp), %rdi
	movq	-64(%rbp), %rsi
	call	strcmp@PLT
	movl	%eax, -20(%rbp)
	cmpl	$0, %eax
	jne	.Laddtree.4
	movq	-8(%rbp), %rax
	addl	$1, 8(%rax)
	jmp	.Laddtree.5
.Laddtree.4:
	cmpl	$0, -20(%rbp)
	jge	.Laddtree.6
	movq	-8(%rbp), %rax
	movq	%rax, -72(%rbp)
	movq	-8(%rbp), %rax
	movq	16(%rax), %rax
	movq	%rax, -80(%rbp)
	movq	-16(%rbp), %rax
	movq	%rax, -88(%rbp)
	movq	-80(%rbp), %rdi
	movq	-88(%rbp), %rsi
	call	addtree@PLT
	movq	-72(%rbp), %rcx
	movq	%rax, 16(%rcx)
	jmp	.Laddtree.7
.Laddtree.6:
	movq	-8(%rbp), %rax
	movq	%rax, -96(%rbp)
	movq	-8(%rbp), %rax
	movq	24(%rax), %rax
	movq	%rax, -104(%rbp)
	movq	-16(%rbp), %rax
	movq	%rax, -112(%rbp)
	movq	-104(%rbp), %rdi
	movq	-112(%rbp), %rsi
	call	addtree@PLT
	movq	-96(%rbp), %rcx
	movq	%rax, 24(%rcx)
.Laddtree.7:
.Laddtree.5:
.Laddtree.3:
	movq	-8(%rbp), %rax
	jmp	.Laddtree.1
.Laddtree.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	addtree, .-addtree
	.type	treeprint, @function
treeprint:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$64, %rsp
	movq	%rdi, -8(%rbp)
	cmpq	$0, -8(%rbp)
	jne	.Ltreeprint.2
	movl	$0, %eax
	jmp	.Ltreeprint.1
.Ltreeprint.2:
	movq	-8(%rbp), %rax
	movq	16(%rax), %rax
	movq	%rax, -24(%rbp)
	movq	-24(%rbp), %rdi
	call	treeprint@PLT
	movl	%eax, -12(%rbp)
	leaq	.Ls0(%rip), %rax
	movq	%rax, -32(%rbp)
	movq	-8(%rbp), %rax
	movl	8(%rax), %eax
	movl	%eax, -36(%rbp)
	movq	-8(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, -48(%rbp)
	movq	-32(%rbp), %rdi
	movl	-36(%rbp), %esi
	movq	-48(%rbp), %rdx
	movl	$0, %eax
	call	printf@PLT
	movl	-12(%rbp), %eax
	addl	$1, %eax
	movq	%rax, -56(%rbp)
	movq	-8(%rbp), %rax
	movq	24(%rax), %rax
	movq	%rax, -64(%rbp)
	movq	-64(%rbp), %rdi
	call	treeprint@PLT
	addl	-56(%rbp), %eax
	jmp	.Ltreeprint.1
.Ltreeprint.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	treeprint, .-treeprint
	.type	count, @function
count:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$48, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	.p2align 5
.Lcount.2:
	cmpq	$0, -8(%rbp)
	je	.Lcount.3
	movq	-16(%rbp), %rax
	movq	%rax, -32(%rbp)
	movq	-8(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, -40(%rbp)
	movq	-32(%rbp), %rdi
	movq	-40(%rbp), %rsi
	call	strcmp@PLT
	movl	%eax, -20(%rbp)
	cmpl	$0, -20(%rbp)
	jne	.Lcount.4
	movq	-8(%rbp), %rax
	movl	8(%rax), %eax
	jmp	.Lcount.1
.Lcount.4:
	cmpl	$0, -20(%rbp)
	jge	.Lcount.5
	movq	-8(%rbp), %rax
	movq	16(%rax), %rax
	jmp	.Lcount.6
.Lcount.5:
	movq	-8(%rbp), %rax
	movq	24(%rax), %rax
.Lcount.6:
	movq	%rax, -8(%rbp)
	jmp	.Lcount.2
.Lcount.3:
	movl	$0, %eax
	jmp	.Lcount.1
.Lcount.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	count, .-count
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$256, %rsp
	movq	$0, -8(%rbp)
	movl	$0, -112(%rbp)
	.p2align 5
.Lmain.2:
	leaq	-108(%rbp), %rax
	movq	%rax, -120(%rbp)
	movl	$100, -124(%rbp)
	movq	-120(%rbp), %rdi
	movl	-124(%rbp), %esi
	call	getword@PLT
	cmpl	$0, %eax
	je	.Lmain.3
	movq	-8(%rbp), %rax
	movq	%rax, -136(%rbp)
	leaq	-108(%rbp), %rax
	movq	%rax, -144(%rbp)
	movq	-136(%rbp), %rdi
	movq	-144(%rbp), %rsi
	call	addtree@PLT
	movq	%rax, -8(%rbp)
	addl	$1, -112(%rbp)
	jmp	.Lmain.2
.Lmain.3:
	movq	-8(%rbp), %rax
	movq	%rax, -160(%rbp)
	movq	-160(%rbp), %rdi
	call	treeprint@PLT
	movl	%eax, -148(%rbp)
	cmpl	$20, -112(%rbp)
	je	.Lmain.4
	movl	$101, %eax
	jmp	.Lmain.1
.Lmain.4:
	movq	-8(%rbp), %rax
	movq	%rax, -168(%rbp)
	leaq	.Ls1(%rip), %rax
	movq	%rax, -176(%rbp)
	movq	-168(%rbp), %rdi
	movq	-176(%rbp), %rsi
	call	count@PLT
	cmpl	$2, %eax
	jne	.Lmain.6
	movq	-8(%rbp), %rax
	movq	%rax, -184(%rbp)
	leaq	.Ls2(%rip), %rax
	movq	%rax, -192(%rbp)
	movq	-184(%rbp), %rdi
	movq	-192(%rbp), %rsi
	call	count@PLT
	cmpl	$2, %eax
	je	.Lmain.5
.Lmain.6:
	movl	$102, %eax
	jmp	.Lmain.1
.Lmain.5:
	movq	-8(%rbp), %rax
	movq	%rax, -200(%rbp)
	leaq	.Ls3(%rip), %rax
	movq	%rax, -208(%rbp)
	movq	-200(%rbp), %rdi
	movq	-208(%rbp), %rsi
	call	count@PLT
	cmpl	$2, %eax
	jne	.Lmain.8
	movq	-8(%rbp), %rax
	movq	%rax, -216(%rbp)
	leaq	.Ls4(%rip), %rax
	movq	%rax, -224(%rbp)
	movq	-216(%rbp), %rdi
	movq	-224(%rbp), %rsi
	call	count@PLT
	cmpl	$1, %eax
	je	.Lmain.7
.Lmain.8:
	movl	$103, %eax
	jmp	.Lmain.1
.Lmain.7:
	imull	$3, -148(%rbp), %eax
	movq	%rax, -232(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -240(%rbp)
	leaq	.Ls5(%rip), %rax
	movq	%rax, -248(%rbp)
	movq	-240(%rbp), %rdi
	movq	-248(%rbp), %rsi
	call	count@PLT
	addl	-232(%rbp), %eax
	jmp	.Lmain.1
.Lmain.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	main, .-main
	.data
.Ls0:
	.string	"%4d %s\n"
.Ls1:
	.string	"of"
.Ls2:
	.string	"tree"
.Ls3:
	.string	"a"
.Ls4:
	.string	"count"
.Ls5:
	.string	"the"
	.data
	.type	text, @object
	.size	text, 90
	.p2align 4
text:
	.string	"the shape of a tree: the root, the nodes of the tree, a word and its count, left or right"
	.bss
	.type	at, @object
	.size	at, 4
	.p2align 2
at:
	.zero	4
	.section	.note.GNU-stack,"",@progbits
