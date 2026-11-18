	.file	"header-squeeze.c"
	.text
	.type	count, @function
count:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -16(%rbp)
	movq	-16(%rbp), %rdi
	call	strlen@PLT
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
	subq	$112, %rsp
	leaq	text(%rip), %rax
	movq	%rax, -16(%rbp)
	movl	$97, -20(%rbp)
	movq	-16(%rbp), %rdi
	movl	-20(%rbp), %esi
	call	squeeze@PLT
	leaq	text(%rip), %rax
	movq	%rax, -32(%rbp)
	leaq	tail(%rip), %rax
	movq	%rax, -40(%rbp)
	movq	-32(%rbp), %rdi
	movq	-40(%rbp), %rsi
	call	concat@PLT
	leaq	text(%rip), %rax
	movq	%rax, -48(%rbp)
	movq	-48(%rbp), %rdi
	call	strlen@PLT
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	subl	$100, %eax
	movl	%eax, -52(%rbp)
	movl	-52(%rbp), %edi
	call	abs@PLT
	movl	%eax, -4(%rbp)
	leaq	.Ls0(%rip), %rax
	movq	%rax, -64(%rbp)
	leaq	text(%rip), %rax
	movq	%rax, -72(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -76(%rbp)
	movq	-64(%rbp), %rdi
	movq	-72(%rbp), %rsi
	movl	-76(%rbp), %edx
	movl	$0, %eax
	call	printf@PLT
	leaq	text(%rip), %rax
	movq	%rax, -88(%rbp)
	leaq	.Ls1(%rip), %rax
	movq	%rax, -96(%rbp)
	movq	-88(%rbp), %rdi
	movq	-96(%rbp), %rsi
	call	strcmp@PLT
	cmpl	$0, %eax
	je	.Lmain.2
	movl	$101, %eax
	jmp	.Lmain.1
.Lmain.2:
	leaq	text(%rip), %rax
	movq	%rax, -104(%rbp)
	movq	-104(%rbp), %rdi
	call	count@PLT
	cmpl	$15, %eax
	jne	.Lmain.4
	leaq	tail(%rip), %rax
	movq	%rax, -112(%rbp)
	movq	-112(%rbp), %rdi
	call	count@PLT
	cmpl	$7, %eax
	je	.Lmain.3
.Lmain.4:
	movl	$102, %eax
	jmp	.Lmain.1
.Lmain.3:
	cmpl	$85, -4(%rbp)
	je	.Lmain.5
	movl	$103, %eax
	jmp	.Lmain.1
.Lmain.5:
	movl	-4(%rbp), %eax
	jmp	.Lmain.1
.Lmain.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	main, .-main
	.globl	squeeze
	.type	squeeze, @function
squeeze:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movl	$0, %eax
	movl	%eax, -20(%rbp)
	movl	%eax, -16(%rbp)
	.p2align 5
.Lsqueeze.2:
	movq	-8(%rbp), %rax
	movslq	-16(%rbp), %rcx
	movsbl	(%rax,%rcx,1), %eax
	cmpl	$0, %eax
	je	.Lsqueeze.4
	movq	-8(%rbp), %rax
	movslq	-16(%rbp), %rcx
	movsbl	(%rax,%rcx,1), %eax
	cmpl	-12(%rbp), %eax
	je	.Lsqueeze.5
	movq	-8(%rbp), %rax
	movl	-20(%rbp), %ecx
	movl	%ecx, %edx
	addl	$1, %ecx
	movl	%ecx, -20(%rbp)
	movslq	%edx, %rdx
	movq	-8(%rbp), %rcx
	movslq	-16(%rbp), %rsi
	movsbl	(%rcx,%rsi,1), %ecx
	movb	%cl, (%rax,%rdx,1)
.Lsqueeze.5:
	addl	$1, -16(%rbp)
	jmp	.Lsqueeze.2
.Lsqueeze.4:
	movq	-8(%rbp), %rax
	movslq	-20(%rbp), %rcx
	movb	$0, (%rax,%rcx,1)
.Lsqueeze.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	squeeze, .-squeeze
	.globl	concat
	.type	concat, @function
concat:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movl	$0, %eax
	movl	%eax, -24(%rbp)
	movl	%eax, -20(%rbp)
	.p2align 5
.Lconcat.2:
	movq	-8(%rbp), %rax
	movslq	-20(%rbp), %rcx
	movsbl	(%rax,%rcx,1), %eax
	cmpl	$0, %eax
	je	.Lconcat.3
	addl	$1, -20(%rbp)
	jmp	.Lconcat.2
.Lconcat.3:
	.p2align 5
.Lconcat.4:
	movq	-8(%rbp), %rax
	movl	-20(%rbp), %ecx
	movl	%ecx, %edx
	addl	$1, %ecx
	movl	%ecx, -20(%rbp)
	movslq	%edx, %rdx
	movq	-16(%rbp), %rcx
	movl	-24(%rbp), %esi
	movl	%esi, %edi
	addl	$1, %esi
	movl	%esi, -24(%rbp)
	movslq	%edi, %rdi
	movsbl	(%rcx,%rdi,1), %ecx
	movb	%cl, (%rax,%rdx,1)
	cmpl	$0, %ecx
	je	.Lconcat.5
	jmp	.Lconcat.4
.Lconcat.5:
.Lconcat.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	concat, .-concat
	.data
.Ls0:
	.string	"%s %d\n"
.Ls1:
	.string	"bnn bndn cabana"
	.data
	.globl	text
	.type	text, @object
	.size	text, 64
	.p2align 4
text:
	.string	"banana bandana"
	.zero	49
	.globl	tail
	.type	tail, @object
	.size	tail, 8
tail:
	.string	" cabana"
	.section	.note.GNU-stack,"",@progbits
