	.file	"header-grids.c"
	.text
	.globl	number
	.type	number, @function
number:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	count(%rip), %eax
	addl	$1, %eax
	movl	%eax, count(%rip)
	jmp	.Lnumber.1
.Lnumber.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	number, .-number
	.globl	fill
	.type	fill, @function
fill:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movl	$0, -4(%rbp)
	.p2align 5
.Lfill.2:
	cmpl	$4, -4(%rbp)
	jge	.Lfill.4
	movl	$0, -8(%rbp)
	.p2align 5
.Lfill.5:
	cmpl	$4, -8(%rbp)
	jge	.Lfill.7
	leaq	grid(%rip), %rax
	movslq	-4(%rbp), %rcx
	imulq	$16, %rcx, %rcx
	addq	%rcx, %rax
	movslq	-8(%rbp), %rcx
	movq	%rax, -16(%rbp)
	movq	%rcx, -24(%rbp)
	call	number@PLT
	movq	-24(%rbp), %rcx
	movq	-16(%rbp), %rdx
	movl	%eax, (%rdx,%rcx,4)
	addl	$1, -8(%rbp)
	jmp	.Lfill.5
.Lfill.7:
	addl	$1, -4(%rbp)
	jmp	.Lfill.2
.Lfill.4:
	movl	$0, -4(%rbp)
	.p2align 5
.Lfill.8:
	cmpl	$4, -4(%rbp)
	jge	.Lfill.10
	movl	$0, -8(%rbp)
	.p2align 5
.Lfill.11:
	cmpl	$4, -8(%rbp)
	jge	.Lfill.13
	leaq	grid(%rip), %rax
	movslq	-4(%rbp), %rcx
	imulq	$16, %rcx, %rcx
	addq	%rcx, %rax
	movslq	-8(%rbp), %rcx
	movl	(%rax,%rcx,4), %eax
	imull	$4, -4(%rbp), %ecx
	addl	-8(%rbp), %ecx
	addl	$1, %ecx
	cmpl	%ecx, %eax
	je	.Lfill.14
	movl	$1, %eax
	jmp	.Lfill.1
.Lfill.14:
	addl	$1, -8(%rbp)
	jmp	.Lfill.11
.Lfill.13:
	addl	$1, -4(%rbp)
	jmp	.Lfill.8
.Lfill.10:
	movl	$0, %eax
	jmp	.Lfill.1
.Lfill.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	fill, .-fill
	.globl	numbered
	.type	numbered, @function
numbered:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$96, %rsp
	movl	$0, -68(%rbp)
	.p2align 5
.Lnumbered.2:
	cmpl	$4, -68(%rbp)
	jge	.Lnumbered.4
	movl	$0, -72(%rbp)
	.p2align 5
.Lnumbered.5:
	cmpl	$4, -72(%rbp)
	jge	.Lnumbered.7
	leaq	-64(%rbp), %rax
	movslq	-68(%rbp), %rcx
	imulq	$16, %rcx, %rcx
	addq	%rcx, %rax
	movslq	-72(%rbp), %rcx
	leaq	(%rax,%rcx,4), %rax
	movq	%rax, -88(%rbp)
	call	number@PLT
	movq	-88(%rbp), %rcx
	movl	%eax, (%rcx)
	addl	$1, -72(%rbp)
	jmp	.Lnumbered.5
.Lnumbered.7:
	addl	$1, -68(%rbp)
	jmp	.Lnumbered.2
.Lnumbered.4:
	movl	$0, -76(%rbp)
	movl	$0, -68(%rbp)
	.p2align 5
.Lnumbered.8:
	cmpl	$4, -68(%rbp)
	jge	.Lnumbered.10
	movslq	-68(%rbp), %rax
	imulq	$16, %rax, %rax
	leaq	-64(%rbp,%rax,1), %rax
	movslq	-68(%rbp), %rcx
	movl	(%rax,%rcx,4), %eax
	addl	%eax, -76(%rbp)
	addl	$1, -68(%rbp)
	jmp	.Lnumbered.8
.Lnumbered.10:
	movl	-76(%rbp), %eax
	jmp	.Lnumbered.1
.Lnumbered.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	numbered, .-numbered
	.globl	layer
	.type	layer, @function
layer:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	leaq	cube(%rip), %rax
	movslq	-4(%rbp), %rcx
	imulq	$36, %rcx, %rcx
	addq	%rcx, %rax
	movslq	-8(%rbp), %rcx
	imulq	$12, %rcx, %rcx
	movl	8(%rax,%rcx,1), %eax
	jmp	.Llayer.1
.Llayer.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	layer, .-layer
	.globl	back
	.type	back, @function
back:
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
	imulq	$4, %rcx, %rcx
	subq	%rcx, %rax
	movl	16(%rax), %eax
	jmp	.Lback.1
.Lback.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	back, .-back
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$80, %rsp
	call	fill@PLT
	cmpl	$0, %eax
	je	.Lmain.2
	movl	$1, %eax
	jmp	.Lmain.1
.Lmain.2:
	call	numbered@PLT
	cmpl	$98, %eax
	je	.Lmain.3
	movl	$2, %eax
	jmp	.Lmain.1
.Lmain.3:
	movl	$0, -4(%rbp)
	.p2align 5
.Lmain.4:
	cmpl	$3, -4(%rbp)
	jge	.Lmain.6
	movl	$0, -8(%rbp)
	.p2align 5
.Lmain.7:
	cmpl	$3, -8(%rbp)
	jge	.Lmain.9
	movl	$0, -12(%rbp)
	.p2align 5
.Lmain.10:
	cmpl	$3, -12(%rbp)
	jge	.Lmain.12
	leaq	cube(%rip), %rax
	movslq	-4(%rbp), %rcx
	imulq	$36, %rcx, %rcx
	addq	%rcx, %rax
	movslq	-8(%rbp), %rcx
	imulq	$12, %rcx, %rcx
	addq	%rcx, %rax
	movslq	-12(%rbp), %rcx
	imull	$9, -4(%rbp), %edx
	imull	$3, -8(%rbp), %esi
	addl	%esi, %edx
	addl	-12(%rbp), %edx
	movl	%edx, (%rax,%rcx,4)
	addl	$1, -12(%rbp)
	jmp	.Lmain.10
.Lmain.12:
	addl	$1, -8(%rbp)
	jmp	.Lmain.7
.Lmain.9:
	addl	$1, -4(%rbp)
	jmp	.Lmain.4
.Lmain.6:
	movl	$2, -16(%rbp)
	movl	$1, -20(%rbp)
	movl	-16(%rbp), %edi
	movl	-20(%rbp), %esi
	call	layer@PLT
	cmpl	$23, %eax
	je	.Lmain.13
	movl	$3, %eax
	jmp	.Lmain.1
.Lmain.13:
	leaq	grid+32(%rip), %rax
	movq	%rax, -32(%rbp)
	movl	$2, -36(%rbp)
	movq	-32(%rbp), %rdi
	movl	-36(%rbp), %esi
	call	back@PLT
	cmpl	$11, %eax
	je	.Lmain.14
	movl	$4, %eax
	jmp	.Lmain.1
.Lmain.14:
	movl	grid+24(%rip), %eax
	movq	%rax, -48(%rbp)
	movl	$1, -52(%rbp)
	movl	$2, -56(%rbp)
	movl	-52(%rbp), %edi
	movl	-56(%rbp), %esi
	call	layer@PLT
	addl	-48(%rbp), %eax
	movq	%rax, -64(%rbp)
	leaq	grid+32(%rip), %rax
	movq	%rax, -72(%rbp)
	movl	$2, -76(%rbp)
	movq	-72(%rbp), %rdi
	movl	-76(%rbp), %esi
	call	back@PLT
	addl	-64(%rbp), %eax
	jmp	.Lmain.1
.Lmain.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	main, .-main
	.bss
	.globl	grid
	.type	grid, @object
	.size	grid, 64
	.p2align 4
grid:
	.zero	64
	.globl	cube
	.type	cube, @object
	.size	cube, 108
	.p2align 4
cube:
	.zero	108
	.globl	count
	.type	count, @object
	.size	count, 4
	.p2align 2
count:
	.zero	4
	.section	.note.GNU-stack,"",@progbits
