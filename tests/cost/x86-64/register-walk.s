	.file	"register-walk.c"
	.text
	.globl	copy
	.type	copy, @function
copy:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	pushq	%r12
	.cfi_offset %r12, -32
	pushq	%r13
	.cfi_offset %r13, -40
	movq	%rdi, %rbx
	movq	%rsi, %r12
	movq	%rbx, %r13
	.p2align 5
.Lcopy.2:
	movq	%rbx, %rax
	addq	$1, %rbx
	movq	%r12, %rcx
	addq	$1, %r12
	movsbl	(%rcx), %ecx
	movb	%cl, (%rax)
	cmpb	$0, %cl
	je	.Lcopy.3
	jmp	.Lcopy.2
.Lcopy.3:
	movq	%r13, %rax
	jmp	.Lcopy.1
.Lcopy.1:
	popq	%r13
	popq	%r12
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	copy, .-copy
	.globl	length
	.type	length, @function
length:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	pushq	%r12
	.cfi_offset %r12, -32
	movq	%rdi, %rbx
	movl	$0, %r12d
	.p2align 5
.Llength.2:
	movq	%rbx, %rax
	addq	$1, %rbx
	movsbl	(%rax), %eax
	cmpb	$0, %al
	je	.Llength.3
	addl	$1, %r12d
	jmp	.Llength.2
.Llength.3:
	movl	%r12d, %eax
	jmp	.Llength.1
.Llength.1:
	popq	%r12
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	length, .-length
	.globl	compare
	.type	compare, @function
compare:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	pushq	%r12
	.cfi_offset %r12, -32
	movq	%rdi, %rbx
	movq	%rsi, %r12
	.p2align 5
.Lcompare.2:
	movsbl	(%rbx), %eax
	movq	%r12, %rcx
	addq	$1, %r12
	movsbl	(%rcx), %ecx
	cmpl	%ecx, %eax
	jne	.Lcompare.3
	movq	%rbx, %rax
	addq	$1, %rbx
	movsbl	(%rax), %eax
	cmpl	$0, %eax
	jne	.Lcompare.4
	movl	$0, %eax
	jmp	.Lcompare.1
.Lcompare.4:
	jmp	.Lcompare.2
.Lcompare.3:
	movsbl	(%rbx), %eax
	subq	$1, %r12
	movsbl	(%r12), %ecx
	subl	%ecx, %eax
	jmp	.Lcompare.1
.Lcompare.1:
	popq	%r12
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	compare, .-compare
	.section	.note.GNU-stack,"",@progbits
