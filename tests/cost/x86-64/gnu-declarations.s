	.file	"gnu-declarations.c"
	.text
	.globl	use
	.type	use, @function
use:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$80, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -32(%rbp)
	movq	-32(%rbp), %rdi
	call	count@PLT
	movq	%rax, -40(%rbp)
	movq	-16(%rbp), %rax
	movl	(%rax), %eax
	movl	%eax, -44(%rbp)
	movl	-44(%rbp), %edi
	call	other_name@PLT
	addl	-40(%rbp), %eax
	movq	%rax, -56(%rbp)
	movq	-16(%rbp), %rax
	movq	%rax, -64(%rbp)
	movq	-24(%rbp), %rax
	movq	%rax, -72(%rbp)
	movq	-64(%rbp), %rdi
	movq	-72(%rbp), %rsi
	call	add@PLT
	addl	-56(%rbp), %eax
	jmp	.Luse.1
.Luse.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	use, .-use
	.globl	local
	.type	local, @function
local:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -8(%rbp)
	movl	-8(%rbp), %eax
	addl	$1, %eax
	jmp	.Llocal.1
.Llocal.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	local, .-local
	.globl	fall
	.type	fall, @function
fall:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	subl	$1, %eax
	cmpl	$1, %eax
	ja	.Lfall.4
	leaq	.Lfall.5(%rip), %rcx
	jmp	*(%rcx,%rax,8)
	.data
	.p2align 3
.Lfall.5:
	.quad	.Lfall.2
	.quad	.Lfall.3
	.text
.Lfall.2:
	addl	$1, -4(%rbp)
.Lfall.3:
	movl	-4(%rbp), %eax
	jmp	.Lfall.1
.Lfall.4:
	movl	$0, %eax
	jmp	.Lfall.1
.Lfall.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	fall, .-fall
	.bss
	.type	local.calls, @object
	.size	local.calls, 4
	.p2align 2
local.calls:
	.zero	4
	.section	.note.GNU-stack,"",@progbits
