	.file	"header-collatz.c"
	.text
	.type	halve, @function
halve:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rcx
	movq	$2, %rsi
	movq	(%rcx), %rax
	cqto
	idivq	%rsi
	movq	%rax, (%rcx)
	addq	$1, steps(%rip)
.Lhalve.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	halve, .-halve
	.type	triple, @function
triple:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	-8(%rbp), %rcx
	imulq	$3, (%rcx), %rcx
	addq	$1, %rcx
	movq	%rcx, (%rax)
	addq	$1, steps(%rip)
.Ltriple.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	triple, .-triple
	.type	odd, @function
odd:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	addq	$1, odd_values(%rip)
.Lodd.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	odd, .-odd
	.type	even, @function
even:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	addq	$1, even_values(%rip)
.Leven.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	even, .-even
	.type	quarter, @function
quarter:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	addq	$1, quarter_values(%rip)
.Lquarter.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	quarter, .-quarter
	.type	nothing, @function
nothing:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
.Lnothing.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	nothing, .-nothing
	.type	record, @function
record:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	cmpq	peak(%rip), %rax
	jle	.Lrecord.2
	movq	-8(%rbp), %rax
	movq	%rax, peak(%rip)
.Lrecord.2:
.Lrecord.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	record, .-record
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$96, %rsp
	movq	$27, -8(%rbp)
	.p2align 5
.Lmain.2:
	cmpq	$1, -8(%rbp)
	je	.Lmain.4
	movq	-8(%rbp), %rax
	movq	$2, %rcx
	cqto
	idivq	%rcx
	cmpq	$0, %rdx
	je	.Lmain.5
	call	odd@PLT
	jmp	.Lmain.6
.Lmain.5:
	call	even@PLT
.Lmain.6:
	movq	-8(%rbp), %rax
	movq	%rax, -16(%rbp)
	movq	-16(%rbp), %rdi
	call	record@PLT
	movq	-8(%rbp), %rax
	movq	$2, %rcx
	cqto
	idivq	%rcx
	cmpq	$0, %rdx
	je	.Lmain.7
	movq	-8(%rbp), %rax
	movq	$4, %rcx
	cqto
	idivq	%rcx
	cmpq	$1, %rdx
	jne	.Lmain.9
	call	quarter@PLT
	jmp	.Lmain.10
.Lmain.9:
	call	nothing@PLT
.Lmain.10:
	jmp	.Lmain.8
.Lmain.7:
	call	nothing@PLT
.Lmain.8:
	movq	-8(%rbp), %rax
	movq	$2, %rcx
	cqto
	idivq	%rcx
	cmpq	$0, %rdx
	je	.Lmain.11
	leaq	-8(%rbp), %rax
	movq	%rax, -24(%rbp)
	movq	-24(%rbp), %rdi
	call	triple@PLT
	jmp	.Lmain.12
.Lmain.11:
	leaq	-8(%rbp), %rax
	movq	%rax, -32(%rbp)
	movq	-32(%rbp), %rdi
	call	halve@PLT
.Lmain.12:
	jmp	.Lmain.2
.Lmain.4:
	leaq	.Ls0(%rip), %rax
	movq	%rax, -40(%rbp)
	movq	steps(%rip), %rax
	movq	%rax, -48(%rbp)
	movq	odd_values(%rip), %rax
	movq	%rax, -56(%rbp)
	movq	even_values(%rip), %rax
	movq	%rax, -64(%rbp)
	movq	peak(%rip), %rax
	movq	%rax, -72(%rbp)
	movq	-40(%rbp), %rdi
	movq	-48(%rbp), %rsi
	movq	-56(%rbp), %rdx
	movq	-64(%rbp), %rcx
	movq	-72(%rbp), %r8
	movl	$0, %eax
	call	printf@PLT
	leaq	.Ls1(%rip), %rax
	movq	%rax, -80(%rbp)
	movq	quarter_values(%rip), %rax
	movq	%rax, -88(%rbp)
	movq	-80(%rbp), %rdi
	movq	-88(%rbp), %rsi
	movl	$0, %eax
	call	printf@PLT
	cmpq	$111, steps(%rip)
	jne	.Lmain.14
	cmpq	$41, odd_values(%rip)
	jne	.Lmain.14
	cmpq	$70, even_values(%rip)
	je	.Lmain.13
.Lmain.14:
	movl	$201, %eax
	jmp	.Lmain.1
.Lmain.13:
	cmpq	$9232, peak(%rip)
	je	.Lmain.15
	movl	$202, %eax
	jmp	.Lmain.1
.Lmain.15:
	cmpq	$17, quarter_values(%rip)
	je	.Lmain.16
	movl	$203, %eax
	jmp	.Lmain.1
.Lmain.16:
	movq	steps(%rip), %rax
	jmp	.Lmain.1
.Lmain.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	main, .-main
	.data
.Ls0:
	.string	"27: %ld steps, %ld odd, %ld even, peak %ld\n"
.Ls1:
	.string	"odd values 1 modulo 4: %ld\n"
	.bss
	.type	steps, @object
	.size	steps, 8
	.p2align 3
steps:
	.zero	8
	.type	odd_values, @object
	.size	odd_values, 8
	.p2align 3
odd_values:
	.zero	8
	.type	even_values, @object
	.size	even_values, 8
	.p2align 3
even_values:
	.zero	8
	.type	quarter_values, @object
	.size	quarter_values, 8
	.p2align 3
quarter_values:
	.zero	8
	.type	peak, @object
	.size	peak, 8
	.p2align 3
peak:
	.zero	8
	.section	.note.GNU-stack,"",@progbits
