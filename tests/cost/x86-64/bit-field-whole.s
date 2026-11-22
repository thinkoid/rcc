	.file	"bit-field-whole.c"
	.text
	.globl	done
	.type	done, @function
done:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movzbl	6(%rax), %eax
	cmpl	$2, %eax
	sete	%al
	movzbl	%al, %eax
	jmp	.Ldone.1
.Ldone.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	done, .-done
	.globl	next
	.type	next, @function
next:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rax
	movq	-16(%rbp), %rcx
	movzwl	4(%rcx), %edx
	movl	%edx, %esi
	addl	$1, %edx
	movw	%dx, 4(%rcx)
	movl	(%rax,%rsi,4), %eax
	jmp	.Lnext.1
.Lnext.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	next, .-next
	.globl	set_state
	.type	set_state, @function
set_state:
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
	movb	%cl, 6(%rax)
.Lset_state.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	set_state, .-set_state
	.globl	add
	.type	add, @function
add:
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
	addw	%cx, 4(%rax)
.Ladd.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	add, .-add
	.globl	sum
	.type	sum, @function
sum:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movsbl	(%rax), %eax
	movq	-8(%rbp), %rcx
	movswl	2(%rcx), %ecx
	addl	%ecx, %eax
	movslq	%eax, %rax
	movq	-8(%rbp), %rcx
	addq	8(%rcx), %rax
	jmp	.Lsum.1
.Lsum.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	sum, .-sum
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
	subw	$1, 2(%rax)
	movq	-8(%rbp), %rax
	imulq	$3, 8(%rax), %rcx
	movq	%rcx, 8(%rax)
.Lstep.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	step, .-step
	.section	.note.GNU-stack,"",@progbits
