	.file	"return-register.c"
	.text
	.globl	copied
	.type	copied, @function
copied:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movq	%rdi, -16(%rbp)
	movl	%esi, -20(%rbp)
	movl	%edx, -24(%rbp)
	movl	-24(%rbp), %ebx
	movq	-16(%rbp), %rcx
	movslq	-20(%rbp), %rdx
	movl	%ebx, %eax
	subl	(%rcx,%rdx,4), %eax
	jmp	.Lcopied.1
.Lcopied.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	copied, .-copied
	.globl	extended
	.type	extended, @function
extended:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	cltd
	idivl	-8(%rbp)
	movslq	%edx, %rax
	jmp	.Lextended.1
.Lextended.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	extended, .-extended
	.globl	reloaded
	.type	reloaded, @function
reloaded:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	movq	%rax, -16(%rbp)
	cmpl	$0, -8(%rbp)
	je	.Lreloaded.2
	movl	$1, %ecx
	jmp	.Lreloaded.3
.Lreloaded.2:
	movl	$2, %ecx
.Lreloaded.3:
	movq	-16(%rbp), %rax
	addl	%ecx, %eax
	jmp	.Lreloaded.1
.Lreloaded.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	reloaded, .-reloaded
	.globl	compared
	.type	compared, @function
compared:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	%edx, -12(%rbp)
	movl	-4(%rbp), %ecx
	movl	-8(%rbp), %eax
	cltd
	idivl	-12(%rbp)
	cmpl	%eax, %ecx
	setl	%al
	movzbl	%al, %eax
	jmp	.Lcompared.1
.Lcompared.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	compared, .-compared
	.globl	negated
	.type	negated, @function
negated:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	cltd
	idivl	-8(%rbp)
	cmpl	$0, %edx
	sete	%al
	movzbl	%al, %eax
	jmp	.Lnegated.1
.Lnegated.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	negated, .-negated
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
	movq	%rdx, -24(%rbp)
	movq	-8(%rbp), %rcx
	movslq	-12(%rbp), %rdx
	movq	-24(%rbp), %rax
	movq	%rax, (%rcx,%rdx,8)
	jmp	.Lstored.1
.Lstored.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	stored, .-stored
	.globl	added
	.type	added, @function
added:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	%rdx, -24(%rbp)
	movq	-8(%rbp), %rcx
	movslq	-12(%rbp), %rdx
	movq	-24(%rbp), %rsi
	movq	(%rcx,%rdx,8), %rax
	addq	%rsi, %rax
	movq	%rax, (%rcx,%rdx,8)
	jmp	.Ladded.1
.Ladded.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	added, .-added
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
	movl	-4(%rbp), %ecx
	movl	%ecx, %eax
	addl	$1, %ecx
	movl	%ecx, -4(%rbp)
	jmp	.Lstepped.1
.Lstepped.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	stepped, .-stepped
	.globl	stepped_element
	.type	stepped_element, @function
stepped_element:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rcx
	movslq	-12(%rbp), %rdx
	movl	(%rcx,%rdx,4), %esi
	movl	%esi, %eax
	addl	$1, %esi
	movl	%esi, (%rcx,%rdx,4)
	jmp	.Lstepped_element.1
.Lstepped_element.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	stepped_element, .-stepped_element
	.globl	divided
	.type	divided, @function
divided:
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
	movl	-4(%rbp), %ecx
	movl	-8(%rbp), %eax
	cltd
	idivl	-12(%rbp)
	addl	%eax, %ecx
	movl	%ecx, %eax
	cltd
	idivl	-16(%rbp)
	jmp	.Ldivided.1
.Ldivided.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	divided, .-divided
	.globl	multiplied
	.type	multiplied, @function
multiplied:
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
	imull	$4, %ebx, %eax
	jmp	.Lmultiplied.1
.Lmultiplied.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	multiplied, .-multiplied
	.globl	called
	.type	called, @function
called:
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
	movl	-12(%rbp), %ebx
	movl	-12(%rbp), %eax
	movl	%eax, -16(%rbp)
	movl	-16(%rbp), %edi
	call	f@PLT
	movl	%ebx, %ecx
	addl	%eax, %ecx
	movl	%ecx, %eax
	jmp	.Lcalled.1
.Lcalled.1:
	leaq	-8(%rbp), %rsp
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	called, .-called
	.globl	deeper
	.type	deeper, @function
deeper:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movq	%rdi, -16(%rbp)
	movl	%esi, -20(%rbp)
	movl	%edx, -24(%rbp)
	movl	-24(%rbp), %ebx
	movq	-16(%rbp), %rcx
	movl	-20(%rbp), %eax
	movl	$2, %esi
	cltd
	idivl	%esi
	movslq	%eax, %rax
	movl	%ebx, %edx
	subl	(%rcx,%rax,4), %edx
	movl	%edx, %eax
	jmp	.Ldeeper.1
.Ldeeper.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	deeper, .-deeper
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
	movl	-12(%rbp), %ebx
	movl	%ebx, %eax
	addl	$2, %ebx
	addl	%ebx, %eax
	jmp	.Lwritten.1
.Lwritten.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	written, .-written
	.globl	remainder_reloaded
	.type	remainder_reloaded, @function
remainder_reloaded:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	movq	%rax, -16(%rbp)
	cmpl	$0, -8(%rbp)
	je	.Lremainder_reloaded.2
	movl	$3, %ecx
	jmp	.Lremainder_reloaded.3
.Lremainder_reloaded.2:
	movl	$5, %ecx
.Lremainder_reloaded.3:
	movq	-16(%rbp), %rax
	cltd
	idivl	%ecx
	movslq	%edx, %rax
	jmp	.Lremainder_reloaded.1
.Lremainder_reloaded.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	remainder_reloaded, .-remainder_reloaded
	.globl	compared_reloaded
	.type	compared_reloaded, @function
compared_reloaded:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	movq	%rax, -16(%rbp)
	cmpl	$0, -8(%rbp)
	je	.Lcompared_reloaded.2
	movl	$1, %eax
	jmp	.Lcompared_reloaded.3
.Lcompared_reloaded.2:
	movl	$2, %eax
.Lcompared_reloaded.3:
	movq	-16(%rbp), %rcx
	cmpl	%eax, %ecx
	setl	%al
	movzbl	%al, %eax
	jmp	.Lcompared_reloaded.1
.Lcompared_reloaded.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	compared_reloaded, .-compared_reloaded
	.globl	negated_reloaded
	.type	negated_reloaded, @function
negated_reloaded:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	movq	%rax, -16(%rbp)
	cmpl	$0, -8(%rbp)
	je	.Lnegated_reloaded.2
	movl	$1, %ecx
	jmp	.Lnegated_reloaded.3
.Lnegated_reloaded.2:
	movl	$2, %ecx
.Lnegated_reloaded.3:
	movq	-16(%rbp), %rax
	addl	%ecx, %eax
	negl	%eax
	jmp	.Lnegated_reloaded.1
.Lnegated_reloaded.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	negated_reloaded, .-negated_reloaded
	.globl	difference_reloaded
	.type	difference_reloaded, @function
difference_reloaded:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	movl	%ecx, -28(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -40(%rbp)
	cmpl	$0, -28(%rbp)
	je	.Ldifference_reloaded.2
	movq	-16(%rbp), %rcx
	jmp	.Ldifference_reloaded.3
.Ldifference_reloaded.2:
	movq	-24(%rbp), %rcx
.Ldifference_reloaded.3:
	movq	-40(%rbp), %rax
	subq	%rcx, %rax
	movq	$4, %rcx
	cqto
	idivq	%rcx
	jmp	.Ldifference_reloaded.1
.Ldifference_reloaded.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	difference_reloaded, .-difference_reloaded
	.globl	indexed_reloaded
	.type	indexed_reloaded, @function
indexed_reloaded:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movl	%edx, -16(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -24(%rbp)
	cmpl	$0, -16(%rbp)
	je	.Lindexed_reloaded.2
	movslq	-12(%rbp), %rax
	jmp	.Lindexed_reloaded.3
.Lindexed_reloaded.2:
	movq	$0, %rax
.Lindexed_reloaded.3:
	movq	-24(%rbp), %rcx
	leaq	(%rcx,%rax,4), %rax
	jmp	.Lindexed_reloaded.1
.Lindexed_reloaded.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	indexed_reloaded, .-indexed_reloaded
	.globl	assigned_reloaded
	.type	assigned_reloaded, @function
assigned_reloaded:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	movq	%rax, -24(%rbp)
	cmpl	$0, -8(%rbp)
	je	.Lassigned_reloaded.2
	movl	$1, %ecx
	jmp	.Lassigned_reloaded.3
.Lassigned_reloaded.2:
	movl	$2, %ecx
.Lassigned_reloaded.3:
	movq	-24(%rbp), %rax
	addl	%ecx, %eax
	movl	%eax, -12(%rbp)
	jmp	.Lassigned_reloaded.1
.Lassigned_reloaded.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	assigned_reloaded, .-assigned_reloaded
	.globl	stored_widened
	.type	stored_widened, @function
stored_widened:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rcx
	movq	-16(%rbp), %rax
	movl	%eax, (%rcx)
	movl	%eax, %eax
	jmp	.Lstored_widened.1
.Lstored_widened.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	stored_widened, .-stored_widened
	.globl	stored_held
	.type	stored_held, @function
stored_held:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movl	%edx, -16(%rbp)
	movl	%ecx, -20(%rbp)
	movq	-8(%rbp), %rcx
	movslq	-12(%rbp), %rdx
	movl	-16(%rbp), %eax
	subl	-20(%rbp), %eax
	movl	%eax, (%rcx,%rdx,4)
	jmp	.Lstored_held.1
.Lstored_held.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	stored_held, .-stored_held
	.globl	incremented_element
	.type	incremented_element, @function
incremented_element:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rcx
	movslq	-12(%rbp), %rdx
	movl	(%rcx,%rdx,4), %eax
	addl	$1, %eax
	movl	%eax, (%rcx,%rdx,4)
	jmp	.Lincremented_element.1
.Lincremented_element.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	incremented_element, .-incremented_element
	.globl	compound_reloaded
	.type	compound_reloaded, @function
compound_reloaded:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -12(%rbp)
	cmpl	$0, -8(%rbp)
	je	.Lcompound_reloaded.2
	movl	$1, %ecx
	jmp	.Lcompound_reloaded.3
.Lcompound_reloaded.2:
	movl	$2, %ecx
.Lcompound_reloaded.3:
	movl	-12(%rbp), %eax
	addl	%ecx, %eax
	movl	%eax, -12(%rbp)
	jmp	.Lcompound_reloaded.1
.Lcompound_reloaded.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	compound_reloaded, .-compound_reloaded
	.globl	called_place
	.type	called_place, @function
called_place:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movl	%edx, -16(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -24(%rbp)
	movl	-12(%rbp), %eax
	movl	%eax, -28(%rbp)
	movl	-28(%rbp), %edi
	call	f@PLT
	movslq	%eax, %rax
	movl	-16(%rbp), %ecx
	movq	-24(%rbp), %rdx
	movl	%ecx, (%rdx,%rax,4)
	movl	%ecx, %eax
	jmp	.Lcalled_place.1
.Lcalled_place.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	called_place, .-called_place
	.section	.note.GNU-stack,"",@progbits
