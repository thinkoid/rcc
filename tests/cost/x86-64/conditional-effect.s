	.file	"conditional-effect.c"
	.text
	.globl	pick
	.type	pick, @function
pick:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	cmpl	$0, -4(%rbp)
	je	.Lpick.2
	call	f@PLT
	jmp	.Lpick.3
.Lpick.2:
	call	g@PLT
.Lpick.3:
.Lpick.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	pick, .-pick
	.globl	dropped
	.type	dropped, @function
dropped:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	cmpl	$0, -4(%rbp)
	je	.Ldropped.2
	call	h@PLT
	jmp	.Ldropped.3
.Ldropped.2:
	call	k@PLT
.Ldropped.3:
.Ldropped.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	dropped, .-dropped
	.globl	nested
	.type	nested, @function
nested:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	cmpl	$0, -4(%rbp)
	je	.Lnested.2
	cmpl	$0, -8(%rbp)
	je	.Lnested.4
	call	f@PLT
	jmp	.Lnested.5
.Lnested.4:
	call	g@PLT
.Lnested.5:
	jmp	.Lnested.3
.Lnested.2:
	call	f@PLT
.Lnested.3:
.Lnested.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	nested, .-nested
	.globl	stored
	.type	stored, @function
stored:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	cmpl	$0, -4(%rbp)
	je	.Lstored.2
	movl	$1, x(%rip)
	jmp	.Lstored.3
.Lstored.2:
	movl	$2, x(%rip)
.Lstored.3:
.Lstored.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	stored, .-stored
	.bss
	.globl	x
	.type	x, @object
	.size	x, 4
	.p2align 2
x:
	.zero	4
	.section	.note.GNU-stack,"",@progbits
