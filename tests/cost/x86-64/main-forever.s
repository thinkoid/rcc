	.file	"main-forever.c"
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
	.p2align 5
.Lmain.2:
	movl	g(%rip), %eax
	subl	$1, %eax
	movl	%eax, g(%rip)
	cmpl	$0, g(%rip)
	jne	.Lmain.5
	movl	$3, %eax
	jmp	.Lmain.1
.Lmain.5:
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
