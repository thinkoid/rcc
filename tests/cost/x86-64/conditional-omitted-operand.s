	.file	"conditional-omitted-operand.c"
	.text
	.globl	in_memory
	.type	in_memory, @function
in_memory:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	cmpl	$0, %eax
	jne	.Lin_memory.2
	movl	-8(%rbp), %eax
.Lin_memory.2:
	movl	%eax, h(%rip)
.Lin_memory.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	in_memory, .-in_memory
	.globl	called
	.type	called, @function
called:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	call	f@PLT
	cmpl	$0, %eax
	jne	.Lcalled.2
	call	g@PLT
.Lcalled.2:
	jmp	.Lcalled.1
.Lcalled.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	called, .-called
	.globl	pointer
	.type	pointer, @function
pointer:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rax
	cmpq	$0, %rax
	jne	.Lpointer.2
	movq	-16(%rbp), %rax
.Lpointer.2:
	jmp	.Lpointer.1
.Lpointer.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	pointer, .-pointer
	.globl	narrow
	.type	narrow, @function
narrow:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movb	%dil, -1(%rbp)
	movl	%esi, -8(%rbp)
	movsbl	-1(%rbp), %eax
	cmpl	$0, %eax
	jne	.Lnarrow.2
	movl	-8(%rbp), %eax
.Lnarrow.2:
	jmp	.Lnarrow.1
.Lnarrow.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	narrow, .-narrow
	.globl	condition
	.type	condition, @function
condition:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	cmpl	$0, -4(%rbp)
	jne	.Lcondition.3
	cmpl	$0, -8(%rbp)
	je	.Lcondition.2
.Lcondition.3:
	movl	$1, %eax
	jmp	.Lcondition.1
.Lcondition.2:
	movl	$0, %eax
	jmp	.Lcondition.1
.Lcondition.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	condition, .-condition
	.globl	into_register
	.type	into_register, @function
into_register:
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
	cmpl	$0, %ebx
	jne	.Linto_register.2
	movl	-16(%rbp), %ebx
.Linto_register.2:
	movl	%ebx, %eax
	jmp	.Linto_register.1
.Linto_register.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	into_register, .-into_register
	.globl	returned
	.type	returned, @function
returned:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	cmpl	$0, %eax
	jne	.Lreturned.2
	movl	-8(%rbp), %eax
.Lreturned.2:
	jmp	.Lreturned.1
.Lreturned.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	returned, .-returned
	.globl	discarded
	.type	discarded, @function
discarded:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	call	f@PLT
	cmpl	$0, %eax
	jne	.Ldiscarded.2
	call	g@PLT
.Ldiscarded.2:
.Ldiscarded.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	discarded, .-discarded
	.bss
	.globl	h
	.type	h, @object
	.size	h, 4
	.p2align 2
h:
	.zero	4
	.section	.note.GNU-stack,"",@progbits
