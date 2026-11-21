	.file	"statement-expression.c"
	.text
	.globl	interrupts_off
	.type	interrupts_off, @function
interrupts_off:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	addl	$1, pending(%rip)
	movl	$0, %eax
.Linterrupts_off.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	interrupts_off, .-interrupts_off
	.globl	assigned
	.type	assigned, @function
assigned:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	call	f@PLT
	movl	%eax, -8(%rbp)
	movl	-8(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	jmp	.Lassigned.1
.Lassigned.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	assigned, .-assigned
	.globl	held
	.type	held, @function
held:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	movq	%rax, -16(%rbp)
	call	g@PLT
	movl	-8(%rbp), %eax
	addl	-16(%rbp), %eax
	jmp	.Lheld.1
.Lheld.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	held, .-held
	.globl	argument
	.type	argument, @function
argument:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	$1, -4(%rbp)
	call	g@PLT
	movl	$2, %eax
	movl	%eax, -8(%rbp)
	movl	$3, -12(%rbp)
	movl	-4(%rbp), %edi
	movl	-8(%rbp), %esi
	movl	-12(%rbp), %edx
	call	three@PLT
	jmp	.Largument.1
.Largument.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	argument, .-argument
	.globl	declared
	.type	declared, @function
declared:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	$1, -4(%rbp)
	call	f@PLT
	movl	%eax, -12(%rbp)
	imull	$2, -12(%rbp), %eax
	movl	%eax, -8(%rbp)
	movl	$3, -16(%rbp)
	movl	-4(%rbp), %edi
	movl	-8(%rbp), %esi
	movl	-16(%rbp), %edx
	call	three@PLT
	jmp	.Ldeclared.1
.Ldeclared.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	declared, .-declared
	.globl	tested
	.type	tested, @function
tested:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	call	g@PLT
	cmpl	$0, -4(%rbp)
	je	.Ltested.2
	movl	$1, %eax
	jmp	.Ltested.1
.Ltested.2:
	movl	$0, %eax
	jmp	.Ltested.1
.Ltested.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	tested, .-tested
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
	subq	$8, %rsp
	call	f@PLT
	movl	%eax, %ebx
	movl	%ebx, %eax
	jmp	.Lcopied.1
.Lcopied.1:
	leaq	-8(%rbp), %rsp
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	copied, .-copied
	.globl	bare
	.type	bare, @function
bare:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	addl	-8(%rbp), %eax
	jmp	.Lbare.1
.Lbare.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	bare, .-bare
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
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	movl	-8(%rbp), %ecx
	movl	%ecx, -12(%rbp)
	addl	-12(%rbp), %eax
	jmp	.Llocal.1
.Llocal.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	local, .-local
	.bss
	.globl	pending
	.type	pending, @object
	.size	pending, 4
	.p2align 2
pending:
	.zero	4
	.section	.note.GNU-stack,"",@progbits
