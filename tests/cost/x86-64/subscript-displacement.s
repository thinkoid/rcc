	.file	"subscript-displacement.c"
	.text
	.globl	row
	.type	row, @function
row:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rax
	movslq	-12(%rbp), %rcx
	movsbl	2(%rax,%rcx,8), %eax
	jmp	.Lrow.1
.Lrow.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	row, .-row
	.globl	local
	.type	local, @function
local:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movslq	-4(%rbp), %rax
	movsbl	-34(%rbp,%rax,8), %eax
	jmp	.Llocal.1
.Llocal.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	local, .-local
	.globl	cell
	.type	cell, @function
cell:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movl	20(%rax), %eax
	jmp	.Lcell.1
.Lcell.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	cell, .-cell
	.globl	address
	.type	address, @function
address:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rax
	movslq	-12(%rbp), %rcx
	leaq	2(%rax,%rcx,8), %rax
	jmp	.Laddress.1
.Laddress.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	address, .-address
	.globl	corner
	.type	corner, @function
corner:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	leaq	20(%rax), %rax
	jmp	.Lcorner.1
.Lcorner.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	corner, .-corner
	.globl	byte_view
	.type	byte_view, @function
byte_view:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movl	4097(%rax), %eax
	jmp	.Lbyte_view.1
.Lbyte_view.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	byte_view, .-byte_view
	.globl	byte_view_store
	.type	byte_view_store, @function
byte_view_store:
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
	movl	%ecx, 4097(%rax)
.Lbyte_view_store.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	byte_view_store, .-byte_view_store
	.globl	byte_view_near
	.type	byte_view_near, @function
byte_view_near:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movl	1(%rax), %eax
	jmp	.Lbyte_view_near.1
.Lbyte_view_near.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	byte_view_near, .-byte_view_near
	.globl	far_called
	.type	far_called, @function
far_called:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -16(%rbp)
	call	g@PLT
	movq	-16(%rbp), %rcx
	movl	%eax, 20000(%rcx)
.Lfar_called.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	far_called, .-far_called
	.globl	far_stored
	.type	far_stored, @function
far_stored:
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
	movl	%ecx, 20000(%rax)
.Lfar_stored.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	far_stored, .-far_stored
	.globl	byte_index
	.type	byte_index, @function
byte_index:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rax
	movslq	-12(%rbp), %rcx
	movl	(%rax,%rcx,1), %eax
	jmp	.Lbyte_index.1
.Lbyte_index.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	byte_index, .-byte_index
	.globl	byte_scaled
	.type	byte_scaled, @function
byte_scaled:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rax
	movslq	-12(%rbp), %rcx
	movq	(%rax,%rcx,2), %rax
	jmp	.Lbyte_scaled.1
.Lbyte_scaled.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	byte_scaled, .-byte_scaled
	.globl	byte_after_index
	.type	byte_after_index, @function
byte_after_index:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rax
	imull	$4, -12(%rbp), %ecx
	movslq	%ecx, %rcx
	movswl	2(%rax,%rcx,1), %eax
	jmp	.Lbyte_after_index.1
.Lbyte_after_index.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	byte_after_index, .-byte_after_index
	.globl	byte_register
	.type	byte_register, @function
byte_register:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movq	%rdi, -16(%rbp)
	movq	-16(%rbp), %rbx
	movl	8(%rbx), %eax
	jmp	.Lbyte_register.1
.Lbyte_register.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	byte_register, .-byte_register
	.globl	byte_slot
	.type	byte_slot, @function
byte_slot:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	movb	%al, -20(%rbp)
	movl	-16(%rbp), %eax
	jmp	.Lbyte_slot.1
.Lbyte_slot.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	byte_slot, .-byte_slot
	.globl	byte_label
	.type	byte_label, @function
byte_label:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	bytes+4(%rip), %eax
	jmp	.Lbyte_label.1
.Lbyte_label.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	byte_label, .-byte_label
	.globl	byte_held
	.type	byte_held, @function
byte_held:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rax
	movslq	-12(%rbp), %rcx
	addq	%rcx, %rax
	movq	%rax, -24(%rbp)
	call	step@PLT
	movq	-24(%rbp), %rcx
	subl	%eax, (%rcx)
.Lbyte_held.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	byte_held, .-byte_held
	.globl	byte_value
	.type	byte_value, @function
byte_value:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	addq	$24, %rax
	jmp	.Lbyte_value.1
.Lbyte_value.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	byte_value, .-byte_value
	.globl	view_subscript
	.type	view_subscript, @function
view_subscript:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movl	13(%rax), %eax
	jmp	.Lview_subscript.1
.Lview_subscript.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	view_subscript, .-view_subscript
	.globl	view_indexed
	.type	view_indexed, @function
view_indexed:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rax
	movslq	-12(%rbp), %rcx
	movl	1(%rax,%rcx,4), %eax
	jmp	.Lview_indexed.1
.Lview_indexed.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	view_indexed, .-view_indexed
	.globl	byte_both_sides
	.type	byte_both_sides, @function
byte_both_sides:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rax
	imull	$4, -12(%rbp), %ecx
	movslq	%ecx, %rcx
	movswl	6(%rax,%rcx,1), %eax
	jmp	.Lbyte_both_sides.1
.Lbyte_both_sides.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	byte_both_sides, .-byte_both_sides
	.globl	two_casts
	.type	two_casts, @function
two_casts:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movl	11(%rax), %eax
	jmp	.Ltwo_casts.1
.Ltwo_casts.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	two_casts, .-two_casts
	.globl	two_indexes
	.type	two_indexes, @function
two_indexes:
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
	imull	$4, -12(%rbp), %ecx
	movslq	%ecx, %rcx
	addq	%rcx, %rax
	movslq	-16(%rbp), %rcx
	movswl	(%rax,%rcx,1), %eax
	jmp	.Ltwo_indexes.1
.Ltwo_indexes.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	two_indexes, .-two_indexes
	.globl	view_beyond
	.type	view_beyond, @function
view_beyond:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	leaq	2000000000(%rax), %rax
	movl	1600000000(%rax), %eax
	jmp	.Lview_beyond.1
.Lview_beyond.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	view_beyond, .-view_beyond
	.globl	member_slot
	.type	member_slot, @function
member_slot:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -24(%rbp)
	movl	-20(%rbp), %eax
	jmp	.Lmember_slot.1
.Lmember_slot.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	member_slot, .-member_slot
	.globl	member_odd
	.type	member_odd, @function
member_odd:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -24(%rbp)
	movl	-23(%rbp), %eax
	jmp	.Lmember_odd.1
.Lmember_odd.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	member_odd, .-member_odd
	.globl	member_far
	.type	member_far, @function
member_far:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$608, %rsp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	movb	%al, -343(%rbp)
	movl	-343(%rbp), %eax
	jmp	.Lmember_far.1
.Lmember_far.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	member_far, .-member_far
	.globl	member_label
	.type	member_label, @function
member_label:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movswl	words+6(%rip), %eax
	jmp	.Lmember_label.1
.Lmember_label.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	member_label, .-member_label
	.globl	member_label_odd
	.type	member_label_odd, @function
member_label_odd:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	words+6(%rip), %eax
	jmp	.Lmember_label_odd.1
.Lmember_label_odd.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	member_label_odd, .-member_label_odd
	.globl	view_address
	.type	view_address, @function
view_address:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	leaq	13(%rax), %rax
	jmp	.Lview_address.1
.Lview_address.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	view_address, .-view_address
	.globl	view_address_zero
	.type	view_address_zero, @function
view_address_zero:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	jmp	.Lview_address_zero.1
.Lview_address_zero.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	view_address_zero, .-view_address_zero
	.globl	view_address_index
	.type	view_address_index, @function
view_address_index:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rax
	imull	$4, -12(%rbp), %ecx
	movslq	%ecx, %rcx
	leaq	4(%rax,%rcx,1), %rax
	addq	$2, %rax
	jmp	.Lview_address_index.1
.Lview_address_index.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	view_address_index, .-view_address_index
	.globl	view_address_register
	.type	view_address_register, @function
view_address_register:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movq	%rdi, -16(%rbp)
	movq	-16(%rbp), %rbx
	addq	$1, %rbx
	leaq	12(%rbx), %rbx
	movq	%rbx, %rax
	jmp	.Lview_address_register.1
.Lview_address_register.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	view_address_register, .-view_address_register
	.globl	view_address_held
	.type	view_address_held, @function
view_address_held:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	leaq	32777(%rax), %rax
	jmp	.Lview_address_held.1
.Lview_address_held.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	view_address_held, .-view_address_held
	.bss
	.globl	bytes
	.type	bytes, @object
	.size	bytes, 16
	.p2align 4
bytes:
	.zero	16
	.globl	words
	.type	words, @object
	.size	words, 32
	.p2align 4
words:
	.zero	32
	.section	.note.GNU-stack,"",@progbits
