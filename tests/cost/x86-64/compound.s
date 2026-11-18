	.file	"compound.c"
	.text
	.globl	compound
	.type	compound, @function
compound:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -12(%rbp)
	movl	-8(%rbp), %eax
	addl	%eax, -12(%rbp)
	movl	-12(%rbp), %eax
	jmp	.Lcompound.1
.Lcompound.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	compound, .-compound
	.globl	narrow
	.type	narrow, @function
narrow:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	movb	%al, -9(%rbp)
	movl	-8(%rbp), %eax
	addb	%al, -9(%rbp)
	movsbl	-9(%rbp), %eax
	jmp	.Lnarrow.1
.Lnarrow.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	narrow, .-narrow
	.globl	in_value
	.type	in_value, @function
in_value:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	movb	%al, -9(%rbp)
	movl	-8(%rbp), %ecx
	movsbl	-9(%rbp), %eax
	addl	%ecx, %eax
	movb	%al, -9(%rbp)
	movsbl	%al, %eax
	jmp	.Lin_value.1
.Lin_value.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	in_value, .-in_value
	.globl	unsigned_value
	.type	unsigned_value, @function
unsigned_value:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rcx
	movl	-12(%rbp), %edx
	movl	(%rcx), %eax
	addl	%edx, %eax
	movl	%eax, (%rcx)
	jmp	.Lunsigned_value.1
.Lunsigned_value.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	unsigned_value, .-unsigned_value
	.globl	unsigned_divided
	.type	unsigned_divided, @function
unsigned_divided:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rcx
	movl	-12(%rbp), %esi
	movl	(%rcx), %eax
	xorl	%edx, %edx
	divl	%esi
	movl	%eax, (%rcx)
	jmp	.Lunsigned_divided.1
.Lunsigned_divided.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	unsigned_divided, .-unsigned_divided
	.globl	unsigned_shifted
	.type	unsigned_shifted, @function
unsigned_shifted:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rdx
	movl	-12(%rbp), %ecx
	movl	(%rdx), %eax
	shrl	%cl, %eax
	movl	%eax, (%rdx)
	jmp	.Lunsigned_shifted.1
.Lunsigned_shifted.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	unsigned_shifted, .-unsigned_shifted
	.globl	unsigned_step
	.type	unsigned_step, @function
unsigned_step:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rcx
	movl	(%rcx), %eax
	addl	$1, %eax
	movl	%eax, (%rcx)
	jmp	.Lunsigned_step.1
.Lunsigned_step.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	unsigned_step, .-unsigned_step
	.globl	unsigned_global
	.type	unsigned_global, @function
unsigned_global:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %ecx
	movl	counter(%rip), %eax
	subl	%ecx, %eax
	movl	%eax, counter(%rip)
	jmp	.Lunsigned_global.1
.Lunsigned_global.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	unsigned_global, .-unsigned_global
	.globl	unsigned_slot
	.type	unsigned_slot, @function
unsigned_slot:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -12(%rbp)
	movl	-8(%rbp), %ecx
	movl	-12(%rbp), %eax
	imull	%ecx, %eax
	movl	%eax, -12(%rbp)
	jmp	.Lunsigned_slot.1
.Lunsigned_slot.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	unsigned_slot, .-unsigned_slot
	.globl	unsigned_postfix
	.type	unsigned_postfix, @function
unsigned_postfix:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rcx
	movl	(%rcx), %edx
	movl	%edx, %eax
	addl	$1, %edx
	movl	%edx, (%rcx)
	jmp	.Lunsigned_postfix.1
.Lunsigned_postfix.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	unsigned_postfix, .-unsigned_postfix
	.globl	unsigned_wide
	.type	unsigned_wide, @function
unsigned_wide:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rcx
	movq	-16(%rbp), %rdx
	movl	(%rcx), %eax
	addq	%rdx, %rax
	movl	%eax, (%rcx)
	movl	%eax, %eax
	jmp	.Lunsigned_wide.1
.Lunsigned_wide.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	unsigned_wide, .-unsigned_wide
	.globl	unsigned_narrow
	.type	unsigned_narrow, @function
unsigned_narrow:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rcx
	movl	-12(%rbp), %edx
	movzwl	(%rcx), %eax
	addl	%edx, %eax
	movw	%ax, (%rcx)
	movzwq	%ax, %rax
	jmp	.Lunsigned_narrow.1
.Lunsigned_narrow.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	unsigned_narrow, .-unsigned_narrow
	.globl	signed_value
	.type	signed_value, @function
signed_value:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rcx
	movl	-12(%rbp), %edx
	movl	(%rcx), %eax
	addl	%edx, %eax
	movl	%eax, (%rcx)
	movslq	%eax, %rax
	jmp	.Lsigned_value.1
.Lsigned_value.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	signed_value, .-signed_value
	.globl	unsigned_plain
	.type	unsigned_plain, @function
unsigned_plain:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rcx
	movq	-16(%rbp), %rax
	movl	%eax, (%rcx)
	movl	%eax, %eax
	jmp	.Lunsigned_plain.1
.Lunsigned_plain.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	unsigned_plain, .-unsigned_plain
	.globl	called_value
	.type	called_value, @function
called_value:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -16(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -24(%rbp)
	movq	-24(%rbp), %rdi
	call	mix@PLT
	addq	-16(%rbp), %rax
	movq	%rax, -16(%rbp)
	jmp	.Lcalled_value.1
.Lcalled_value.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	called_value, .-called_value
	.globl	product_element
	.type	product_element, @function
product_element:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movl	%edx, -16(%rbp)
	movq	-8(%rbp), %rax
	movslq	-12(%rbp), %rcx
	movl	-16(%rbp), %edx
	imull	(%rax,%rcx,4), %edx
	movl	%edx, (%rax,%rcx,4)
.Lproduct_element.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	product_element, .-product_element
	.globl	product_pointer
	.type	product_pointer, @function
product_pointer:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rax
	movq	-16(%rbp), %rcx
	imulq	(%rax), %rcx
	movq	%rcx, (%rax)
	movq	-16(%rbp), %rax
	jmp	.Lproduct_pointer.1
.Lproduct_pointer.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	product_pointer, .-product_pointer
	.globl	product_value
	.type	product_value, @function
product_value:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rcx
	movq	-16(%rbp), %rdx
	movq	(%rcx), %rax
	imulq	%rdx, %rax
	movq	%rax, (%rcx)
	jmp	.Lproduct_value.1
.Lproduct_value.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	product_value, .-product_value
	.globl	called_minus
	.type	called_minus, @function
called_minus:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -16(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -24(%rbp)
	movq	-24(%rbp), %rdi
	call	mix@PLT
	movq	-16(%rbp), %rcx
	subq	%rax, %rcx
	movq	%rcx, -16(%rbp)
	movq	%rcx, %rax
	jmp	.Lcalled_minus.1
.Lcalled_minus.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	called_minus, .-called_minus
	.globl	product_char
	.type	product_char, @function
product_char:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rax
	movl	-12(%rbp), %ecx
	movsbl	(%rax), %edx
	imull	%ecx, %edx
	movb	%dl, (%rax)
.Lproduct_char.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	product_char, .-product_char
	.globl	times_slot
	.type	times_slot, @function
times_slot:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -8(%rbp)
	imull	$12, -8(%rbp), %eax
	movl	%eax, -8(%rbp)
	movl	-8(%rbp), %eax
	jmp	.Ltimes_slot.1
.Ltimes_slot.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	times_slot, .-times_slot
	.globl	times_pointer
	.type	times_pointer, @function
times_pointer:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rcx
	imull	$12, (%rcx), %eax
	movl	%eax, (%rcx)
	jmp	.Ltimes_pointer.1
.Ltimes_pointer.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	times_pointer, .-times_pointer
	.globl	times_global
	.type	times_global, @function
times_global:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	imulq	$4, scaled_global(%rip), %rax
	movq	%rax, scaled_global(%rip)
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	jmp	.Ltimes_global.1
.Ltimes_global.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	times_global, .-times_global
	.globl	times_char
	.type	times_char, @function
times_char:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rcx
	movsbl	(%rcx), %eax
	imull	$12, %eax, %eax
	movb	%al, (%rcx)
	movsbl	%al, %eax
	jmp	.Ltimes_char.1
.Ltimes_char.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	times_char, .-times_char
	.bss
	.globl	counter
	.type	counter, @object
	.size	counter, 4
	.p2align 2
counter:
	.zero	4
	.globl	scaled_global
	.type	scaled_global, @object
	.size	scaled_global, 8
	.p2align 3
scaled_global:
	.zero	8
	.section	.note.GNU-stack,"",@progbits
