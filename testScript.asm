	.text
	.def	@feat.00;
	.scl	3;
	.type	0;
	.endef
	.globl	@feat.00
.set @feat.00, 0
	.file	"testScript"
	.def	printChar;
	.scl	2;
	.type	32;
	.endef
	.globl	printChar
	.p2align	4, 0x90
printChar:
.seh_proc printChar
	subq	$56, %rsp
	.seh_stackalloc 56
	.seh_endprologue
	cmpl	$6, current(%rip)
	jl	.LBB0_2
	movb	$89, 46(%rsp)
	movl	$0, 48(%rsp)
	leaq	46(%rsp), %rcx
	jmp	.LBB0_3
.LBB0_2:
	movb	$78, 47(%rsp)
	movl	$0, 52(%rsp)
	leaq	47(%rsp), %rcx
.LBB0_3:
	callq	puts
	nop
	addq	$56, %rsp
	retq
	.seh_endproc

	.def	main;
	.scl	2;
	.type	32;
	.endef
	.globl	main
	.p2align	4, 0x90
main:
.seh_proc main
	subq	$40, %rsp
	.seh_stackalloc 40
	.seh_endprologue
	movl	$0, 36(%rsp)
	cmpl	$9, 36(%rsp)
	jg	.LBB1_3
	.p2align	4, 0x90
.LBB1_2:
	incl	current(%rip)
	callq	printChar
	incl	36(%rsp)
	cmpl	$9, 36(%rsp)
	jle	.LBB1_2
.LBB1_3:
	xorl	%eax, %eax
	addq	$40, %rsp
	retq
	.seh_endproc

	.data
	.weak	current
	.p2align	2, 0x0
current:
	.long	1

