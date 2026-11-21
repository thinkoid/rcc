	.file	"conditional-omitted-once.c"
	.text
	.globl	next
	.type	next, @function
next:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	addl	$1, calls(%rip)
	movl	counter(%rip), %ecx
	movl	%ecx, %eax
	addl	$1, %ecx
	movl	%ecx, counter(%rip)
	jmp	.Lnext.1
.Lnext.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	next, .-next
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	subq	$40, %rsp
	call	next@PLT
	cmpl	$0, %eax
	jne	.Lmain.4
	movl	$7, %eax
.Lmain.4:
	cmpl	$7, %eax
	jne	.Lmain.3
	cmpl	$1, calls(%rip)
	je	.Lmain.2
.Lmain.3:
	movl	$101, %eax
	jmp	.Lmain.1
.Lmain.2:
	call	next@PLT
	cmpl	$0, %eax
	jne	.Lmain.7
	movl	$7, %eax
.Lmain.7:
	cmpl	$1, %eax
	jne	.Lmain.6
	cmpl	$2, calls(%rip)
	je	.Lmain.5
.Lmain.6:
	movl	$102, %eax
	jmp	.Lmain.1
.Lmain.5:
	movb	$-3, -9(%rbp)
	movsbl	-9(%rbp), %eax
	cmpl	$0, %eax
	jne	.Lmain.9
	movl	$9, %eax
.Lmain.9:
	cmpl	$-3, %eax
	je	.Lmain.8
	movl	$103, %eax
	jmp	.Lmain.1
.Lmain.8:
	movb	$0, -9(%rbp)
	movsbl	-9(%rbp), %eax
	cmpl	$0, %eax
	jne	.Lmain.11
	movl	$9, %eax
.Lmain.11:
	cmpl	$9, %eax
	je	.Lmain.10
	movl	$104, %eax
	jmp	.Lmain.1
.Lmain.10:
	movq	$0, -24(%rbp)
	leaq	.Ls0(%rip), %rax
	movq	%rax, -32(%rbp)
	movq	-24(%rbp), %rax
	cmpq	$0, %rax
	jne	.Lmain.14
	movq	-32(%rbp), %rax
.Lmain.14:
	cmpq	-32(%rbp), %rax
	jne	.Lmain.13
	movq	-32(%rbp), %rax
	cmpq	$0, %rax
	jne	.Lmain.15
	movq	-24(%rbp), %rax
.Lmain.15:
	cmpq	-32(%rbp), %rax
	je	.Lmain.12
.Lmain.13:
	movl	$105, %eax
	jmp	.Lmain.1
.Lmain.12:
	call	next@PLT
	movl	%eax, %ebx
	cmpl	$0, %ebx
	jne	.Lmain.16
	movl	$5, %ebx
.Lmain.16:
	cmpl	$2, %ebx
	jne	.Lmain.18
	cmpl	$3, calls(%rip)
	je	.Lmain.17
.Lmain.18:
	movl	$106, %eax
	jmp	.Lmain.1
.Lmain.17:
	movl	$0, counter(%rip)
	call	next@PLT
	cmpl	$0, %eax
	jne	.Lmain.20
	movl	$0, %eax
.Lmain.20:
	cmpl	$0, %eax
	je	.Lmain.19
	movl	$107, %eax
	jmp	.Lmain.1
.Lmain.19:
	movl	$0, counter(%rip)
	movl	$0, %eax
	cmpl	$0, %eax
	jne	.Lmain.23
	call	next@PLT
	cmpl	$0, %eax
	jne	.Lmain.24
	movl	$8, %eax
.Lmain.24:
.Lmain.23:
	cmpl	$8, %eax
	jne	.Lmain.22
	cmpl	$5, calls(%rip)
	je	.Lmain.21
.Lmain.22:
	movl	$108, %eax
	jmp	.Lmain.1
.Lmain.21:
	movl	$-5, -36(%rbp)
	movslq	-36(%rbp), %rax
	cmpq	$0, %rax
	jne	.Lmain.25
	movq	$3, %rax
.Lmain.25:
	movq	%rax, -48(%rbp)
	cmpq	$-5, -48(%rbp)
	je	.Lmain.26
	movl	$109, %eax
	jmp	.Lmain.1
.Lmain.26:
	call	next@PLT
	cmpl	$0, %eax
	jne	.Lmain.27
	addl	$10, calls(%rip)
.Lmain.27:
	cmpl	$6, calls(%rip)
	je	.Lmain.28
	movl	$110, %eax
	jmp	.Lmain.1
.Lmain.28:
	movl	$0, counter(%rip)
	call	next@PLT
	cmpl	$0, %eax
	jne	.Lmain.29
	addl	$10, calls(%rip)
.Lmain.29:
	cmpl	$17, calls(%rip)
	je	.Lmain.30
	movl	$111, %eax
	jmp	.Lmain.1
.Lmain.30:
	movl	$42, %eax
	jmp	.Lmain.1
.Lmain.1:
	leaq	-8(%rbp), %rsp
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	main, .-main
	.data
.Ls0:
	.string	"q"
	.bss
	.globl	counter
	.type	counter, @object
	.size	counter, 4
	.p2align 2
counter:
	.zero	4
	.globl	calls
	.type	calls, @object
	.size	calls, 4
	.p2align 2
calls:
	.zero	4
	.section	.note.GNU-stack,"",@progbits
