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
	addl	%ecx, %eax
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
	addl	%ebx, %eax
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
	.globl	written_branched
	.type	written_branched, @function
written_branched:
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
	je	.Lwritten_branched.2
	movl	%ebx, %ecx
	addl	$1, %ebx
	jmp	.Lwritten_branched.3
.Lwritten_branched.2:
	movl	$2, %ecx
.Lwritten_branched.3:
	movq	-24(%rbp), %rax
	addl	%ecx, %eax
	jmp	.Lwritten_branched.1
.Lwritten_branched.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	written_branched, .-written_branched
	.globl	written_nested
	.type	written_nested, @function
written_nested:
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
	movl	-16(%rbp), %ecx
	movl	%ebx, %eax
	movl	$2, %ebx
	addl	%ebx, %ecx
	addl	%ecx, %eax
	jmp	.Lwritten_nested.1
.Lwritten_nested.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	written_nested, .-written_nested
	.globl	written_called
	.type	written_called, @function
written_called:
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
	movl	%ebx, %eax
	movq	%rax, -24(%rbp)
	movl	-12(%rbp), %eax
	movl	%eax, -28(%rbp)
	movl	-28(%rbp), %edi
	call	f@PLT
	movl	%eax, %ebx
	addl	-24(%rbp), %eax
	jmp	.Lwritten_called.1
.Lwritten_called.1:
	leaq	-8(%rbp), %rsp
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	written_called, .-written_called
	.globl	written_sum
	.type	written_sum, @function
written_sum:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movq	%rdi, -16(%rbp)
	movq	%rsi, -24(%rbp)
	movl	%edx, -28(%rbp)
	movq	-16(%rbp), %rbx
	movl	-28(%rbp), %ecx
	movq	%rbx, %rax
	movq	-24(%rbp), %rbx
	movl	$0, %edx
	addl	%edx, %ecx
	movslq	%ecx, %rcx
	addq	%rcx, %rax
	jmp	.Lwritten_sum.1
.Lwritten_sum.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	written_sum, .-written_sum
	.globl	written_row
	.type	written_row, @function
written_row:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movq	%rdi, -16(%rbp)
	movq	%rsi, -24(%rbp)
	movl	%edx, -28(%rbp)
	movq	-16(%rbp), %rbx
	movl	-28(%rbp), %ecx
	movq	%rbx, %rax
	movq	-24(%rbp), %rbx
	movl	$0, %edx
	addl	%edx, %ecx
	movslq	%ecx, %rcx
	imulq	$12, %rcx, %rcx
	addq	%rcx, %rax
	jmp	.Lwritten_row.1
.Lwritten_row.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	written_row, .-written_row
	.globl	written_difference
	.type	written_difference, @function
written_difference:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movq	%rdi, -16(%rbp)
	movq	%rsi, -24(%rbp)
	movq	%rdx, -32(%rbp)
	movq	-16(%rbp), %rbx
	movq	-32(%rbp), %rcx
	movq	%rbx, %rax
	movq	-24(%rbp), %rbx
	movl	$0, %edx
	movslq	%edx, %rdx
	addq	%rdx, %rcx
	subq	%rcx, %rax
	jmp	.Lwritten_difference.1
.Lwritten_difference.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	written_difference, .-written_difference
	.globl	written_quotient
	.type	written_quotient, @function
written_quotient:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movq	%rdi, -16(%rbp)
	movq	%rsi, -24(%rbp)
	movq	-16(%rbp), %rbx
	movq	%rbx, %rax
	movq	-24(%rbp), %rbx
	subq	%rbx, %rax
	movq	$4, %rcx
	cqto
	idivq	%rcx
	jmp	.Lwritten_quotient.1
.Lwritten_quotient.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	written_quotient, .-written_quotient
	.globl	written_sum_called
	.type	written_sum_called, @function
written_sum_called:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	subq	$56, %rsp
	movq	%rdi, -16(%rbp)
	movq	%rsi, -24(%rbp)
	movl	%edx, -28(%rbp)
	movq	-16(%rbp), %rbx
	movl	-28(%rbp), %eax
	movq	%rbx, %rcx
	movq	-24(%rbp), %rbx
	movq	%rax, -40(%rbp)
	movq	%rcx, -48(%rbp)
	movl	-28(%rbp), %eax
	movl	%eax, -52(%rbp)
	movl	-52(%rbp), %edi
	call	f@PLT
	addl	-40(%rbp), %eax
	movslq	%eax, %rax
	addq	-48(%rbp), %rax
	jmp	.Lwritten_sum_called.1
.Lwritten_sum_called.1:
	leaq	-8(%rbp), %rsp
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	written_sum_called, .-written_sum_called
	.globl	written_quotient_called
	.type	written_quotient_called, @function
written_quotient_called:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	subq	$56, %rsp
	movq	%rdi, -16(%rbp)
	movq	%rsi, -24(%rbp)
	movl	%edx, -28(%rbp)
	movq	-16(%rbp), %rbx
	movq	%rbx, %rcx
	movq	-24(%rbp), %rbx
	movq	-24(%rbp), %rsi
	movq	%rcx, -40(%rbp)
	movq	%rsi, -48(%rbp)
	movl	-28(%rbp), %eax
	movl	%eax, -52(%rbp)
	movl	-52(%rbp), %edi
	call	f@PLT
	movslq	%eax, %rax
	movq	-48(%rbp), %rcx
	leaq	(%rcx,%rax,4), %rcx
	movq	-40(%rbp), %rax
	subq	%rcx, %rax
	movq	$4, %rcx
	cqto
	idivq	%rcx
	jmp	.Lwritten_quotient_called.1
.Lwritten_quotient_called.1:
	leaq	-8(%rbp), %rsp
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	written_quotient_called, .-written_quotient_called
	.globl	called_minus
	.type	called_minus, @function
called_minus:
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
	subl	%eax, %ecx
	movl	%ecx, %eax
	jmp	.Lcalled_minus.1
.Lcalled_minus.1:
	leaq	-8(%rbp), %rsp
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	called_minus, .-called_minus
	.globl	compound_called
	.type	compound_called, @function
compound_called:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-8(%rbp), %eax
	movl	%eax, -12(%rbp)
	movl	-12(%rbp), %edi
	call	f@PLT
	movl	-4(%rbp), %ecx
	addl	%ecx, %eax
	movl	%eax, -4(%rbp)
	jmp	.Lcompound_called.1
.Lcompound_called.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	compound_called, .-compound_called
	.globl	pointer_called
	.type	pointer_called, @function
pointer_called:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -24(%rbp)
	movl	-12(%rbp), %eax
	movl	%eax, -28(%rbp)
	movl	-28(%rbp), %edi
	call	f@PLT
	movslq	%eax, %rax
	addq	-24(%rbp), %rax
	jmp	.Lpointer_called.1
.Lpointer_called.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	pointer_called, .-pointer_called
	.globl	row_called
	.type	row_called, @function
row_called:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -24(%rbp)
	movl	-12(%rbp), %eax
	movl	%eax, -28(%rbp)
	movl	-28(%rbp), %edi
	call	f@PLT
	movslq	%eax, %rax
	imulq	$12, %rax, %rax
	addq	-24(%rbp), %rax
	jmp	.Lrow_called.1
.Lrow_called.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	row_called, .-row_called
	.globl	scaled_called
	.type	scaled_called, @function
scaled_called:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -24(%rbp)
	movl	-12(%rbp), %eax
	movl	%eax, -28(%rbp)
	movl	-28(%rbp), %edi
	call	f@PLT
	movslq	%eax, %rax
	movq	-24(%rbp), %rcx
	leaq	(%rcx,%rax,4), %rax
	jmp	.Lscaled_called.1
.Lscaled_called.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	scaled_called, .-scaled_called
	.globl	called_pointer
	.type	called_pointer, @function
called_pointer:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movl	-12(%rbp), %eax
	movl	%eax, -16(%rbp)
	movl	-16(%rbp), %edi
	call	f@PLT
	movslq	%eax, %rax
	addq	-8(%rbp), %rax
	jmp	.Lcalled_pointer.1
.Lcalled_pointer.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	called_pointer, .-called_pointer
	.globl	register_row_called
	.type	register_row_called, @function
register_row_called:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	subq	$24, %rsp
	movq	%rdi, -16(%rbp)
	movl	%esi, -20(%rbp)
	movq	-16(%rbp), %rbx
	movl	-20(%rbp), %eax
	movl	%eax, -24(%rbp)
	movl	-24(%rbp), %edi
	call	f@PLT
	movslq	%eax, %rax
	imulq	$12, %rax, %rax
	addq	%rbx, %rax
	jmp	.Lregister_row_called.1
.Lregister_row_called.1:
	leaq	-8(%rbp), %rsp
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	register_row_called, .-register_row_called
	.globl	pointer_stored
	.type	pointer_stored, @function
pointer_stored:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -24(%rbp)
	movl	-12(%rbp), %eax
	movl	%eax, -28(%rbp)
	movl	-28(%rbp), %edi
	call	f@PLT
	movslq	%eax, %rax
	addq	-24(%rbp), %rax
	movq	%rax, stored_pointer(%rip)
.Lpointer_stored.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	pointer_stored, .-pointer_stored
	.globl	spilled_add
	.type	spilled_add, @function
spilled_add:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	movq	%rax, -16(%rbp)
	movl	-8(%rbp), %eax
	movl	%eax, -20(%rbp)
	movl	-20(%rbp), %edi
	call	f@PLT
	addl	-16(%rbp), %eax
	jmp	.Lspilled_add.1
.Lspilled_add.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	spilled_add, .-spilled_add
	.globl	spilled_mul
	.type	spilled_mul, @function
spilled_mul:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	movq	%rax, -16(%rbp)
	movl	-8(%rbp), %eax
	movl	%eax, -20(%rbp)
	movl	-20(%rbp), %edi
	call	f@PLT
	imull	-16(%rbp), %eax
	jmp	.Lspilled_mul.1
.Lspilled_mul.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	spilled_mul, .-spilled_mul
	.globl	spilled_and
	.type	spilled_and, @function
spilled_and:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	movq	%rax, -16(%rbp)
	movl	-8(%rbp), %eax
	movl	%eax, -20(%rbp)
	movl	-20(%rbp), %edi
	call	f@PLT
	andl	-16(%rbp), %eax
	jmp	.Lspilled_and.1
.Lspilled_and.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	spilled_and, .-spilled_and
	.globl	spilled_or
	.type	spilled_or, @function
spilled_or:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	movq	%rax, -16(%rbp)
	movl	-8(%rbp), %eax
	movl	%eax, -20(%rbp)
	movl	-20(%rbp), %edi
	call	f@PLT
	orl	-16(%rbp), %eax
	jmp	.Lspilled_or.1
.Lspilled_or.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	spilled_or, .-spilled_or
	.globl	spilled_xor
	.type	spilled_xor, @function
spilled_xor:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	movq	%rax, -16(%rbp)
	movl	-8(%rbp), %eax
	movl	%eax, -20(%rbp)
	movl	-20(%rbp), %edi
	call	f@PLT
	xorl	-16(%rbp), %eax
	jmp	.Lspilled_xor.1
.Lspilled_xor.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	spilled_xor, .-spilled_xor
	.globl	spilled_add_long
	.type	spilled_add_long, @function
spilled_add_long:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -24(%rbp)
	movq	-16(%rbp), %rax
	movq	%rax, -32(%rbp)
	movq	-32(%rbp), %rdi
	call	lf@PLT
	addq	-24(%rbp), %rax
	jmp	.Lspilled_add_long.1
.Lspilled_add_long.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	spilled_add_long, .-spilled_add_long
	.globl	spilled_mul_long
	.type	spilled_mul_long, @function
spilled_mul_long:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -24(%rbp)
	movq	-16(%rbp), %rax
	movq	%rax, -32(%rbp)
	movq	-32(%rbp), %rdi
	call	lf@PLT
	imulq	-24(%rbp), %rax
	jmp	.Lspilled_mul_long.1
.Lspilled_mul_long.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	spilled_mul_long, .-spilled_mul_long
	.globl	spilled_and_long
	.type	spilled_and_long, @function
spilled_and_long:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -24(%rbp)
	movq	-16(%rbp), %rax
	movq	%rax, -32(%rbp)
	movq	-32(%rbp), %rdi
	call	lf@PLT
	andq	-24(%rbp), %rax
	jmp	.Lspilled_and_long.1
.Lspilled_and_long.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	spilled_and_long, .-spilled_and_long
	.globl	spilled_or_long
	.type	spilled_or_long, @function
spilled_or_long:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -24(%rbp)
	movq	-16(%rbp), %rax
	movq	%rax, -32(%rbp)
	movq	-32(%rbp), %rdi
	call	lf@PLT
	orq	-24(%rbp), %rax
	jmp	.Lspilled_or_long.1
.Lspilled_or_long.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	spilled_or_long, .-spilled_or_long
	.globl	spilled_xor_long
	.type	spilled_xor_long, @function
spilled_xor_long:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -24(%rbp)
	movq	-16(%rbp), %rax
	movq	%rax, -32(%rbp)
	movq	-32(%rbp), %rdi
	call	lf@PLT
	xorq	-24(%rbp), %rax
	jmp	.Lspilled_xor_long.1
.Lspilled_xor_long.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	spilled_xor_long, .-spilled_xor_long
	.globl	spilled_stored
	.type	spilled_stored, @function
spilled_stored:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	movq	%rax, -16(%rbp)
	movl	-8(%rbp), %eax
	movl	%eax, -20(%rbp)
	movl	-20(%rbp), %edi
	call	f@PLT
	addl	-16(%rbp), %eax
	movl	%eax, stored_sum(%rip)
.Lspilled_stored.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	spilled_stored, .-spilled_stored
	.globl	spilled_minus
	.type	spilled_minus, @function
spilled_minus:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	movq	%rax, -16(%rbp)
	movl	-8(%rbp), %eax
	movl	%eax, -20(%rbp)
	movl	-20(%rbp), %edi
	call	f@PLT
	movq	-16(%rbp), %rcx
	subl	%eax, %ecx
	movl	%ecx, %eax
	jmp	.Lspilled_minus.1
.Lspilled_minus.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	spilled_minus, .-spilled_minus
	.globl	spilled_count
	.type	spilled_count, @function
spilled_count:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-8(%rbp), %eax
	movl	-4(%rbp), %ecx
	movq	%rax, -16(%rbp)
	movq	%rcx, -24(%rbp)
	movl	-8(%rbp), %eax
	movl	%eax, -28(%rbp)
	movl	-28(%rbp), %edi
	call	f@PLT
	movq	-24(%rbp), %rcx
	addl	%eax, %ecx
	movq	-16(%rbp), %rax
	shll	%cl, %eax
	jmp	.Lspilled_count.1
.Lspilled_count.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	spilled_count, .-spilled_count
	.bss
	.globl	stored_pointer
	.type	stored_pointer, @object
	.size	stored_pointer, 8
	.p2align 3
stored_pointer:
	.zero	8
	.globl	stored_sum
	.type	stored_sum, @object
	.size	stored_sum, 4
	.p2align 2
stored_sum:
	.zero	4
	.section	.note.GNU-stack,"",@progbits
