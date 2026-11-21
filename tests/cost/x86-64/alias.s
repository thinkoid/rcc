	.file	"alias.c"
	.text
	.globl	fgcmd
	.type	fgcmd, @function
fgcmd:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	$0, %eax
	jmp	.Lfgcmd.1
.Lfgcmd.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	fgcmd, .-fgcmd
	.globl	statically
	.type	statically, @function
statically:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	call	fgalias@PLT
	jmp	.Lstatically.1
.Lstatically.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	statically, .-statically
	.data
	.globl	count
	.type	count, @object
	.size	count, 4
	.p2align 2
count:
	.long	1
	.globl	bgcmd
	.type	bgcmd, @function
bgcmd = fgcmd
	.globl	total
total = count
	.type	fgalias, @function
fgalias = fgcmd
	.weak	wbgcmd
	.type	wbgcmd, @function
wbgcmd = fgcmd
	.weak	wtotal
wtotal = count
	.section	.note.GNU-stack,"",@progbits
