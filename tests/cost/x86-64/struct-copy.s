	.file	"struct-copy.c"
	.text
	.globl	through
	.type	through, @function
through:
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
	movl	(%rcx), %edx
	movl	%edx, (%rax)
	movl	4(%rcx), %edx
	movl	%edx, 4(%rax)
.Lthrough.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	through, .-through
	.globl	globals
	.type	globals, @function
globals:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	corner(%rip), %eax
	movl	%eax, origin(%rip)
	movl	corner+4(%rip), %eax
	movl	%eax, origin+4(%rip)
	movq	second(%rip), %rax
	movq	%rax, first(%rip)
	movq	second+8(%rip), %rax
	movq	%rax, first+8(%rip)
	movq	second+16(%rip), %rax
	movq	%rax, first+16(%rip)
.Lglobals.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	globals, .-globals
	.globl	local
	.type	local, @function
local:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movzbl	(%rax), %ecx
	movb	%cl, -11(%rbp)
	movzbl	1(%rax), %ecx
	movb	%cl, -10(%rbp)
	movzbl	2(%rax), %ecx
	movb	%cl, -9(%rbp)
	movzbl	-11(%rbp), %eax
	movb	%al, three(%rip)
	movzbl	-10(%rbp), %eax
	movb	%al, three+1(%rip)
	movzbl	-9(%rbp), %eax
	movb	%al, three+2(%rip)
	movsbl	-9(%rbp), %eax
	jmp	.Llocal.1
.Llocal.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	local, .-local
	.globl	member
	.type	member, @function
member:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movl	%edx, -20(%rbp)
	movq	-8(%rbp), %rax
	movq	-16(%rbp), %rcx
	movslq	-20(%rbp), %rdx
	movl	(%rcx,%rdx,8), %esi
	movl	%esi, 8(%rax)
	movl	4(%rcx,%rdx,8), %esi
	movl	%esi, 12(%rax)
	movq	-16(%rbp), %rax
	movl	-20(%rbp), %ecx
	addl	$1, %ecx
	movslq	%ecx, %rcx
	movq	-8(%rbp), %rdx
	movl	(%rdx), %esi
	movl	%esi, (%rax,%rcx,8)
	movl	4(%rdx), %esi
	movl	%esi, 4(%rax,%rcx,8)
.Lmember.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	member, .-member
	.globl	unions
	.type	unions, @function
unions:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	other(%rip), %rax
	movq	%rax, one(%rip)
.Lunions.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	unions, .-unions
	.globl	effects
	.type	effects, @function
effects:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rax
	movl	(%rax), %ecx
	movl	%ecx, origin(%rip)
	movl	4(%rax), %ecx
	movl	%ecx, origin+4(%rip)
	movl	$0, -16(%rbp)
	.p2align 5
.Leffects.2:
	movl	-16(%rbp), %eax
	cmpl	-12(%rbp), %eax
	jge	.Leffects.4
	movq	-8(%rbp), %rax
	movslq	-16(%rbp), %rcx
	movl	(%rax,%rcx,8), %edx
	movl	%edx, corner(%rip)
	movl	4(%rax,%rcx,8), %edx
	movl	%edx, corner+4(%rip)
	addl	$1, -16(%rbp)
	jmp	.Leffects.2
.Leffects.4:
	movl	corner(%rip), %eax
	movl	%eax, origin(%rip)
	movl	corner+4(%rip), %eax
	movl	%eax, origin+4(%rip)
	movl	origin(%rip), %eax
	movl	%eax, corner(%rip)
	movl	origin+4(%rip), %eax
	movl	%eax, corner+4(%rip)
	movl	corner(%rip), %eax
	jmp	.Leffects.1
.Leffects.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	effects, .-effects
	.globl	codes
	.type	codes, @function
codes:
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
	movzwl	(%rax,%rcx,4), %edx
	movw	%dx, -16(%rbp)
	movzwl	2(%rax,%rcx,4), %edx
	movw	%dx, -14(%rbp)
	movq	-8(%rbp), %rax
	movl	-12(%rbp), %ecx
	addl	$1, %ecx
	movzwl	-16(%rbp), %edx
	movw	%dx, (%rax,%rcx,4)
	movzwl	-14(%rbp), %edx
	movw	%dx, 2(%rax,%rcx,4)
.Lcodes.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	codes, .-codes
	.globl	eight_units
	.type	eight_units, @function
eight_units:
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
	movl	(%rcx), %edx
	movl	%edx, (%rax)
	movl	4(%rcx), %edx
	movl	%edx, 4(%rax)
	movl	8(%rcx), %edx
	movl	%edx, 8(%rax)
	movl	12(%rcx), %edx
	movl	%edx, 12(%rax)
	movl	16(%rcx), %edx
	movl	%edx, 16(%rax)
	movl	20(%rcx), %edx
	movl	%edx, 20(%rax)
	movl	24(%rcx), %edx
	movl	%edx, 24(%rax)
	movl	28(%rcx), %edx
	movl	%edx, 28(%rax)
.Leight_units.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	eight_units, .-eight_units
	.globl	nine_units
	.type	nine_units, @function
nine_units:
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
	movq	$-72, %rsi
.Lnine_units.2:
	movq	72(%rcx,%rsi,1), %rdx
	movq	%rdx, 72(%rax,%rsi,1)
	addq	$8, %rsi
	jne	.Lnine_units.2
.Lnine_units.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	nine_units, .-nine_units
	.globl	nine_named
	.type	nine_named, @function
nine_named:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	$-72, %rdx
.Lnine_named.2:
	movq	72(%rax,%rdx,1), %rcx
	movq	%rcx, -8(%rbp,%rdx,1)
	addq	$8, %rdx
	jne	.Lnine_named.2
	leaq	spare(%rip), %rax
	movq	$-72, %rdx
.Lnine_named.3:
	movq	-8(%rbp,%rdx,1), %rcx
	movq	%rcx, 72(%rax,%rdx,1)
	addq	$8, %rdx
	jne	.Lnine_named.3
.Lnine_named.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	nine_named, .-nine_named
	.globl	nine_indexed
	.type	nine_indexed, @function
nine_indexed:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movl	%edx, -20(%rbp)
	movq	-8(%rbp), %rax
	movslq	-20(%rbp), %rcx
	imulq	$72, %rcx, %rcx
	movq	-16(%rbp), %rdx
	movslq	-20(%rbp), %rsi
	imulq	$72, %rsi, %rsi
	addq	%rcx, %rax
	addq	%rsi, %rdx
	movq	$-72, %rsi
.Lnine_indexed.2:
	movq	72(%rdx,%rsi,1), %rcx
	movq	%rcx, 72(%rax,%rsi,1)
	addq	$8, %rsi
	jne	.Lnine_indexed.2
.Lnine_indexed.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	nine_indexed, .-nine_indexed
	.globl	text_units
	.type	text_units, @function
text_units:
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
	movq	$-32, %rsi
.Ltext_units.2:
	movzbl	32(%rcx,%rsi,1), %edx
	movb	%dl, 32(%rax,%rsi,1)
	addq	$1, %rsi
	jne	.Ltext_units.2
.Ltext_units.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	text_units, .-text_units
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
	call	next@PLT
	movq	%rax, -8(%rbp)
	call	last@PLT
	movq	-8(%rbp), %rcx
	movzwl	(%rax), %edx
	movw	%dx, (%rcx)
	movzwl	2(%rax), %edx
	movw	%dx, 2(%rcx)
.Lheld.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	held, .-held
	.globl	registers
	.type	registers, @function
registers:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	pushq	%r12
	.cfi_offset %r12, -32
	movq	%rdi, -24(%rbp)
	movq	%rsi, -32(%rbp)
	movq	-24(%rbp), %rbx
	movq	-32(%rbp), %r12
	movzwl	(%r12), %eax
	movw	%ax, (%rbx)
	movzwl	2(%r12), %eax
	movw	%ax, 2(%rbx)
.Lregisters.1:
	popq	%r12
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	registers, .-registers
	.globl	stepped
	.type	stepped, @function
stepped:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	pushq	%r12
	.cfi_offset %r12, -32
	movq	%rdi, -24(%rbp)
	movq	%rsi, -32(%rbp)
	movq	-24(%rbp), %rbx
	movq	-32(%rbp), %r12
	movq	%rbx, %rax
	addq	$4, %rbx
	movzwl	(%r12), %ecx
	movw	%cx, (%rax)
	movzwl	2(%r12), %ecx
	movw	%cx, 2(%rax)
	addq	$4, %rbx
	movq	%r12, %rax
	subq	$4, %r12
	movzwl	(%rax), %ecx
	movw	%cx, (%rbx)
	movzwl	2(%rax), %ecx
	movw	%cx, 2(%rbx)
.Lstepped.1:
	popq	%r12
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	stepped, .-stepped
	.globl	comma_effects
	.type	comma_effects, @function
comma_effects:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movl	$0, -16(%rbp)
	.p2align 5
.Lcomma_effects.2:
	movl	-16(%rbp), %eax
	cmpl	-12(%rbp), %eax
	jge	.Lcomma_effects.4
	addl	$1, -16(%rbp)
	movq	-8(%rbp), %rax
	movslq	-16(%rbp), %rcx
	movl	(%rax,%rcx,8), %edx
	movl	%edx, corner(%rip)
	movl	4(%rax,%rcx,8), %edx
	movl	%edx, corner+4(%rip)
	jmp	.Lcomma_effects.2
.Lcomma_effects.4:
	addl	$1, -12(%rbp)
	movl	corner(%rip), %eax
	movl	%eax, origin(%rip)
	movl	corner+4(%rip), %eax
	movl	%eax, origin+4(%rip)
	addl	$1, -12(%rbp)
	movl	origin(%rip), %eax
	movl	%eax, corner(%rip)
	movl	origin+4(%rip), %eax
	movl	%eax, corner+4(%rip)
	movl	-12(%rbp), %eax
	jmp	.Lcomma_effects.1
.Lcomma_effects.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	comma_effects, .-comma_effects
	.bss
	.globl	origin
	.type	origin, @object
	.size	origin, 8
	.p2align 2
origin:
	.zero	8
	.globl	corner
	.type	corner, @object
	.size	corner, 8
	.p2align 2
corner:
	.zero	8
	.globl	first
	.type	first, @object
	.size	first, 24
	.p2align 3
first:
	.zero	24
	.globl	second
	.type	second, @object
	.size	second, 24
	.p2align 3
second:
	.zero	24
	.globl	three
	.type	three, @object
	.size	three, 3
three:
	.zero	3
	.globl	one
	.type	one, @object
	.size	one, 8
	.p2align 3
one:
	.zero	8
	.globl	other
	.type	other, @object
	.size	other, 8
	.p2align 3
other:
	.zero	8
	.globl	spare
	.type	spare, @object
	.size	spare, 72
	.p2align 3
spare:
	.zero	72
	.section	.note.GNU-stack,"",@progbits
