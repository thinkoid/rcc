	.file	"compound.c"
	.text
	.globl	compound
	.type	compound, @function
compound:
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
	movl	-8(%rbp), %eax
	addl	%eax, -12(%rbp)
	movl	-12(%rbp), %eax
	jmp	.Lcompound.1
.Lcompound.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	compound, .-compound
	.globl	narrow
	.type	narrow, @function
narrow:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	movb	%al, -9(%rbp)
	movl	-8(%rbp), %eax
	addb	%al, -9(%rbp)
	movsbl	-9(%rbp), %eax
	jmp	.Lnarrow.1
.Lnarrow.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	narrow, .-narrow
	.globl	in_value
	.type	in_value, @function
in_value:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	movb	%al, -9(%rbp)
	movsbl	-9(%rbp), %eax
	movl	-8(%rbp), %ecx
	addl	%ecx, %eax
	movb	%al, -9(%rbp)
	movsbl	%al, %eax
	jmp	.Lin_value.1
.Lin_value.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	in_value, .-in_value
	.section	.note.GNU-stack,"",@progbits
