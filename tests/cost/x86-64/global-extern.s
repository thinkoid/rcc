	.file	"global-extern.c"
	.text
	.globl	get
	.type	get, @function
get:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	counter@GOTPCREL(%rip), %rax
	movl	(%rax), %eax
	jmp	.Lget.1
.Lget.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	get, .-get
	.globl	set
	.type	set, @function
set:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movq	counter@GOTPCREL(%rip), %rax
	movl	-4(%rbp), %ecx
	movl	%ecx, (%rax)
.Lset.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	set, .-set
	.globl	add
	.type	add, @function
add:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movq	counter@GOTPCREL(%rip), %rax
	movl	-4(%rbp), %ecx
	addl	%ecx, (%rax)
.Ladd.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	add, .-add
	.globl	step
	.type	step, @function
step:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	counter@GOTPCREL(%rip), %rax
	addl	$1, (%rax)
.Lstep.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	step, .-step
	.globl	address
	.type	address, @function
address:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	counter@GOTPCREL(%rip), %rax
	jmp	.Laddress.1
.Laddress.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	address, .-address
	.globl	increment
	.type	increment, @function
increment:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	counter@GOTPCREL(%rip), %rax
	movq	counter@GOTPCREL(%rip), %rcx
	movl	(%rcx), %ecx
	addl	$1, %ecx
	movl	%ecx, (%rax)
.Lincrement.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	increment, .-increment
	.section	.note.GNU-stack,"",@progbits
