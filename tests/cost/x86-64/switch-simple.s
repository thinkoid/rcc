	.file	"switch-simple.c"
	.text
	.globl	kind
	.type	kind, @function
kind:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	cmpl	$-1, %eax
	je	.Lkind.4
	cmpl	$0, %eax
	je	.Lkind.3
	cmpl	$10, %eax
	je	.Lkind.2
	cmpl	$100, %eax
	je	.Lkind.5
	jmp	.Lkind.6
.Lkind.2:
	movl	$1, %eax
	jmp	.Lkind.1
.Lkind.3:
	movl	$2, %eax
	jmp	.Lkind.1
.Lkind.4:
	movl	$3, %eax
	jmp	.Lkind.1
.Lkind.5:
	movl	$4, %eax
	jmp	.Lkind.1
.Lkind.6:
	movl	$5, %eax
	jmp	.Lkind.1
.Lkind.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	kind, .-kind
	.section	.note.GNU-stack,"",@progbits
