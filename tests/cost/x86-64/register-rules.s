	.file	"register-rules.c"
	.text
	.globl	compound_in_place
	.type	compound_in_place, @function
compound_in_place:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movl	%edi, -12(%rbp)
	movl	-12(%rbp), %ebx
	addl	$5, %ebx
	addl	$1, %ebx
	movl	%ebx, %eax
	jmp	.Lcompound_in_place.1
.Lcompound_in_place.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	compound_in_place, .-compound_in_place
	.globl	narrow_store
	.type	narrow_store, @function
narrow_store:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	subq	$8, %rsp
	movl	%edi, -12(%rbp)
	movsbl	%bl, %ebx
	movl	-12(%rbp), %eax
	movl	%eax, -16(%rbp)
	movl	-16(%rbp), %edi
	call	g@PLT
	movsbl	%al, %ebx
	movl	%ebx, %eax
	jmp	.Lnarrow_store.1
.Lnarrow_store.1:
	leaq	-8(%rbp), %rsp
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	narrow_store, .-narrow_store
	.globl	narrow_step
	.type	narrow_step, @function
narrow_step:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movb	%dil, -9(%rbp)
	movsbl	-9(%rbp), %ebx
	addl	$1, %ebx
	movsbl	%bl, %ebx
	movl	%ebx, %eax
	jmp	.Lnarrow_step.1
.Lnarrow_step.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	narrow_step, .-narrow_step
	.globl	narrow_unwritten
	.type	narrow_unwritten, @function
narrow_unwritten:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movsbl	%bl, %ebx
	movl	%ebx, %eax
	jmp	.Lnarrow_unwritten.1
.Lnarrow_unwritten.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	narrow_unwritten, .-narrow_unwritten
	.globl	held_step
	.type	held_step, @function
held_step:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movl	%edi, -12(%rbp)
	movl	-12(%rbp), %ebx
	movl	%ebx, %eax
	movl	%ebx, %ecx
	addl	$1, %ebx
	addl	%ecx, %eax
	jmp	.Lheld_step.1
.Lheld_step.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	held_step, .-held_step
	.globl	held_and
	.type	held_and, @function
held_and:
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
	movl	-12(%rbp), %ebx
	movq	%rbx, -24(%rbp)
	cmpl	$0, -16(%rbp)
	je	.Lheld_and.2
	movl	%ebx, %ecx
	addl	$1, %ebx
	cmpl	$0, %ecx
	je	.Lheld_and.2
	movl	$1, %ecx
	jmp	.Lheld_and.3
.Lheld_and.2:
	movl	$0, %ecx
.Lheld_and.3:
	movq	-24(%rbp), %rax
	addl	%ecx, %eax
	jmp	.Lheld_and.1
.Lheld_and.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	held_and, .-held_and
	.globl	held_or
	.type	held_or, @function
held_or:
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
	movl	-12(%rbp), %ebx
	movq	%rbx, -24(%rbp)
	cmpl	$0, -16(%rbp)
	jne	.Lheld_or.4
	movl	%ebx, %ecx
	addl	$1, %ebx
	cmpl	$0, %ecx
	je	.Lheld_or.2
.Lheld_or.4:
	movl	$1, %ecx
	jmp	.Lheld_or.3
.Lheld_or.2:
	movl	$0, %ecx
.Lheld_or.3:
	movq	-24(%rbp), %rax
	addl	%ecx, %eax
	jmp	.Lheld_or.1
.Lheld_or.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	held_or, .-held_or
	.globl	held_conditional
	.type	held_conditional, @function
held_conditional:
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
	movl	-12(%rbp), %ebx
	movq	%rbx, -24(%rbp)
	cmpl	$0, -16(%rbp)
	je	.Lheld_conditional.2
	movl	%ebx, %ecx
	addl	$1, %ebx
	jmp	.Lheld_conditional.3
.Lheld_conditional.2:
	movl	$0, %ecx
.Lheld_conditional.3:
	movq	-24(%rbp), %rax
	addl	%ecx, %eax
	jmp	.Lheld_conditional.1
.Lheld_conditional.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	held_conditional, .-held_conditional
	.globl	stacked
	.type	stacked, @function
stacked:
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
	movl	%edi, -20(%rbp)
	movl	%esi, -24(%rbp)
	movl	%edx, -28(%rbp)
	movl	%ecx, -32(%rbp)
	movl	%r8d, -36(%rbp)
	movl	%r9d, -40(%rbp)
	movl	16(%rbp), %ebx
	movl	32(%rbp), %r12d
	movl	-20(%rbp), %eax
	addl	-24(%rbp), %eax
	addl	-28(%rbp), %eax
	addl	-32(%rbp), %eax
	addl	-36(%rbp), %eax
	addl	-40(%rbp), %eax
	addl	%ebx, %eax
	addl	24(%rbp), %eax
	addl	%r12d, %eax
	jmp	.Lstacked.1
.Lstacked.1:
	popq	%r12
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	stacked, .-stacked
	.globl	mixed
	.type	mixed, @function
mixed:
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
	movl	%edi, -20(%rbp)
	movl	%esi, %ebx
	movl	%edx, -24(%rbp)
	movl	%ecx, %r12d
	movl	-20(%rbp), %eax
	addl	%ebx, %eax
	addl	-24(%rbp), %eax
	addl	%r12d, %eax
	jmp	.Lmixed.1
.Lmixed.1:
	popq	%r12
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	mixed, .-mixed
	.globl	epilogue
	.type	epilogue, @function
epilogue:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	subq	$24, %rsp
	movl	%edi, -12(%rbp)
	movl	-12(%rbp), %ebx
	movl	%ebx, -28(%rbp)
	movl	-28(%rbp), %eax
	movl	%eax, -32(%rbp)
	movl	-32(%rbp), %edi
	call	g@PLT
	addl	%ebx, %eax
	jmp	.Lepilogue.1
.Lepilogue.1:
	leaq	-8(%rbp), %rsp
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	epilogue, .-epilogue
	.globl	sixth
	.type	sixth, @function
sixth:
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
	pushq	%r14
	.cfi_offset %r14, -48
	pushq	%r15
	.cfi_offset %r15, -56
	movl	%edi, -44(%rbp)
	movl	$0, -48(%rbp)
	movl	-44(%rbp), %ebx
	movl	-44(%rbp), %r12d
	addl	$1, %r12d
	movl	-44(%rbp), %r13d
	addl	$2, %r13d
	movl	-44(%rbp), %r14d
	addl	$3, %r14d
	movl	-44(%rbp), %r15d
	addl	$4, %r15d
	movl	-44(%rbp), %eax
	addl	$5, %eax
	movl	%eax, -52(%rbp)
	movl	%ebx, %eax
	addl	%r12d, %eax
	addl	%r13d, %eax
	addl	%r14d, %eax
	addl	%r15d, %eax
	addl	-52(%rbp), %eax
	movl	%eax, -48(%rbp)
	imull	$2, -44(%rbp), %ebx
	movl	-48(%rbp), %eax
	addl	%ebx, %eax
	movl	%eax, -48(%rbp)
	movl	-48(%rbp), %eax
	jmp	.Lsixth.1
.Lsixth.1:
	popq	%r15
	popq	%r14
	popq	%r13
	popq	%r12
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	sixth, .-sixth
	.section	.note.GNU-stack,"",@progbits
