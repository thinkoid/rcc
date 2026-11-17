	.file	"static.c"
	.text
	.type	hidden, @function
hidden:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	addl	$1, %eax
	jmp	.Lhidden.1
.Lhidden.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	hidden, .-hidden
	.globl	shown
	.type	shown, @function
shown:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	addl	$1, %eax
	jmp	.Lshown.1
.Lshown.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	shown, .-shown
	.globl	calls
	.type	calls, @function
calls:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$48, %rsp
	movl	%edi, -4(%rbp)
	leaq	hidden(%rip), %rax
	movq	%rax, -16(%rbp)
	leaq	shown(%rip), %rax
	movq	%rax, -24(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -28(%rbp)
	movl	-28(%rbp), %edi
	call	hidden@PLT
	movq	%rax, -40(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -44(%rbp)
	movl	-44(%rbp), %edi
	call	shown@PLT
	movq	-40(%rbp), %rcx
	addl	%eax, %ecx
	movslq	%ecx, %rax
	addq	-16(%rbp), %rax
	subq	-24(%rbp), %rax
	jmp	.Lcalls.1
.Lcalls.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	calls, .-calls
	.globl	get_quiet
	.type	get_quiet, @function
get_quiet:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	quiet(%rip), %eax
	jmp	.Lget_quiet.1
.Lget_quiet.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	get_quiet, .-get_quiet
	.globl	get_loud
	.type	get_loud, @function
get_loud:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	loud(%rip), %eax
	jmp	.Lget_loud.1
.Lget_loud.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	get_loud, .-get_loud
	.globl	next
	.type	next, @function
next:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	next.count(%rip), %eax
	addl	$1, %eax
	movl	%eax, next.count(%rip)
	jmp	.Lnext.1
.Lnext.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	next, .-next
	.globl	next_total
	.type	next_total, @function
next_total:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	total(%rip), %eax
	addl	$1, %eax
	movl	%eax, total(%rip)
	jmp	.Lnext_total.1
.Lnext_total.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	next_total, .-next_total
	.bss
	.type	quiet, @object
	.size	quiet, 4
	.p2align 2
quiet:
	.zero	4
	.globl	loud
	.type	loud, @object
	.size	loud, 4
	.p2align 2
loud:
	.zero	4
	.globl	total
	.type	total, @object
	.size	total, 4
	.p2align 2
total:
	.zero	4
	.type	next.count, @object
	.size	next.count, 4
	.p2align 2
next.count:
	.zero	4
	.section	.note.GNU-stack,"",@progbits
