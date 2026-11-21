	.file	"parameter-size-entry.c"
	.text
	.globl	mark
	.type	mark, @function
mark:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	leaq	order(%rip), %rax
	movl	marks(%rip), %ecx
	movl	%ecx, %edx
	addl	$1, %ecx
	movl	%ecx, marks(%rip)
	movslq	%edx, %rdx
	movl	-4(%rbp), %ecx
	movl	%ecx, (%rax,%rdx,4)
	movl	-4(%rbp), %eax
	jmp	.Lmark.1
.Lmark.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	mark, .-mark
	.globl	in_order
	.type	in_order, @function
in_order:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	marks(%rip), %eax
	cmpl	-4(%rbp), %eax
	je	.Lin_order.2
	movl	$0, %eax
	jmp	.Lin_order.1
.Lin_order.2:
	movl	$0, -8(%rbp)
	.p2align 5
.Lin_order.3:
	movl	-8(%rbp), %eax
	cmpl	-4(%rbp), %eax
	jge	.Lin_order.5
	leaq	order(%rip), %rax
	movslq	-8(%rbp), %rcx
	movl	(%rax,%rcx,4), %eax
	movl	-8(%rbp), %ecx
	addl	$1, %ecx
	cmpl	%ecx, %eax
	je	.Lin_order.6
	movl	$0, %eax
	jmp	.Lin_order.1
.Lin_order.6:
	addl	$1, -8(%rbp)
	jmp	.Lin_order.3
.Lin_order.5:
	movl	$0, marks(%rip)
	movl	$1, %eax
	jmp	.Lin_order.1
.Lin_order.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	in_order, .-in_order
	.globl	stepped
	.type	stepped, @function
stepped:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	addl	$1, -4(%rbp)
	movl	-4(%rbp), %eax
	jmp	.Lstepped.1
.Lstepped.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	stepped, .-stepped
	.globl	qualified
	.type	qualified, @function
qualified:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	addl	$1, -4(%rbp)
	imull	$10, -4(%rbp), %eax
	movq	-16(%rbp), %rcx
	addl	(%rcx), %eax
	jmp	.Lqualified.1
.Lqualified.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	qualified, .-qualified
	.globl	parenthesized
	.type	parenthesized, @function
parenthesized:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	addl	$2, -4(%rbp)
	movl	-4(%rbp), %eax
	jmp	.Lparenthesized.1
.Lparenthesized.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	parenthesized, .-parenthesized
	.globl	two
	.type	two, @function
two:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movl	$1, -20(%rbp)
	movl	-20(%rbp), %edi
	call	mark@PLT
	movl	$2, -24(%rbp)
	movl	-24(%rbp), %edi
	call	mark@PLT
	movl	$3, -28(%rbp)
	movl	-28(%rbp), %edi
	call	mark@PLT
	jmp	.Ltwo.1
.Ltwo.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	two, .-two
	.globl	in_register
	.type	in_register, @function
in_register:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movl	%edi, %ebx
	movq	%rsi, -16(%rbp)
	subl	$1, %ebx
	movl	%ebx, %eax
	jmp	.Lin_register.1
.Lin_register.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	in_register, .-in_register
	.type	inline_size, @function
inline_size:
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
	movl	%eax, -20(%rbp)
	movl	-20(%rbp), %edi
	call	mark@PLT
	movl	-4(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -24(%rbp)
	movl	-24(%rbp), %edi
	call	mark@PLT
	jmp	.Linline_size.1
.Linline_size.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	inline_size, .-inline_size
	.globl	constant
	.type	constant, @function
constant:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	movl	-4(%rbp), %eax
	movq	-16(%rbp), %rcx
	addl	8(%rcx), %eax
	jmp	.Lconstant.1
.Lconstant.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	constant, .-constant
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$128, %rsp
	movl	$0, -16(%rbp)
	movl	$0, -12(%rbp)
	movl	$0, -8(%rbp)
	movl	$0, -4(%rbp)
	movl	$7, -16(%rbp)
	movl	$0, -12(%rbp)
	movl	$5, -8(%rbp)
	movl	$0, -4(%rbp)
	movl	$3, -20(%rbp)
	leaq	-16(%rbp), %rax
	movq	%rax, -32(%rbp)
	movl	-20(%rbp), %edi
	movq	-32(%rbp), %rsi
	call	stepped@PLT
	cmpl	$4, %eax
	je	.Lmain.2
	movl	$101, %eax
	jmp	.Lmain.1
.Lmain.2:
	movl	$3, -36(%rbp)
	leaq	-16(%rbp), %rax
	movq	%rax, -48(%rbp)
	movl	-36(%rbp), %edi
	movq	-48(%rbp), %rsi
	call	qualified@PLT
	cmpl	$47, %eax
	je	.Lmain.3
	movl	$102, %eax
	jmp	.Lmain.1
.Lmain.3:
	movl	$3, -52(%rbp)
	leaq	-16(%rbp), %rax
	movq	%rax, -64(%rbp)
	movl	-52(%rbp), %edi
	movq	-64(%rbp), %rsi
	call	parenthesized@PLT
	cmpl	$5, %eax
	je	.Lmain.4
	movl	$103, %eax
	jmp	.Lmain.1
.Lmain.4:
	leaq	-16(%rbp), %rax
	movq	%rax, -72(%rbp)
	leaq	-16(%rbp), %rax
	movq	%rax, -80(%rbp)
	movq	-72(%rbp), %rdi
	movq	-80(%rbp), %rsi
	call	two@PLT
	cmpl	$3, %eax
	jne	.Lmain.6
	movl	$3, -84(%rbp)
	movl	-84(%rbp), %edi
	call	in_order@PLT
	cmpl	$0, %eax
	jne	.Lmain.5
.Lmain.6:
	movl	$104, %eax
	jmp	.Lmain.1
.Lmain.5:
	movl	$9, -88(%rbp)
	leaq	-16(%rbp), %rax
	movq	%rax, -96(%rbp)
	movl	-88(%rbp), %edi
	movq	-96(%rbp), %rsi
	call	in_register@PLT
	cmpl	$8, %eax
	je	.Lmain.7
	movl	$105, %eax
	jmp	.Lmain.1
.Lmain.7:
	movl	$1, -100(%rbp)
	leaq	-16(%rbp), %rax
	movq	%rax, -112(%rbp)
	movl	-100(%rbp), %edi
	movq	-112(%rbp), %rsi
	call	inline_size@PLT
	cmpl	$2, %eax
	jne	.Lmain.9
	movl	$2, -116(%rbp)
	movl	-116(%rbp), %edi
	call	in_order@PLT
	cmpl	$0, %eax
	jne	.Lmain.8
.Lmain.9:
	movl	$106, %eax
	jmp	.Lmain.1
.Lmain.8:
	movl	$1, -120(%rbp)
	leaq	-16(%rbp), %rax
	movq	%rax, -128(%rbp)
	movl	-120(%rbp), %edi
	movq	-128(%rbp), %rsi
	call	constant@PLT
	cmpl	$6, %eax
	jne	.Lmain.11
	cmpl	$0, marks(%rip)
	je	.Lmain.10
.Lmain.11:
	movl	$107, %eax
	jmp	.Lmain.1
.Lmain.10:
	movl	$42, %eax
	jmp	.Lmain.1
.Lmain.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	main, .-main
	.bss
	.globl	order
	.type	order, @object
	.size	order, 64
	.p2align 4
order:
	.zero	64
	.globl	marks
	.type	marks, @object
	.size	marks, 4
	.p2align 2
marks:
	.zero	4
	.section	.note.GNU-stack,"",@progbits
