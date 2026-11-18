	.file	"main-goto.c"
	.text
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
.Lmain.2:
	movl	g(%rip), %eax
	addl	$1, %eax
	movl	%eax, g(%rip)
	cmpl	$10, g(%rip)
	jne	.Lmain.3
	movl	g(%rip), %eax
	jmp	.Lmain.1
.Lmain.3:
	jmp	.Lmain.2
.Lmain.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	main, .-main
	.bss
	.globl	g
	.type	g, @object
	.size	g, 4
	.p2align 2
g:
	.zero	4
	.section	.note.GNU-stack,"",@progbits
