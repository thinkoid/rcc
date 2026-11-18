	.file	"loop-terminated.c"
	.text
	.globl	while_returns
	.type	while_returns, @function
while_returns:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	.p2align 5
.Lwhile_returns.2:
	cmpl	$0, -4(%rbp)
	je	.Lwhile_returns.3
	movl	-4(%rbp), %eax
	subl	$1, %eax
	movl	%eax, -4(%rbp)
	cmpl	$5, -4(%rbp)
	jg	.Lwhile_returns.3
	movl	$3, %eax
	jmp	.Lwhile_returns.1
.Lwhile_returns.3:
	movl	-4(%rbp), %eax
	jmp	.Lwhile_returns.1
.Lwhile_returns.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	while_returns, .-while_returns
	.globl	while_breaks
	.type	while_breaks, @function
while_breaks:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	.p2align 5
.Lwhile_breaks.2:
	cmpl	$0, -4(%rbp)
	je	.Lwhile_breaks.3
	movl	-4(%rbp), %eax
	subl	$1, %eax
	movl	%eax, -4(%rbp)
	jmp	.Lwhile_breaks.3
.Lwhile_breaks.3:
	movl	-4(%rbp), %eax
	jmp	.Lwhile_breaks.1
.Lwhile_breaks.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	while_breaks, .-while_breaks
	.globl	while_continues
	.type	while_continues, @function
while_continues:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	.p2align 5
.Lwhile_continues.2:
	cmpl	$0, -4(%rbp)
	je	.Lwhile_continues.3
	movl	-4(%rbp), %eax
	subl	$1, %eax
	movl	%eax, -4(%rbp)
	testl	$1, -4(%rbp)
	jne	.Lwhile_continues.2
	movl	$3, %eax
	jmp	.Lwhile_continues.1
.Lwhile_continues.3:
	movl	-4(%rbp), %eax
	jmp	.Lwhile_continues.1
.Lwhile_continues.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	while_continues, .-while_continues
	.globl	while_falls
	.type	while_falls, @function
while_falls:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	.p2align 5
.Lwhile_falls.2:
	cmpl	$0, -4(%rbp)
	je	.Lwhile_falls.3
	movl	-4(%rbp), %eax
	subl	$1, %eax
	movl	%eax, -4(%rbp)
	cmpl	$5, -4(%rbp)
	jle	.Lwhile_falls.4
	movl	$3, %eax
	jmp	.Lwhile_falls.1
.Lwhile_falls.4:
	jmp	.Lwhile_falls.2
.Lwhile_falls.3:
	movl	-4(%rbp), %eax
	jmp	.Lwhile_falls.1
.Lwhile_falls.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	while_falls, .-while_falls
	.globl	for_returns
	.type	for_returns, @function
for_returns:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	$0, -8(%rbp)
	.p2align 5
.Lfor_returns.2:
	movl	-8(%rbp), %eax
	cmpl	-4(%rbp), %eax
	jge	.Lfor_returns.4
	cmpl	$5, -8(%rbp)
	jg	.Lfor_returns.4
	movl	$3, %eax
	jmp	.Lfor_returns.1
.Lfor_returns.4:
	movl	-4(%rbp), %eax
	jmp	.Lfor_returns.1
.Lfor_returns.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	for_returns, .-for_returns
	.globl	for_continues
	.type	for_continues, @function
for_continues:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	$0, -8(%rbp)
	.p2align 5
.Lfor_continues.2:
	movl	-8(%rbp), %eax
	cmpl	-4(%rbp), %eax
	jge	.Lfor_continues.4
	testl	$1, -8(%rbp)
	jne	.Lfor_continues.3
	movl	$3, %eax
	jmp	.Lfor_continues.1
.Lfor_continues.3:
	movl	-8(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -8(%rbp)
	jmp	.Lfor_continues.2
.Lfor_continues.4:
	movl	-4(%rbp), %eax
	jmp	.Lfor_continues.1
.Lfor_continues.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	for_continues, .-for_continues
	.globl	for_falls
	.type	for_falls, @function
for_falls:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	$0, -8(%rbp)
	.p2align 5
.Lfor_falls.2:
	movl	-8(%rbp), %eax
	cmpl	-4(%rbp), %eax
	jge	.Lfor_falls.4
	cmpl	$5, -8(%rbp)
	jle	.Lfor_falls.5
	movl	$3, %eax
	jmp	.Lfor_falls.1
.Lfor_falls.5:
.Lfor_falls.3:
	movl	-8(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -8(%rbp)
	jmp	.Lfor_falls.2
.Lfor_falls.4:
	movl	-4(%rbp), %eax
	jmp	.Lfor_falls.1
.Lfor_falls.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	for_falls, .-for_falls
	.globl	for_ever
	.type	for_ever, @function
for_ever:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	.p2align 5
.Lfor_ever.2:
	movl	-4(%rbp), %eax
	subl	$1, %eax
	movl	%eax, -4(%rbp)
	cmpl	$0, -4(%rbp)
	jne	.Lfor_ever.5
	movl	$3, %eax
	jmp	.Lfor_ever.1
.Lfor_ever.5:
.Lfor_ever.3:
	jmp	.Lfor_ever.2
.Lfor_ever.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	for_ever, .-for_ever
	.globl	do_returns
	.type	do_returns, @function
do_returns:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	.p2align 5
.Ldo_returns.2:
	movl	-4(%rbp), %eax
	subl	$1, %eax
	movl	%eax, -4(%rbp)
	cmpl	$5, -4(%rbp)
	jg	.Ldo_returns.4
	movl	$3, %eax
	jmp	.Ldo_returns.1
.Ldo_returns.4:
	movl	-4(%rbp), %eax
	jmp	.Ldo_returns.1
.Ldo_returns.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	do_returns, .-do_returns
	.globl	do_continues
	.type	do_continues, @function
do_continues:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	.p2align 5
.Ldo_continues.2:
	movl	-4(%rbp), %eax
	subl	$1, %eax
	movl	%eax, -4(%rbp)
	testl	$1, -4(%rbp)
	jne	.Ldo_continues.3
	movl	$3, %eax
	jmp	.Ldo_continues.1
.Ldo_continues.3:
	cmpl	$0, -4(%rbp)
	jne	.Ldo_continues.2
	movl	-4(%rbp), %eax
	jmp	.Ldo_continues.1
.Ldo_continues.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	do_continues, .-do_continues
	.globl	do_falls
	.type	do_falls, @function
do_falls:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	.p2align 5
.Ldo_falls.2:
	movl	-4(%rbp), %eax
	subl	$1, %eax
	movl	%eax, -4(%rbp)
	cmpl	$5, -4(%rbp)
	jle	.Ldo_falls.5
	movl	$3, %eax
	jmp	.Ldo_falls.1
.Ldo_falls.5:
.Ldo_falls.3:
	cmpl	$0, -4(%rbp)
	jne	.Ldo_falls.2
	movl	-4(%rbp), %eax
	jmp	.Ldo_falls.1
.Ldo_falls.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	do_falls, .-do_falls
	.globl	switch_returns
	.type	switch_returns, @function
switch_returns:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	cmpl	$0, -4(%rbp)
	je	.Lswitch_returns.2
	movl	-8(%rbp), %eax
	cmpl	$1, %eax
	je	.Lswitch_returns.3
	jmp	.Lswitch_returns.4
.Lswitch_returns.3:
	movl	$1, %eax
	jmp	.Lswitch_returns.1
.Lswitch_returns.4:
	movl	$2, %eax
	jmp	.Lswitch_returns.1
.Lswitch_returns.2:
	movl	$3, -8(%rbp)
	movl	-8(%rbp), %eax
	jmp	.Lswitch_returns.1
.Lswitch_returns.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	switch_returns, .-switch_returns
	.globl	switch_breaks
	.type	switch_breaks, @function
switch_breaks:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	cmpl	$0, -4(%rbp)
	je	.Lswitch_breaks.2
	movl	-8(%rbp), %eax
	cmpl	$1, %eax
	je	.Lswitch_breaks.3
	jmp	.Lswitch_breaks.4
.Lswitch_breaks.3:
	jmp	.Lswitch_breaks.5
.Lswitch_breaks.4:
	movl	$2, %eax
	jmp	.Lswitch_breaks.1
.Lswitch_breaks.5:
	jmp	.Lswitch_breaks.6
.Lswitch_breaks.2:
	movl	$3, -8(%rbp)
.Lswitch_breaks.6:
	movl	-8(%rbp), %eax
	jmp	.Lswitch_breaks.1
.Lswitch_breaks.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	switch_breaks, .-switch_breaks
	.section	.note.GNU-stack,"",@progbits
