	.file	"compare.c"
	.text
	.globl	less
	.type	less, @function
less:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	cmpl	-8(%rbp), %eax
	setl	%al
	movzbl	%al, %eax
	jmp	.Lless.1
.Lless.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	less, .-less
	.globl	equal
	.type	equal, @function
equal:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	cmpl	-8(%rbp), %eax
	sete	%al
	movzbl	%al, %eax
	jmp	.Lequal.1
.Lequal.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	equal, .-equal
	.globl	less_long
	.type	less_long, @function
less_long:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	cmpl	-8(%rbp), %eax
	setl	%al
	movzbl	%al, %eax
	jmp	.Lless_long.1
.Lless_long.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	less_long, .-less_long
	.globl	less_unsigned_long
	.type	less_unsigned_long, @function
less_unsigned_long:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	cmpl	-8(%rbp), %eax
	setl	%al
	movzbl	%al, %eax
	jmp	.Lless_unsigned_long.1
.Lless_unsigned_long.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	less_unsigned_long, .-less_unsigned_long
	.globl	not_long
	.type	not_long, @function
not_long:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	cmpl	$0, %eax
	sete	%al
	movzbl	%al, %eax
	jmp	.Lnot_long.1
.Lnot_long.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	not_long, .-not_long
	.globl	both_long
	.type	both_long, @function
both_long:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	cmpl	$0, -4(%rbp)
	je	.Lboth_long.2
	cmpl	$0, -8(%rbp)
	je	.Lboth_long.2
	movl	$1, %eax
	jmp	.Lboth_long.3
.Lboth_long.2:
	movl	$0, %eax
.Lboth_long.3:
	jmp	.Lboth_long.1
.Lboth_long.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	both_long, .-both_long
	.globl	less_register
	.type	less_register, @function
less_register:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movl	%edi, -12(%rbp)
	movl	%esi, -16(%rbp)
	movl	-12(%rbp), %eax
	cmpl	-16(%rbp), %eax
	setl	%bl
	movzbl	%bl, %ebx
	movq	%rbx, %rax
	jmp	.Lless_register.1
.Lless_register.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	less_register, .-less_register
	.globl	not_register
	.type	not_register, @function
not_register:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movl	%edi, -12(%rbp)
	movl	-12(%rbp), %eax
	cmpl	$0, %eax
	sete	%al
	movzbl	%al, %eax
	movq	%rax, %rbx
	movq	%rbx, %rax
	jmp	.Lnot_register.1
.Lnot_register.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	not_register, .-not_register
	.globl	both_register
	.type	both_register, @function
both_register:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movl	%edi, -12(%rbp)
	movl	%esi, -16(%rbp)
	cmpl	$0, -12(%rbp)
	je	.Lboth_register.2
	cmpl	$0, -16(%rbp)
	je	.Lboth_register.2
	movl	$1, %eax
	jmp	.Lboth_register.3
.Lboth_register.2:
	movl	$0, %eax
.Lboth_register.3:
	movq	%rax, %rbx
	movq	%rbx, %rax
	jmp	.Lboth_register.1
.Lboth_register.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	both_register, .-both_register
	.section	.note.GNU-stack,"",@progbits
