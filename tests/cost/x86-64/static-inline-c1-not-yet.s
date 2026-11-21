	.file	"static-inline-c1-not-yet.c"
	.text
	.type	frame, @function
frame:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$20000016, %rsp
	leaq	.Ls0(%rip), %rax
	movq	%rax, -8(%rbp)
	addl	$1, frame.calls(%rip)
	movsbl	-10000008(%rbp), %eax
	movsbl	-20000008(%rbp), %ecx
	addl	%ecx, %eax
	movq	-8(%rbp), %rcx
	movsbl	(%rcx), %ecx
	addl	%ecx, %eax
	jmp	.Lframe.1
.Lframe.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	frame, .-frame
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	leaq	.Ls1(%rip), %rax
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	movsbl	(%rax), %eax
	subl	$109, %eax
	jmp	.Lmain.1
.Lmain.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	main, .-main
	.data
.Ls0:
	.string	"frame"
.Ls1:
	.string	"main"
	.bss
	.type	frame.calls, @object
	.size	frame.calls, 4
	.p2align 2
frame.calls:
	.zero	4
	.section	.note.GNU-stack,"",@progbits
