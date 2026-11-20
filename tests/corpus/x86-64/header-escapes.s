	.file	"header-escapes.c"
	.text
	.type	escape_name, @function
escape_name:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	subl	$7, %eax
	cmpl	$6, %eax
	ja	.Lescape_name.9
	leaq	.Lescape_name.10(%rip), %rcx
	jmp	*(%rcx,%rax,8)
	.data
	.p2align 3
.Lescape_name.10:
	.quad	.Lescape_name.2
	.quad	.Lescape_name.3
	.quad	.Lescape_name.4
	.quad	.Lescape_name.5
	.quad	.Lescape_name.6
	.quad	.Lescape_name.7
	.quad	.Lescape_name.8
	.text
.Lescape_name.2:
	leaq	.Ls0(%rip), %rax
	jmp	.Lescape_name.1
.Lescape_name.3:
	leaq	.Ls1(%rip), %rax
	jmp	.Lescape_name.1
.Lescape_name.4:
	leaq	.Ls2(%rip), %rax
	jmp	.Lescape_name.1
.Lescape_name.5:
	leaq	.Ls3(%rip), %rax
	jmp	.Lescape_name.1
.Lescape_name.6:
	leaq	.Ls4(%rip), %rax
	jmp	.Lescape_name.1
.Lescape_name.7:
	leaq	.Ls5(%rip), %rax
	jmp	.Lescape_name.1
.Lescape_name.8:
	leaq	.Ls6(%rip), %rax
	jmp	.Lescape_name.1
.Lescape_name.9:
	movq	$0, %rax
	jmp	.Lescape_name.1
.Lescape_name.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	escape_name, .-escape_name
	.type	leap, @function
leap:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	$4, %ecx
	cltd
	idivl	%ecx
	cmpl	$0, %edx
	jne	.Lleap.4
	movl	-4(%rbp), %eax
	movl	$100, %ecx
	cltd
	idivl	%ecx
	cmpl	$0, %edx
	jne	.Lleap.3
.Lleap.4:
	movl	-4(%rbp), %eax
	movl	$400, %ecx
	cltd
	idivl	%ecx
	cmpl	$0, %edx
	jne	.Lleap.2
.Lleap.3:
	movl	$1, %eax
	jmp	.Lleap.1
.Lleap.2:
	movl	$0, %eax
	jmp	.Lleap.1
.Lleap.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	leap, .-leap
	.type	days, @function
days:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	leaq	month_days(%rip), %rax
	movl	-4(%rbp), %ecx
	movl	(%rax,%rcx,4), %eax
	movq	%rax, -16(%rbp)
	cmpl	$2, -4(%rbp)
	jne	.Ldays.2
	cmpl	$1, -8(%rbp)
	jne	.Ldays.2
	movl	$1, %ecx
	jmp	.Ldays.3
.Ldays.2:
	movl	$0, %ecx
.Ldays.3:
	movq	-16(%rbp), %rax
	addl	%ecx, %eax
	jmp	.Ldays.1
.Ldays.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	days, .-days
	.type	year_days, @function
year_days:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movl	%edi, -4(%rbp)
	movl	$0, -12(%rbp)
	movl	$1, -8(%rbp)
	.p2align 5
.Lyear_days.2:
	cmpl	$12, -8(%rbp)
	ja	.Lyear_days.4
	movl	-8(%rbp), %eax
	movl	%eax, -16(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -24(%rbp)
	movl	-24(%rbp), %edi
	call	leap@PLT
	movl	%eax, -20(%rbp)
	movl	-16(%rbp), %edi
	movl	-20(%rbp), %esi
	call	days@PLT
	addl	%eax, -12(%rbp)
	addl	$1, -8(%rbp)
	jmp	.Lyear_days.2
.Lyear_days.4:
	movl	-12(%rbp), %eax
	jmp	.Lyear_days.1
.Lyear_days.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	year_days, .-year_days
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$176, %rsp
	movl	$-1, -4(%rbp)
	movl	$0, -20(%rbp)
	movl	$0, -24(%rbp)
	.p2align 5
.Lmain.2:
	leaq	text(%rip), %rax
	movslq	-24(%rbp), %rcx
	movsbl	(%rax,%rcx,1), %eax
	cmpl	$0, %eax
	je	.Lmain.4
	leaq	text(%rip), %rax
	movslq	-24(%rbp), %rcx
	movsbl	(%rax,%rcx,1), %eax
	movl	%eax, -28(%rbp)
	movl	-28(%rbp), %edi
	call	escape_name@PLT
	movq	%rax, -16(%rbp)
	cmpq	$0, -16(%rbp)
	je	.Lmain.3
	movl	-20(%rbp), %eax
	movl	%eax, %ecx
	addl	$1, %eax
	movl	%eax, -20(%rbp)
	cmpl	$0, %ecx
	jle	.Lmain.5
	leaq	names(%rip), %rax
	movq	%rax, -40(%rbp)
	leaq	.Ls7(%rip), %rax
	movq	%rax, -48(%rbp)
	movq	-40(%rbp), %rdi
	movq	-48(%rbp), %rsi
	call	strcat@PLT
.Lmain.5:
	leaq	names(%rip), %rax
	movq	%rax, -56(%rbp)
	movq	-16(%rbp), %rax
	movq	%rax, -64(%rbp)
	movq	-56(%rbp), %rdi
	movq	-64(%rbp), %rsi
	call	strcat@PLT
.Lmain.3:
	addl	$1, -24(%rbp)
	jmp	.Lmain.2
.Lmain.4:
	leaq	.Ls8(%rip), %rax
	movq	%rax, -72(%rbp)
	leaq	names(%rip), %rax
	movq	%rax, -80(%rbp)
	movl	$1988, -88(%rbp)
	movl	-88(%rbp), %edi
	call	year_days@PLT
	movl	%eax, -84(%rbp)
	movl	$1900, -96(%rbp)
	movl	-96(%rbp), %edi
	call	year_days@PLT
	movl	%eax, -92(%rbp)
	movq	-72(%rbp), %rdi
	movq	-80(%rbp), %rsi
	movl	-84(%rbp), %edx
	movl	-92(%rbp), %ecx
	movl	$0, %eax
	call	printf@PLT
	leaq	names(%rip), %rax
	movq	%rax, -104(%rbp)
	leaq	.Ls9(%rip), %rax
	movq	%rax, -112(%rbp)
	movq	-104(%rbp), %rdi
	movq	-112(%rbp), %rsi
	call	strcmp@PLT
	cmpl	$0, %eax
	je	.Lmain.6
	movl	$101, %eax
	jmp	.Lmain.1
.Lmain.6:
	movl	$1988, -116(%rbp)
	movl	-116(%rbp), %edi
	call	year_days@PLT
	cmpl	$366, %eax
	jne	.Lmain.8
	movl	$1900, -120(%rbp)
	movl	-120(%rbp), %edi
	call	year_days@PLT
	cmpl	$365, %eax
	jne	.Lmain.8
	movl	$2000, -124(%rbp)
	movl	-124(%rbp), %edi
	call	year_days@PLT
	cmpl	$366, %eax
	je	.Lmain.7
.Lmain.8:
	movl	$102, %eax
	jmp	.Lmain.1
.Lmain.7:
	movl	$2, -128(%rbp)
	movl	$1988, -136(%rbp)
	movl	-136(%rbp), %edi
	call	leap@PLT
	movl	%eax, -132(%rbp)
	movl	-128(%rbp), %edi
	movl	-132(%rbp), %esi
	call	days@PLT
	cmpl	$29, %eax
	jne	.Lmain.10
	movl	$2, -140(%rbp)
	movl	$0, -144(%rbp)
	movl	-140(%rbp), %edi
	movl	-144(%rbp), %esi
	call	days@PLT
	cmpl	$28, %eax
	jne	.Lmain.10
	movl	$12, -148(%rbp)
	movl	$1, -152(%rbp)
	movl	-148(%rbp), %edi
	movl	-152(%rbp), %esi
	call	days@PLT
	cmpl	$31, %eax
	je	.Lmain.9
.Lmain.10:
	movl	$103, %eax
	jmp	.Lmain.1
.Lmain.9:
	cmpl	$0, -4(%rbp)
	jb	.Lmain.12
	movl	$0, %eax
	cmpl	$0, %eax
	je	.Lmain.11
.Lmain.12:
	movl	$104, %eax
	jmp	.Lmain.1
.Lmain.11:
	movl	$2, -156(%rbp)
	movl	$1988, -164(%rbp)
	movl	-164(%rbp), %edi
	call	leap@PLT
	movl	%eax, -160(%rbp)
	movl	-156(%rbp), %edi
	movl	-160(%rbp), %esi
	call	days@PLT
	addl	-20(%rbp), %eax
	jmp	.Lmain.1
.Lmain.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	main, .-main
	.data
.Ls0:
	.string	"BELL"
.Ls1:
	.string	"BACKSPACE"
.Ls2:
	.string	"TAB"
.Ls3:
	.string	"NEWLINE"
.Ls4:
	.string	"VTAB"
.Ls5:
	.string	"FORMFEED"
.Ls6:
	.string	"RETURN"
.Ls7:
	.string	" "
.Ls8:
	.string	"%s; 1988 has %d days, 1900 has %d\n"
.Ls9:
	.string	"TAB NEWLINE BELL RETURN VTAB BACKSPACE FORMFEED"
	.data
	.type	month_days, @object
	.size	month_days, 52
	.p2align 4
month_days:
	.long	0
	.long	31
	.long	28
	.long	31
	.long	30
	.long	31
	.long	30
	.long	31
	.long	31
	.long	30
	.long	31
	.long	30
	.long	31
	.type	text, @object
	.size	text, 15
text:
	.string	"a\tb\nc\007d\re\013f\010g\014"
	.bss
	.type	names, @object
	.size	names, 64
	.p2align 4
names:
	.zero	64
	.section	.note.GNU-stack,"",@progbits
