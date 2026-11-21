	.file	"asm-barrier.c"
	.text
	.globl	stored
	.type	stored, @function
stored:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	$1, g(%rip)
.Lstored.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	stored, .-stored
	.globl	stored_null
	.type	stored_null, @function
stored_null:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	$1, g(%rip)
.Lstored_null.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	stored_null, .-stored_null
	.globl	stored_cc
	.type	stored_cc, @function
stored_cc:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	$1, g(%rip)
.Lstored_cc.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	stored_cc, .-stored_cc
	.globl	stored_cc_null
	.type	stored_cc_null, @function
stored_cc_null:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	$1, g(%rip)
.Lstored_cc_null.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	stored_cc_null, .-stored_cc_null
	.globl	stored_bare
	.type	stored_bare, @function
stored_bare:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	$1, g(%rip)
.Lstored_bare.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	stored_bare, .-stored_bare
	.globl	stored_bare_null
	.type	stored_bare_null, @function
stored_bare_null:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	$1, g(%rip)
.Lstored_bare_null.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	stored_bare_null, .-stored_bare_null
	.globl	fenced
	.type	fenced, @function
fenced:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	$1, g(%rip)
.Lfenced.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	fenced, .-fenced
	.globl	fenced_null
	.type	fenced_null, @function
fenced_null:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	$1, g(%rip)
.Lfenced_null.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	fenced_null, .-fenced_null
	.globl	held
	.type	held, @function
held:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	addl	$1, pending(%rip)
	movl	$0, %eax
.Lheld.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	held, .-held
	.globl	held_null
	.type	held_null, @function
held_null:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	addl	$1, pending(%rip)
	movl	$0, %eax
.Lheld_null.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	held_null, .-held_null
	.globl	valued
	.type	valued, @function
valued:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	addl	$1, g(%rip)
	movl	$0, %eax
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	jmp	.Lvalued.1
.Lvalued.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	valued, .-valued
	.globl	valued_null
	.type	valued_null, @function
valued_null:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	addl	$1, g(%rip)
	movl	$0, %eax
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	jmp	.Lvalued_null.1
.Lvalued_null.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	valued_null, .-valued_null
	.bss
	.globl	g
	.type	g, @object
	.size	g, 4
	.p2align 2
g:
	.zero	4
	.globl	pending
	.type	pending, @object
	.size	pending, 4
	.p2align 2
pending:
	.zero	4
	.section	.note.GNU-stack,"",@progbits
