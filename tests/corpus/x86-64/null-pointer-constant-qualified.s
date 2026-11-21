	.file	"null-pointer-constant-qualified.c"
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
	movl	$0, %eax
	cmpl	$0, %eax
	je	.Lmain.2
	movl	$101, %eax
	jmp	.Lmain.1
.Lmain.2:
	movl	$0, %eax
	cmpl	$0, %eax
	je	.Lmain.3
	movl	$102, %eax
	jmp	.Lmain.1
.Lmain.3:
	movl	$0, %eax
	cmpl	$0, %eax
	je	.Lmain.4
	movl	$103, %eax
	jmp	.Lmain.1
.Lmain.4:
	movl	$42, %eax
	jmp	.Lmain.1
.Lmain.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	main, .-main
	.bss
	.globl	c
	.type	c, @object
	.size	c, 4
	.p2align 2
c:
	.zero	4
	.globl	ip
	.type	ip, @object
	.size	ip, 8
	.p2align 3
ip:
	.zero	8
	.section	.note.GNU-stack,"",@progbits
