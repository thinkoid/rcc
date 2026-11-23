	.file	"bit-field-compound.c"
	.text
	.globl	bump
	.type	bump, @function
bump:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movzbl	(%rax), %eax
	movb	%al, -9(%rbp)
	movq	-8(%rbp), %rax
	movzbl	-9(%rbp), %ecx
	shrl	$4, %ecx
	addl	$2, %ecx
	shll	$4, %ecx
	movzbl	-9(%rbp), %edx
	andl	$-241, %edx
	orl	%edx, %ecx
	movb	%cl, (%rax)
.Lbump.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	bump, .-bump
	.globl	raised
	.type	raised, @function
raised:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movzbl	(%rax), %eax
	movb	%al, -9(%rbp)
	movq	-8(%rbp), %rcx
	movzbl	-9(%rbp), %eax
	shrl	$4, %eax
	addl	$2, %eax
	shll	$4, %eax
	movzbl	-9(%rbp), %edx
	andl	$-241, %edx
	orl	%edx, %eax
	movb	%al, (%rcx)
	movzbl	%al, %eax
	shrl	$4, %eax
	jmp	.Lraised.1
.Lraised.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	raised, .-raised
	.globl	step
	.type	step, @function
step:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movzbl	(%rax), %eax
	movb	%al, -9(%rbp)
	movq	-8(%rbp), %rax
	movzbl	-9(%rbp), %ecx
	shll	$28, %ecx
	shrl	$28, %ecx
	addl	$1, %ecx
	andl	$15, %ecx
	movzbl	-9(%rbp), %edx
	andl	$-16, %edx
	orl	%edx, %ecx
	movb	%cl, (%rax)
.Lstep.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	step, .-step
	.globl	stepped
	.type	stepped, @function
stepped:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movzbl	(%rax), %eax
	movb	%al, -9(%rbp)
	movq	-8(%rbp), %rax
	movzbl	-9(%rbp), %ecx
	shll	$28, %ecx
	shrl	$28, %ecx
	addl	$1, %ecx
	andl	$15, %ecx
	movzbl	-9(%rbp), %edx
	andl	$-16, %edx
	orl	%edx, %ecx
	movb	%cl, (%rax)
.Lstepped.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	stepped, .-stepped
	.globl	before
	.type	before, @function
before:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movzbl	(%rax), %eax
	movb	%al, -9(%rbp)
	movq	-8(%rbp), %rax
	movzbl	-9(%rbp), %ecx
	shll	$28, %ecx
	shrl	$28, %ecx
	addl	$1, %ecx
	andl	$15, %ecx
	movzbl	-9(%rbp), %edx
	andl	$-16, %edx
	orl	%edx, %ecx
	movb	%cl, (%rax)
	movzbl	-9(%rbp), %eax
	shll	$28, %eax
	shrl	$28, %eax
	jmp	.Lbefore.1
.Lbefore.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	before, .-before
	.globl	lowered
	.type	lowered, @function
lowered:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movsbl	1(%rax), %eax
	movb	%al, -9(%rbp)
	movq	-8(%rbp), %rcx
	movsbl	-9(%rbp), %eax
	shll	$27, %eax
	sarl	$27, %eax
	subl	$1, %eax
	andl	$31, %eax
	movsbl	-9(%rbp), %edx
	andl	$-32, %edx
	orl	%edx, %eax
	movb	%al, 1(%rcx)
	movsbl	%al, %eax
	shll	$27, %eax
	sarl	$27, %eax
	jmp	.Llowered.1
.Llowered.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	lowered, .-lowered
	.globl	toggle
	.type	toggle, @function
toggle:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movzbl	1(%rax), %eax
	movb	%al, -9(%rbp)
	movq	-8(%rbp), %rax
	movzbl	-9(%rbp), %ecx
	shll	$26, %ecx
	shrl	$31, %ecx
	xorl	$1, %ecx
	cmpl	$0, %ecx
	setne	%cl
	movzbl	%cl, %ecx
	shll	$5, %ecx
	andl	$32, %ecx
	movzbl	-9(%rbp), %edx
	andl	$-33, %edx
	orl	%edx, %ecx
	movb	%cl, 1(%rax)
.Ltoggle.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	toggle, .-toggle
	.globl	turned
	.type	turned, @function
turned:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movzbl	1(%rax), %eax
	movb	%al, -9(%rbp)
	movq	-8(%rbp), %rax
	movzbl	-9(%rbp), %ecx
	shll	$26, %ecx
	shrl	$31, %ecx
	subl	$1, %ecx
	cmpl	$0, %ecx
	setne	%cl
	movzbl	%cl, %ecx
	shll	$5, %ecx
	andl	$32, %ecx
	movzbl	-9(%rbp), %edx
	andl	$-33, %edx
	orl	%edx, %ecx
	movb	%cl, 1(%rax)
	movzbl	-9(%rbp), %eax
	shll	$26, %eax
	shrl	$31, %eax
	jmp	.Lturned.1
.Lturned.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	turned, .-turned
	.globl	ticked
	.type	ticked, @function
ticked:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	call	tick@PLT
	movl	%eax, -16(%rbp)
	movq	-8(%rbp), %rax
	movsbl	1(%rax), %eax
	movb	%al, -9(%rbp)
	movq	-8(%rbp), %rax
	movsbl	-9(%rbp), %ecx
	shll	$27, %ecx
	sarl	$27, %ecx
	addl	-16(%rbp), %ecx
	andl	$31, %ecx
	movsbl	-9(%rbp), %edx
	andl	$-32, %edx
	orl	%edx, %ecx
	movb	%cl, 1(%rax)
.Lticked.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	ticked, .-ticked
	.section	.note.GNU-stack,"",@progbits
