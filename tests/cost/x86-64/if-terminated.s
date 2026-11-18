	.file	"if-terminated.c"
	.text
	.globl	value
	.type	value, @function
value:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	cmpl	$0, -4(%rbp)
	je	.Lvalue.2
	movl	-8(%rbp), %eax
	addl	$1, %eax
	jmp	.Lvalue.1
.Lvalue.2:
	imull	$3, -8(%rbp), %eax
	movl	%eax, -8(%rbp)
	movl	-8(%rbp), %eax
	jmp	.Lvalue.1
.Lvalue.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	value, .-value
	.globl	broken
	.type	broken, @function
broken:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	$0, -8(%rbp)
	.p2align 5
.Lbroken.2:
	cmpl	$0, -4(%rbp)
	je	.Lbroken.3
	testl	$1, -4(%rbp)
	je	.Lbroken.4
	movl	-8(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -8(%rbp)
	jmp	.Lbroken.3
.Lbroken.4:
	movl	-8(%rbp), %eax
	addl	$2, %eax
	movl	%eax, -8(%rbp)
	movl	-4(%rbp), %eax
	sarl	$1, %eax
	movl	%eax, -4(%rbp)
	jmp	.Lbroken.2
.Lbroken.3:
	movl	-8(%rbp), %eax
	jmp	.Lbroken.1
.Lbroken.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	broken, .-broken
	.globl	continued
	.type	continued, @function
continued:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	$0, -8(%rbp)
	movl	$0, -12(%rbp)
	.p2align 5
.Lcontinued.2:
	movl	-12(%rbp), %eax
	cmpl	-4(%rbp), %eax
	jge	.Lcontinued.4
	testl	$1, -12(%rbp)
	je	.Lcontinued.5
	movl	-8(%rbp), %eax
	addl	-12(%rbp), %eax
	movl	%eax, -8(%rbp)
	jmp	.Lcontinued.3
.Lcontinued.5:
	movl	-8(%rbp), %eax
	subl	$1, %eax
	movl	%eax, -8(%rbp)
	movl	-8(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -8(%rbp)
.Lcontinued.3:
	movl	-12(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -12(%rbp)
	jmp	.Lcontinued.2
.Lcontinued.4:
	movl	-8(%rbp), %eax
	jmp	.Lcontinued.1
.Lcontinued.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	continued, .-continued
	.globl	jumped
	.type	jumped, @function
jumped:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	cmpl	$0, -4(%rbp)
	jge	.Ljumped.2
	movl	$0, -4(%rbp)
	jmp	.Ljumped.3
.Ljumped.2:
	movl	-4(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -4(%rbp)
	imull	$2, -4(%rbp), %eax
	movl	%eax, -4(%rbp)
.Ljumped.3:
	movl	-4(%rbp), %eax
	jmp	.Ljumped.1
.Ljumped.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	jumped, .-jumped
	.globl	nested
	.type	nested, @function
nested:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	%edx, -12(%rbp)
	cmpl	$0, -4(%rbp)
	je	.Lnested.2
	cmpl	$0, -8(%rbp)
	je	.Lnested.3
	movl	$1, %eax
	jmp	.Lnested.1
.Lnested.3:
	movl	$2, %eax
	jmp	.Lnested.1
.Lnested.2:
	movl	$3, -12(%rbp)
	movl	-12(%rbp), %eax
	jmp	.Lnested.1
.Lnested.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	nested, .-nested
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
	movl	%esi, -8(%rbp)
	movl	%edx, -12(%rbp)
	cmpl	$0, -4(%rbp)
	je	.Lfall.2
	cmpl	$0, -8(%rbp)
	je	.Lfall.3
	movl	$1, %eax
	jmp	.Lfall.1
.Lfall.3:
	jmp	.Lfall.4
.Lfall.2:
	movl	$3, -12(%rbp)
.Lfall.4:
	movl	-12(%rbp), %eax
	jmp	.Lfall.1
.Lfall.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	fall, .-fall
	.globl	landed
	.type	landed, @function
landed:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	cmpl	$0, -4(%rbp)
	je	.Llanded.2
	cmpl	$9, -8(%rbp)
	jg	.Llanded.3
	movl	-8(%rbp), %eax
	jmp	.Llanded.1
.Llanded.3:
	jmp	.Llanded.4
.Llanded.2:
	movl	$2, -8(%rbp)
.Llanded.4:
	movl	-8(%rbp), %eax
	addl	$1, %eax
	jmp	.Llanded.1
.Llanded.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	landed, .-landed
	.section	.note.GNU-stack,"",@progbits
