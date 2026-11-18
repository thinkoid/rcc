	.file	"header-calendar.c"
	.text
	.globl	leap_year
	.type	leap_year, @function
leap_year:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	$4, %ecx
	cltd
	idivl	%ecx
	cmpl	$0, %edx
	jne	.Lleap_year.5
	movl	-4(%rbp), %eax
	movl	$100, %ecx
	cltd
	idivl	%ecx
	cmpl	$0, %edx
	jne	.Lleap_year.4
.Lleap_year.5:
	movl	-4(%rbp), %eax
	movl	$400, %ecx
	cltd
	idivl	%ecx
	cmpl	$0, %edx
	jne	.Lleap_year.2
.Lleap_year.4:
	movl	$1, %eax
	jmp	.Lleap_year.3
.Lleap_year.2:
	movl	$0, %eax
.Lleap_year.3:
	jmp	.Lleap_year.1
.Lleap_year.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	leap_year, .-leap_year
	.globl	day_of_year
	.type	day_of_year, @function
day_of_year:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	%edx, -12(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -24(%rbp)
	movl	-24(%rbp), %edi
	call	leap_year@PLT
	movl	%eax, -20(%rbp)
	movl	$1, -16(%rbp)
	.p2align 5
.Lday_of_year.2:
	movl	-16(%rbp), %eax
	cmpl	-8(%rbp), %eax
	jge	.Lday_of_year.4
	leaq	daytab(%rip), %rax
	movslq	-20(%rbp), %rcx
	imulq	$13, %rcx, %rcx
	addq	%rcx, %rax
	movslq	-16(%rbp), %rcx
	movsbl	(%rax,%rcx,1), %eax
	addl	%eax, -12(%rbp)
	addl	$1, -16(%rbp)
	jmp	.Lday_of_year.2
.Lday_of_year.4:
	movl	-12(%rbp), %eax
	jmp	.Lday_of_year.1
.Lday_of_year.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	day_of_year, .-day_of_year
	.globl	month_day
	.type	month_day, @function
month_day:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$48, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movq	%rdx, -16(%rbp)
	movq	%rcx, -24(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -36(%rbp)
	movl	-36(%rbp), %edi
	call	leap_year@PLT
	movl	%eax, -32(%rbp)
	movl	$1, -28(%rbp)
	.p2align 5
.Lmonth_day.2:
	movl	-8(%rbp), %eax
	leaq	daytab(%rip), %rcx
	movslq	-32(%rbp), %rdx
	imulq	$13, %rdx, %rdx
	addq	%rdx, %rcx
	movslq	-28(%rbp), %rdx
	movsbl	(%rcx,%rdx,1), %ecx
	cmpl	%ecx, %eax
	jle	.Lmonth_day.4
	leaq	daytab(%rip), %rax
	movslq	-32(%rbp), %rcx
	imulq	$13, %rcx, %rcx
	addq	%rcx, %rax
	movslq	-28(%rbp), %rcx
	movsbl	(%rax,%rcx,1), %eax
	subl	%eax, -8(%rbp)
	addl	$1, -28(%rbp)
	jmp	.Lmonth_day.2
.Lmonth_day.4:
	movq	-16(%rbp), %rax
	movl	-28(%rbp), %ecx
	movl	%ecx, (%rax)
	movq	-24(%rbp), %rax
	movl	-8(%rbp), %ecx
	movl	%ecx, (%rax)
.Lmonth_day.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	month_day, .-month_day
	.globl	month_name
	.type	month_name, @function
month_name:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	cmpl	$1, -4(%rbp)
	jl	.Lmonth_name.4
	cmpl	$12, -4(%rbp)
	jle	.Lmonth_name.2
.Lmonth_name.4:
	movq	name(%rip), %rax
	jmp	.Lmonth_name.3
.Lmonth_name.2:
	leaq	name(%rip), %rax
	movslq	-4(%rbp), %rcx
	movq	(%rax,%rcx,8), %rax
.Lmonth_name.3:
	jmp	.Lmonth_name.1
.Lmonth_name.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	month_name, .-month_name
	.globl	round_trip
	.type	round_trip, @function
round_trip:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$80, %rsp
	movl	%edi, -4(%rbp)
	movl	$1, -8(%rbp)
	.p2align 5
.Lround_trip.2:
	movl	-8(%rbp), %eax
	movq	%rax, -24(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -28(%rbp)
	movl	-28(%rbp), %edi
	call	leap_year@PLT
	addl	$365, %eax
	movq	-24(%rbp), %rcx
	cmpl	%eax, %ecx
	jg	.Lround_trip.4
	movl	-4(%rbp), %eax
	movl	%eax, -32(%rbp)
	movl	-8(%rbp), %eax
	movl	%eax, -36(%rbp)
	leaq	-12(%rbp), %rax
	movq	%rax, -48(%rbp)
	leaq	-16(%rbp), %rax
	movq	%rax, -56(%rbp)
	movl	-32(%rbp), %edi
	movl	-36(%rbp), %esi
	movq	-48(%rbp), %rdx
	movq	-56(%rbp), %rcx
	call	month_day@PLT
	movl	-4(%rbp), %eax
	movl	%eax, -60(%rbp)
	movl	-12(%rbp), %eax
	movl	%eax, -64(%rbp)
	movl	-16(%rbp), %eax
	movl	%eax, -68(%rbp)
	movl	-60(%rbp), %edi
	movl	-64(%rbp), %esi
	movl	-68(%rbp), %edx
	call	day_of_year@PLT
	cmpl	-8(%rbp), %eax
	je	.Lround_trip.5
	movl	-8(%rbp), %eax
	jmp	.Lround_trip.1
.Lround_trip.5:
	addl	$1, -8(%rbp)
	jmp	.Lround_trip.2
.Lround_trip.4:
	movl	$0, %eax
	jmp	.Lround_trip.1
.Lround_trip.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	round_trip, .-round_trip
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$144, %rsp
	movl	$1988, -12(%rbp)
	movl	-12(%rbp), %edi
	call	round_trip@PLT
	cmpl	$0, %eax
	jne	.Lmain.3
	movl	$2026, -16(%rbp)
	movl	-16(%rbp), %edi
	call	round_trip@PLT
	cmpl	$0, %eax
	je	.Lmain.2
.Lmain.3:
	movl	$1, %eax
	jmp	.Lmain.1
.Lmain.2:
	movl	$0, -20(%rbp)
	movl	-20(%rbp), %edi
	call	month_name@PLT
	cmpq	name(%rip), %rax
	jne	.Lmain.5
	movl	$13, -24(%rbp)
	movl	-24(%rbp), %edi
	call	month_name@PLT
	cmpq	name(%rip), %rax
	je	.Lmain.4
.Lmain.5:
	movl	$2, %eax
	jmp	.Lmain.1
.Lmain.4:
	movl	$1988, -28(%rbp)
	movl	$60, -32(%rbp)
	leaq	-4(%rbp), %rax
	movq	%rax, -40(%rbp)
	leaq	-8(%rbp), %rax
	movq	%rax, -48(%rbp)
	movl	-28(%rbp), %edi
	movl	-32(%rbp), %esi
	movq	-40(%rbp), %rdx
	movq	-48(%rbp), %rcx
	call	month_day@PLT
	leaq	.Ls0(%rip), %rax
	movq	%rax, -56(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -68(%rbp)
	movl	-68(%rbp), %edi
	call	month_name@PLT
	movq	%rax, -64(%rbp)
	movl	-8(%rbp), %eax
	movl	%eax, -72(%rbp)
	movq	-56(%rbp), %rdi
	movq	-64(%rbp), %rsi
	movl	-72(%rbp), %edx
	movl	$0, %eax
	call	printf@PLT
	movl	$2026, -76(%rbp)
	movl	$365, -80(%rbp)
	leaq	-4(%rbp), %rax
	movq	%rax, -88(%rbp)
	leaq	-8(%rbp), %rax
	movq	%rax, -96(%rbp)
	movl	-76(%rbp), %edi
	movl	-80(%rbp), %esi
	movq	-88(%rbp), %rdx
	movq	-96(%rbp), %rcx
	call	month_day@PLT
	leaq	.Ls1(%rip), %rax
	movq	%rax, -104(%rbp)
	movl	$365, -108(%rbp)
	movl	$2026, -112(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -124(%rbp)
	movl	-124(%rbp), %edi
	call	month_name@PLT
	movq	%rax, -120(%rbp)
	movl	-8(%rbp), %eax
	movl	%eax, -128(%rbp)
	movq	-104(%rbp), %rdi
	movl	-108(%rbp), %esi
	movl	-112(%rbp), %edx
	movq	-120(%rbp), %rcx
	movl	-128(%rbp), %r8d
	movl	$0, %eax
	call	printf@PLT
	movl	$1988, -132(%rbp)
	movl	$3, -136(%rbp)
	movl	$1, -140(%rbp)
	movl	-132(%rbp), %edi
	movl	-136(%rbp), %esi
	movl	-140(%rbp), %edx
	call	day_of_year@PLT
	jmp	.Lmain.1
.Lmain.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	main, .-main
	.data
.Ls0:
	.string	"day 60 of 1988 is %s %d\n"
.Ls1:
	.string	"day %d of %d is %s %d\n"
.Ls2:
	.string	"Illegal month"
.Ls3:
	.string	"January"
.Ls4:
	.string	"February"
.Ls5:
	.string	"March"
.Ls6:
	.string	"April"
.Ls7:
	.string	"May"
.Ls8:
	.string	"June"
.Ls9:
	.string	"July"
.Ls10:
	.string	"August"
.Ls11:
	.string	"September"
.Ls12:
	.string	"October"
.Ls13:
	.string	"November"
.Ls14:
	.string	"December"
	.data
	.globl	daytab
	.type	daytab, @object
	.size	daytab, 26
	.p2align 4
daytab:
	.byte	0
	.byte	31
	.byte	28
	.byte	31
	.byte	30
	.byte	31
	.byte	30
	.byte	31
	.byte	31
	.byte	30
	.byte	31
	.byte	30
	.byte	31
	.byte	0
	.byte	31
	.byte	29
	.byte	31
	.byte	30
	.byte	31
	.byte	30
	.byte	31
	.byte	31
	.byte	30
	.byte	31
	.byte	30
	.byte	31
	.globl	name
	.type	name, @object
	.size	name, 104
	.p2align 4
name:
	.quad	.Ls2
	.quad	.Ls3
	.quad	.Ls4
	.quad	.Ls5
	.quad	.Ls6
	.quad	.Ls7
	.quad	.Ls8
	.quad	.Ls9
	.quad	.Ls10
	.quad	.Ls11
	.quad	.Ls12
	.quad	.Ls13
	.quad	.Ls14
	.section	.note.GNU-stack,"",@progbits
