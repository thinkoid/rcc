	.file	"statement-expression-order.c"
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
	.globl	three
	.type	three, @function
three:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	%edx, -12(%rbp)
	imull	$100, -4(%rbp), %eax
	imull	$10, -8(%rbp), %ecx
	addl	%ecx, %eax
	addl	-12(%rbp), %eax
	jmp	.Lthree.1
.Lthree.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	three, .-three
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
	movl	$1, -24(%rbp)
	movl	-24(%rbp), %edi
	call	mark@PLT
	movq	%rax, -32(%rbp)
	movl	$2, -36(%rbp)
	movl	-36(%rbp), %edi
	call	mark@PLT
	movl	$3, %eax
	addl	-32(%rbp), %eax
	movq	%rax, -48(%rbp)
	movl	$3, -52(%rbp)
	movl	-52(%rbp), %edi
	call	mark@PLT
	addl	-48(%rbp), %eax
	movl	%eax, -20(%rbp)
	movl	$3, -56(%rbp)
	movl	-56(%rbp), %edi
	call	in_order@PLT
	cmpl	$0, %eax
	je	.Lmain.3
	cmpl	$7, -20(%rbp)
	je	.Lmain.2
.Lmain.3:
	movl	$101, %eax
	jmp	.Lmain.1
.Lmain.2:
	movl	$1, -60(%rbp)
	movl	-60(%rbp), %edi
	call	mark@PLT
	imull	$10, %eax, %eax
	movq	%rax, -72(%rbp)
	movl	$2, -80(%rbp)
	movl	-80(%rbp), %edi
	call	mark@PLT
	movl	%eax, -76(%rbp)
	movl	$3, -84(%rbp)
	movl	-84(%rbp), %edi
	call	mark@PLT
	movl	-76(%rbp), %eax
	addl	-72(%rbp), %eax
	movl	%eax, -20(%rbp)
	movl	$3, -88(%rbp)
	movl	-88(%rbp), %edi
	call	in_order@PLT
	cmpl	$0, %eax
	je	.Lmain.5
	cmpl	$12, -20(%rbp)
	je	.Lmain.4
.Lmain.5:
	movl	$102, %eax
	jmp	.Lmain.1
.Lmain.4:
	movl	$1, -96(%rbp)
	movl	-96(%rbp), %edi
	call	mark@PLT
	movl	%eax, -92(%rbp)
	movl	$2, -104(%rbp)
	movl	-104(%rbp), %edi
	call	mark@PLT
	movl	$5, %eax
	movl	%eax, -100(%rbp)
	movl	$3, -112(%rbp)
	movl	-112(%rbp), %edi
	call	mark@PLT
	movl	%eax, -108(%rbp)
	movl	-92(%rbp), %edi
	movl	-100(%rbp), %esi
	movl	-108(%rbp), %edx
	call	three@PLT
	movl	%eax, -20(%rbp)
	movl	$3, -116(%rbp)
	movl	-116(%rbp), %edi
	call	in_order@PLT
	cmpl	$0, %eax
	je	.Lmain.7
	cmpl	$153, -20(%rbp)
	je	.Lmain.6
.Lmain.7:
	movl	$103, %eax
	jmp	.Lmain.1
.Lmain.6:
	movl	$1, -120(%rbp)
	movl	-120(%rbp), %edi
	call	mark@PLT
	movq	%rax, -128(%rbp)
	movl	$2, -136(%rbp)
	movl	-136(%rbp), %edi
	call	mark@PLT
	movq	%rax, -144(%rbp)
	movl	$3, -148(%rbp)
	movl	-148(%rbp), %edi
	call	mark@PLT
	movl	$4, %eax
	addl	-144(%rbp), %eax
	movl	%eax, -132(%rbp)
	movl	$4, -152(%rbp)
	movl	-152(%rbp), %edi
	call	mark@PLT
	movl	-132(%rbp), %eax
	addl	-128(%rbp), %eax
	movl	%eax, -20(%rbp)
	movl	$4, -156(%rbp)
	movl	-156(%rbp), %edi
	call	in_order@PLT
	cmpl	$0, %eax
	je	.Lmain.9
	cmpl	$7, -20(%rbp)
	je	.Lmain.8
.Lmain.9:
	movl	$104, %eax
	jmp	.Lmain.1
.Lmain.8:
	movl	$1, -160(%rbp)
	movl	-160(%rbp), %edi
	call	mark@PLT
	movslq	%eax, %rax
	movq	%rax, -168(%rbp)
	movl	$2, -172(%rbp)
	movl	-172(%rbp), %edi
	call	mark@PLT
	movl	$9, %eax
	movq	-168(%rbp), %rcx
	movl	%eax, -16(%rbp,%rcx,4)
	movl	$2, -176(%rbp)
	movl	-176(%rbp), %edi
	call	in_order@PLT
	cmpl	$0, %eax
	je	.Lmain.11
	cmpl	$9, -12(%rbp)
	je	.Lmain.10
.Lmain.11:
	movl	$105, %eax
	jmp	.Lmain.1
.Lmain.10:
	movl	$1, -180(%rbp)
	movl	-180(%rbp), %edi
	call	mark@PLT
	cmpl	$0, %eax
	je	.Lmain.12
	movl	$2, -184(%rbp)
	movl	-184(%rbp), %edi
	call	mark@PLT
	movl	$0, %eax
	cmpl	$0, %eax
	je	.Lmain.12
	movl	$3, -188(%rbp)
	movl	-188(%rbp), %edi
	call	mark@PLT
	cmpl	$0, %eax
	je	.Lmain.12
	movl	$1, %eax
	jmp	.Lmain.13
.Lmain.12:
	movl	$0, %eax
.Lmain.13:
	movl	%eax, -20(%rbp)
	movl	$2, -192(%rbp)
	movl	-192(%rbp), %edi
	call	in_order@PLT
	cmpl	$0, %eax
	je	.Lmain.15
	cmpl	$0, -20(%rbp)
	je	.Lmain.14
.Lmain.15:
	movl	$106, %eax
	jmp	.Lmain.1
.Lmain.14:
	movl	$1, -196(%rbp)
	movl	-196(%rbp), %edi
	call	mark@PLT
	cmpl	$0, %eax
	je	.Lmain.16
	movl	$2, -200(%rbp)
	movl	-200(%rbp), %edi
	call	mark@PLT
	movl	$6, %eax
	jmp	.Lmain.17
.Lmain.16:
	movl	$9, -204(%rbp)
	movl	-204(%rbp), %edi
	call	mark@PLT
.Lmain.17:
	movl	%eax, -20(%rbp)
	movl	$2, -208(%rbp)
	movl	-208(%rbp), %edi
	call	in_order@PLT
	cmpl	$0, %eax
	je	.Lmain.19
	cmpl	$6, -20(%rbp)
	je	.Lmain.18
.Lmain.19:
	movl	$107, %eax
	jmp	.Lmain.1
.Lmain.18:
	movl	$1, -212(%rbp)
	movl	-212(%rbp), %edi
	call	mark@PLT
	movl	$2, -216(%rbp)
	movl	-216(%rbp), %edi
	call	mark@PLT
	cmpl	$2, %eax
	jne	.Lmain.20
	movl	$3, -220(%rbp)
	movl	-220(%rbp), %edi
	call	mark@PLT
	movl	%eax, -20(%rbp)
.Lmain.20:
	movl	$3, -224(%rbp)
	movl	-224(%rbp), %edi
	call	in_order@PLT
	cmpl	$0, %eax
	je	.Lmain.22
	cmpl	$3, -20(%rbp)
	je	.Lmain.21
.Lmain.22:
	movl	$108, %eax
	jmp	.Lmain.1
.Lmain.21:
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
