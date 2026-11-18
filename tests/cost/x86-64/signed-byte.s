	.file	"signed-byte.c"
	.text
	.globl	call_byte
	.type	call_byte, @function
call_byte:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	call	next@PLT
	movsbl	%al, %eax
	jmp	.Lcall_byte.1
.Lcall_byte.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	call_byte, .-call_byte
	.globl	call_byte_long
	.type	call_byte_long, @function
call_byte_long:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	call	next@PLT
	movsbq	%al, %rax
	jmp	.Lcall_byte_long.1
.Lcall_byte_long.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	call_byte_long, .-call_byte_long
	.globl	register_byte
	.type	register_byte, @function
register_byte:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movl	%edi, -12(%rbp)
	movsbl	%bl, %ebx
	movl	-12(%rbp), %eax
	movsbl	%al, %ebx
	movl	%ebx, %eax
	addl	$1, %eax
	jmp	.Lregister_byte.1
.Lregister_byte.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	register_byte, .-register_byte
	.globl	register_call_byte
	.type	register_call_byte, @function
register_call_byte:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	subq	$8, %rsp
	call	next@PLT
	movsbl	%al, %ebx
	movl	%ebx, %eax
	jmp	.Lregister_call_byte.1
.Lregister_call_byte.1:
	leaq	-8(%rbp), %rsp
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	register_call_byte, .-register_call_byte
	.section	.note.GNU-stack,"",@progbits
