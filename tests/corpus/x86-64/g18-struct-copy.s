	.file	"g18-struct-copy.c"
	.text
	.globl	at
	.type	at, @function
at:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rax
	movslq	-12(%rbp), %rcx
	leaq	(%rax,%rcx,4), %rax
	jmp	.Lat.1
.Lat.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	at, .-at
	.globl	fill
	.type	fill, @function
fill:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	$0, -4(%rbp)
	.p2align 5
.Lfill.2:
	cmpl	$8, -4(%rbp)
	jge	.Lfill.4
	leaq	codes(%rip), %rax
	movslq	-4(%rbp), %rcx
	imull	$7, -4(%rbp), %edx
	addl	$1, %edx
	movb	%dl, (%rax,%rcx,4)
	leaq	codes(%rip), %rax
	movslq	-4(%rbp), %rcx
	movl	-4(%rbp), %edx
	addl	$3, %edx
	movb	%dl, 1(%rax,%rcx,4)
	leaq	codes(%rip), %rax
	movslq	-4(%rbp), %rcx
	imull	$1000, -4(%rbp), %edx
	addl	$17, %edx
	movw	%dx, 2(%rax,%rcx,4)
	leaq	points(%rip), %rax
	movslq	-4(%rbp), %rcx
	imull	$11, -4(%rbp), %edx
	subl	$5, %edx
	movl	%edx, (%rax,%rcx,8)
	leaq	points(%rip), %rax
	movslq	-4(%rbp), %rcx
	movl	-4(%rbp), %edx
	imull	-4(%rbp), %edx
	movl	%edx, 4(%rax,%rcx,8)
	addl	$1, -4(%rbp)
	jmp	.Lfill.2
.Lfill.4:
	movl	$0, -4(%rbp)
	.p2align 5
.Lfill.5:
	cmpl	$4, -4(%rbp)
	jge	.Lfill.7
	movl	$0, -8(%rbp)
	.p2align 5
.Lfill.8:
	cmpl	$32, -8(%rbp)
	jge	.Lfill.10
	leaq	texts(%rip), %rax
	movslq	-4(%rbp), %rcx
	imulq	$32, %rcx, %rcx
	addq	%rcx, %rax
	movslq	-8(%rbp), %rcx
	imull	$32, -4(%rbp), %edx
	addl	-8(%rbp), %edx
	movb	%dl, (%rax,%rcx,1)
	addl	$1, -8(%rbp)
	jmp	.Lfill.8
.Lfill.10:
	addl	$1, -4(%rbp)
	jmp	.Lfill.5
.Lfill.7:
	movl	$0, -4(%rbp)
	.p2align 5
.Lfill.11:
	cmpl	$4, -4(%rbp)
	jge	.Lfill.13
	movl	$0, -8(%rbp)
	.p2align 5
.Lfill.14:
	cmpl	$9, -8(%rbp)
	jge	.Lfill.16
	leaq	nines(%rip), %rax
	movslq	-4(%rbp), %rcx
	imulq	$72, %rcx, %rcx
	addq	%rcx, %rax
	movslq	-8(%rbp), %rcx
	movslq	-4(%rbp), %rdx
	imulq	$1000003, %rdx, %rdx
	movslq	-8(%rbp), %rsi
	addq	%rsi, %rdx
	movq	%rdx, (%rax,%rcx,8)
	addl	$1, -8(%rbp)
	jmp	.Lfill.14
.Lfill.16:
	addl	$1, -4(%rbp)
	jmp	.Lfill.11
.Lfill.13:
.Lfill.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	fill, .-fill
	.globl	rotate
	.type	rotate, @function
rotate:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$128, %rsp
	movzwl	codes(%rip), %eax
	movw	%ax, -4(%rbp)
	movzwl	codes+2(%rip), %eax
	movw	%ax, -2(%rbp)
	movl	$0, -52(%rbp)
	.p2align 5
.Lrotate.2:
	cmpl	$7, -52(%rbp)
	jge	.Lrotate.4
	leaq	codes(%rip), %rax
	movslq	-52(%rbp), %rcx
	leaq	codes(%rip), %rdx
	movl	-52(%rbp), %esi
	addl	$1, %esi
	movslq	%esi, %rsi
	movzwl	(%rdx,%rsi,4), %edi
	movw	%di, (%rax,%rcx,4)
	movzwl	2(%rdx,%rsi,4), %edi
	movw	%di, 2(%rax,%rcx,4)
	addl	$1, -52(%rbp)
	jmp	.Lrotate.2
.Lrotate.4:
	movzwl	-4(%rbp), %eax
	movw	%ax, codes+28(%rip)
	movzwl	-2(%rbp), %eax
	movw	%ax, codes+30(%rip)
	leaq	points(%rip), %rax
	movq	%rax, -48(%rbp)
	.p2align 5
.Lrotate.5:
	movq	-48(%rbp), %rax
	leaq	points+56(%rip), %rcx
	cmpq	%rcx, %rax
	jae	.Lrotate.7
	movq	-48(%rbp), %rax
	movq	-48(%rbp), %rcx
	movl	8(%rcx), %edx
	movl	%edx, (%rax)
	movl	12(%rcx), %edx
	movl	%edx, 4(%rax)
	addq	$8, -48(%rbp)
	jmp	.Lrotate.5
.Lrotate.7:
	leaq	spare(%rip), %rax
	leaq	nines(%rip), %rcx
	movq	$-72, %rsi
.Lrotate.8:
	movq	72(%rcx,%rsi,1), %rdx
	movq	%rdx, 72(%rax,%rsi,1)
	addq	$8, %rsi
	jne	.Lrotate.8
	movl	$0, -52(%rbp)
	.p2align 5
.Lrotate.9:
	cmpl	$3, -52(%rbp)
	jge	.Lrotate.11
	leaq	nines(%rip), %rax
	movslq	-52(%rbp), %rcx
	imulq	$72, %rcx, %rcx
	leaq	nines(%rip), %rdx
	movl	-52(%rbp), %esi
	addl	$1, %esi
	movslq	%esi, %rsi
	imulq	$72, %rsi, %rsi
	addq	%rcx, %rax
	addq	%rsi, %rdx
	movq	$-72, %rsi
.Lrotate.12:
	movq	72(%rdx,%rsi,1), %rcx
	movq	%rcx, 72(%rax,%rsi,1)
	addq	$8, %rsi
	jne	.Lrotate.12
	addl	$1, -52(%rbp)
	jmp	.Lrotate.9
.Lrotate.11:
	leaq	nines+216(%rip), %rax
	leaq	spare(%rip), %rcx
	movq	$-72, %rsi
.Lrotate.13:
	movq	72(%rcx,%rsi,1), %rdx
	movq	%rdx, 72(%rax,%rsi,1)
	addq	$8, %rsi
	jne	.Lrotate.13
	leaq	texts+96(%rip), %rax
	movq	$-32, %rdx
.Lrotate.14:
	movzbl	32(%rax,%rdx,1), %ecx
	movb	%cl, -4(%rbp,%rdx,1)
	addq	$1, %rdx
	jne	.Lrotate.14
	movl	$3, -52(%rbp)
	.p2align 5
.Lrotate.15:
	cmpl	$0, -52(%rbp)
	jle	.Lrotate.17
	leaq	texts(%rip), %rax
	movslq	-52(%rbp), %rcx
	imulq	$32, %rcx, %rcx
	leaq	texts(%rip), %rdx
	movl	-52(%rbp), %esi
	subl	$1, %esi
	movslq	%esi, %rsi
	imulq	$32, %rsi, %rsi
	addq	%rcx, %rax
	addq	%rsi, %rdx
	movq	$-32, %rsi
.Lrotate.18:
	movzbl	32(%rdx,%rsi,1), %ecx
	movb	%cl, 32(%rax,%rsi,1)
	addq	$1, %rsi
	jne	.Lrotate.18
	subl	$1, -52(%rbp)
	jmp	.Lrotate.15
.Lrotate.17:
	leaq	texts(%rip), %rax
	movq	$-32, %rdx
.Lrotate.19:
	movzbl	-4(%rbp,%rdx,1), %ecx
	movb	%cl, 32(%rax,%rdx,1)
	addq	$1, %rdx
	jne	.Lrotate.19
	leaq	codes(%rip), %rax
	movq	%rax, -64(%rbp)
	movl	$2, -68(%rbp)
	movq	-64(%rbp), %rdi
	movl	-68(%rbp), %esi
	call	at@PLT
	movzwl	(%rax), %ecx
	movw	%cx, -4(%rbp)
	movzwl	2(%rax), %ecx
	movw	%cx, -2(%rbp)
	leaq	codes(%rip), %rax
	movq	%rax, -80(%rbp)
	movl	$2, -84(%rbp)
	movq	-80(%rbp), %rdi
	movl	-84(%rbp), %esi
	call	at@PLT
	movq	%rax, -96(%rbp)
	leaq	codes(%rip), %rax
	movq	%rax, -104(%rbp)
	movl	$5, -108(%rbp)
	movq	-104(%rbp), %rdi
	movl	-108(%rbp), %esi
	call	at@PLT
	movq	-96(%rbp), %rcx
	movzwl	(%rax), %edx
	movw	%dx, (%rcx)
	movzwl	2(%rax), %edx
	movw	%dx, 2(%rcx)
	leaq	codes(%rip), %rax
	movq	%rax, -120(%rbp)
	movl	$5, -124(%rbp)
	movq	-120(%rbp), %rdi
	movl	-124(%rbp), %esi
	call	at@PLT
	movzwl	-4(%rbp), %ecx
	movw	%cx, (%rax)
	movzwl	-2(%rbp), %ecx
	movw	%cx, 2(%rax)
.Lrotate.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	rotate, .-rotate
	.globl	sum
	.type	sum, @function
sum:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	$0, -8(%rbp)
	movl	$0, -12(%rbp)
	.p2align 5
.Lsum.2:
	cmpl	$8, -12(%rbp)
	jge	.Lsum.4
	imulq	$31, -8(%rbp), %rax
	leaq	codes(%rip), %rcx
	movslq	-12(%rbp), %rdx
	movzbl	(%rcx,%rdx,4), %ecx
	addq	%rcx, %rax
	leaq	codes(%rip), %rcx
	movslq	-12(%rbp), %rdx
	movzbl	1(%rcx,%rdx,4), %ecx
	imull	$3, %ecx, %ecx
	movslq	%ecx, %rcx
	addq	%rcx, %rax
	leaq	codes(%rip), %rcx
	movslq	-12(%rbp), %rdx
	movzwl	2(%rcx,%rdx,4), %ecx
	imull	$7, %ecx, %ecx
	movslq	%ecx, %rcx
	addq	%rcx, %rax
	leaq	points(%rip), %rcx
	movslq	-12(%rbp), %rdx
	movslq	(%rcx,%rdx,8), %rcx
	imulq	$5, %rcx, %rcx
	addq	%rcx, %rax
	leaq	points(%rip), %rcx
	movslq	-12(%rbp), %rdx
	movslq	4(%rcx,%rdx,8), %rcx
	addq	%rcx, %rax
	movq	%rax, -8(%rbp)
	addl	$1, -12(%rbp)
	jmp	.Lsum.2
.Lsum.4:
	movl	$0, -12(%rbp)
	.p2align 5
.Lsum.5:
	cmpl	$4, -12(%rbp)
	jge	.Lsum.7
	movl	$0, -16(%rbp)
	.p2align 5
.Lsum.8:
	cmpl	$32, -16(%rbp)
	jge	.Lsum.10
	imulq	$31, -8(%rbp), %rax
	leaq	texts(%rip), %rcx
	movslq	-12(%rbp), %rdx
	imulq	$32, %rdx, %rdx
	addq	%rdx, %rcx
	movslq	-16(%rbp), %rdx
	movzbl	(%rcx,%rdx,1), %ecx
	addq	%rcx, %rax
	movq	%rax, -8(%rbp)
	addl	$1, -16(%rbp)
	jmp	.Lsum.8
.Lsum.10:
	movl	$0, -16(%rbp)
	.p2align 5
.Lsum.11:
	cmpl	$9, -16(%rbp)
	jge	.Lsum.13
	imulq	$31, -8(%rbp), %rax
	leaq	nines(%rip), %rcx
	movslq	-12(%rbp), %rdx
	imulq	$72, %rdx, %rdx
	addq	%rdx, %rcx
	movslq	-16(%rbp), %rdx
	addq	(%rcx,%rdx,8), %rax
	movq	%rax, -8(%rbp)
	addl	$1, -16(%rbp)
	jmp	.Lsum.11
.Lsum.13:
	addl	$1, -12(%rbp)
	jmp	.Lsum.5
.Lsum.7:
	movq	-8(%rbp), %rax
	jmp	.Lsum.1
.Lsum.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	sum, .-sum
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	call	fill@PLT
	call	rotate@PLT
	call	rotate@PLT
	call	sum@PLT
	movq	$251, %rcx
	xorl	%edx, %edx
	divq	%rcx
	movl	%edx, %eax
	jmp	.Lmain.1
.Lmain.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	main, .-main
	.bss
	.globl	codes
	.type	codes, @object
	.size	codes, 32
	.p2align 4
codes:
	.zero	32
	.globl	points
	.type	points, @object
	.size	points, 64
	.p2align 4
points:
	.zero	64
	.globl	texts
	.type	texts, @object
	.size	texts, 128
	.p2align 4
texts:
	.zero	128
	.globl	nines
	.type	nines, @object
	.size	nines, 288
	.p2align 4
nines:
	.zero	288
	.globl	spare
	.type	spare, @object
	.size	spare, 72
	.p2align 3
spare:
	.zero	72
	.section	.note.GNU-stack,"",@progbits
