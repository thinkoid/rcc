	.file	"conditional-condition.c"
	.text
	.globl	joined
	.type	joined, @function
joined:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	%edx, -12(%rbp)
	cmpl	$0, -4(%rbp)
	je	.Ljoined.3
	movl	-8(%rbp), %eax
	jmp	.Ljoined.4
.Ljoined.3:
	movl	-12(%rbp), %eax
.Ljoined.4:
	cmpl	$0, %eax
	je	.Ljoined.2
	call	g@PLT
.Ljoined.2:
.Ljoined.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	joined, .-joined
	.globl	compared
	.type	compared, @function
compared:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	%edx, -12(%rbp)
	movl	%ecx, -16(%rbp)
	cmpl	$0, -4(%rbp)
	je	.Lcompared.3
	movl	-8(%rbp), %eax
	cmpl	-12(%rbp), %eax
	jge	.Lcompared.2
	jmp	.Lcompared.4
.Lcompared.3:
	cmpl	$0, -16(%rbp)
	je	.Lcompared.2
.Lcompared.4:
	call	g@PLT
.Lcompared.2:
.Lcompared.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	compared, .-compared
	.globl	called
	.type	called, @function
called:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	cmpl	$0, -4(%rbp)
	je	.Lcalled.3
	call	f@PLT
	jmp	.Lcalled.4
.Lcalled.3:
	movl	-8(%rbp), %eax
.Lcalled.4:
	cmpl	$0, %eax
	je	.Lcalled.2
	call	g@PLT
.Lcalled.2:
.Lcalled.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	called, .-called
	.globl	negated
	.type	negated, @function
negated:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	%edx, -12(%rbp)
	movl	%ecx, -16(%rbp)
	cmpl	$0, -4(%rbp)
	je	.Lnegated.3
	movl	-8(%rbp), %eax
	cmpl	-12(%rbp), %eax
	jl	.Lnegated.2
	jmp	.Lnegated.4
.Lnegated.3:
	cmpl	$0, -16(%rbp)
	jne	.Lnegated.2
.Lnegated.4:
	call	g@PLT
.Lnegated.2:
.Lnegated.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	negated, .-negated
	.globl	looped
	.type	looped, @function
looped:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	%edx, -12(%rbp)
	movl	%ecx, -16(%rbp)
	.p2align 5
.Llooped.2:
	cmpl	$0, -4(%rbp)
	je	.Llooped.4
	movl	-8(%rbp), %eax
	cmpl	-12(%rbp), %eax
	jge	.Llooped.3
	jmp	.Llooped.5
.Llooped.4:
	cmpl	$0, -16(%rbp)
	je	.Llooped.3
.Llooped.5:
	call	g@PLT
	jmp	.Llooped.2
.Llooped.3:
.Llooped.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	looped, .-looped
	.globl	both
	.type	both, @function
both:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	%edx, -12(%rbp)
	movl	%ecx, -16(%rbp)
	cmpl	$0, -4(%rbp)
	je	.Lboth.3
	movl	-8(%rbp), %eax
	cmpl	-12(%rbp), %eax
	jge	.Lboth.2
	jmp	.Lboth.4
.Lboth.3:
	movl	-8(%rbp), %eax
	cmpl	-16(%rbp), %eax
	jle	.Lboth.2
.Lboth.4:
	call	g@PLT
.Lboth.2:
.Lboth.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	both, .-both
	.globl	held
	.type	held, @function
held:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	%edx, -12(%rbp)
	movl	%ecx, -16(%rbp)
	movl	-16(%rbp), %eax
	movq	%rax, -24(%rbp)
	cmpl	$0, -4(%rbp)
	je	.Lheld.4
	movl	-8(%rbp), %ecx
	cmpl	-12(%rbp), %ecx
	jge	.Lheld.2
	jmp	.Lheld.5
.Lheld.4:
	cmpl	$0, -16(%rbp)
	je	.Lheld.2
.Lheld.5:
	movl	$1, %ecx
	jmp	.Lheld.3
.Lheld.2:
	movl	$2, %ecx
.Lheld.3:
	movq	-24(%rbp), %rax
	addl	%ecx, %eax
	jmp	.Lheld.1
.Lheld.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	held, .-held
	.globl	written
	.type	written, @function
written:
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
	movl	%edx, -20(%rbp)
	movl	-20(%rbp), %ebx
	movq	%rbx, -32(%rbp)
	cmpl	$0, -12(%rbp)
	je	.Lwritten.4
	movl	-16(%rbp), %ebx
	cmpl	-20(%rbp), %ebx
	jge	.Lwritten.2
	jmp	.Lwritten.5
.Lwritten.4:
	cmpl	$0, %ebx
	je	.Lwritten.2
.Lwritten.5:
	movl	$1, %ecx
	jmp	.Lwritten.3
.Lwritten.2:
	movl	$2, %ecx
.Lwritten.3:
	movq	-32(%rbp), %rax
	addl	%ecx, %eax
	jmp	.Lwritten.1
.Lwritten.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	written, .-written
	.globl	converted
	.type	converted, @function
converted:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	%edx, -12(%rbp)
	movl	%ecx, -16(%rbp)
	cmpl	$0, -4(%rbp)
	je	.Lconverted.3
	movl	-8(%rbp), %eax
	cmpl	-12(%rbp), %eax
	jge	.Lconverted.2
	jmp	.Lconverted.4
.Lconverted.3:
	cmpl	$0, -16(%rbp)
	je	.Lconverted.2
.Lconverted.4:
	call	g@PLT
.Lconverted.2:
.Lconverted.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	converted, .-converted
	.globl	narrow
	.type	narrow, @function
narrow:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	%edx, -12(%rbp)
	movb	%cl, -13(%rbp)
	cmpl	$0, -4(%rbp)
	je	.Lnarrow.3
	movl	-8(%rbp), %eax
	cmpl	-12(%rbp), %eax
	jge	.Lnarrow.2
	jmp	.Lnarrow.4
.Lnarrow.3:
	cmpb	$0, -13(%rbp)
	je	.Lnarrow.2
.Lnarrow.4:
	call	g@PLT
.Lnarrow.2:
.Lnarrow.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	narrow, .-narrow
	.globl	registers
	.type	registers, @function
registers:
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
	subq	$16, %rsp
	movl	%edi, -20(%rbp)
	movl	%esi, -24(%rbp)
	movl	%edx, -28(%rbp)
	movl	-24(%rbp), %ebx
	movl	-28(%rbp), %r12d
	cmpl	$0, -20(%rbp)
	je	.Lregisters.3
	movl	%ebx, %eax
	jmp	.Lregisters.4
.Lregisters.3:
	movl	%r12d, %eax
.Lregisters.4:
	cmpl	$0, %eax
	je	.Lregisters.2
	call	g@PLT
.Lregisters.2:
.Lregisters.1:
	leaq	-16(%rbp), %rsp
	popq	%r12
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	registers, .-registers
	.section	.note.GNU-stack,"",@progbits
