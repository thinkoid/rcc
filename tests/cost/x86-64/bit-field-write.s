	.file	"bit-field-write.c"
	.text
	.globl	set_used
	.type	set_used, @function
set_used:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	orb	$8, 7(%rax)
.Lset_used.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	set_used, .-set_used
	.globl	clear_changed
	.type	clear_changed, @function
clear_changed:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	andb	$-17, 7(%rax)
.Lclear_changed.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	clear_changed, .-clear_changed
	.globl	clear_f
	.type	clear_f, @function
clear_f:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	andb	$7, (%rax)
.Lclear_f.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	clear_f, .-clear_f
	.globl	fill_f
	.type	fill_f, @function
fill_f:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	orb	$-8, (%rax)
.Lfill_f.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	fill_f, .-fill_f
	.globl	clear_u
	.type	clear_u, @function
clear_u:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	andb	$-128, 1(%rax)
.Lclear_u.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	clear_u, .-clear_u
	.globl	fill_u
	.type	fill_u, @function
fill_u:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	orb	$127, 1(%rax)
.Lfill_u.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	fill_u, .-fill_u
	.globl	five
	.type	five, @function
five:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	-8(%rbp), %rcx
	movsbl	(%rcx), %ecx
	andl	$-249, %ecx
	orl	$40, %ecx
	movb	%cl, (%rax)
.Lfive.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	five, .-five
	.globl	store
	.type	store, @function
store:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rax
	movl	-12(%rbp), %ecx
	shll	$3, %ecx
	movq	-8(%rbp), %rdx
	movsbl	(%rdx), %edx
	andl	$-249, %edx
	orl	%edx, %ecx
	movb	%cl, (%rax)
.Lstore.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	store, .-store
	.globl	store_bool
	.type	store_bool, @function
store_bool:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rax
	cmpl	$0, -12(%rbp)
	setne	%cl
	movzbl	%cl, %ecx
	shll	$7, %ecx
	movq	-8(%rbp), %rdx
	movzbl	1(%rdx), %edx
	andl	$-129, %edx
	orl	%edx, %ecx
	movb	%cl, 1(%rax)
.Lstore_bool.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	store_bool, .-store_bool
	.globl	stored
	.type	stored, @function
stored:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rcx
	movl	-12(%rbp), %eax
	shll	$3, %eax
	movq	-8(%rbp), %rdx
	movsbl	(%rdx), %edx
	andl	$-249, %edx
	orl	%edx, %eax
	movb	%al, (%rcx)
	movsbl	%al, %eax
	sarl	$3, %eax
	jmp	.Lstored.1
.Lstored.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	stored, .-stored
	.globl	mark
	.type	mark, @function
mark:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rax
	movl	-12(%rbp), %ecx
	movl	%ecx, %edx
	addl	$1, %ecx
	movl	%ecx, -12(%rbp)
	movslq	%edx, %rdx
	imulq	$16, %rdx, %rdx
	orb	$8, 7(%rax,%rdx,1)
.Lmark.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	mark, .-mark
	.section	.note.GNU-stack,"",@progbits
