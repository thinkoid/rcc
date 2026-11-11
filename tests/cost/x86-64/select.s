	.file	"select.c"
	.text
	.globl	select
	.type	select, @function
select:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	movl	-8(%rbp), %ecx
	cmpl	%ecx, %eax
	jge	.Lselect.2
	movl	-4(%rbp), %eax
	jmp	.Lselect.3
.Lselect.2:
	movl	-8(%rbp), %eax
.Lselect.3:
	jmp	.Lselect.1
.Lselect.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	select, .-select
	.section	.note.GNU-stack,"",@progbits
