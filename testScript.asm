	.text
	.def	@feat.00;
	.scl	3;
	.type	0;
	.endef
	.globl	@feat.00
.set @feat.00, 0
	.file	"testScript"
	.def	printYes;
	.scl	2;
	.type	32;
	.endef
	.globl	printYes
	.p2align	4, 0x90
printYes:
.seh_proc printYes
	subq	$40, %rsp
	.seh_stackalloc 40
	.seh_endprologue
	movb	$89, 33(%rsp)
	movb	$101, 34(%rsp)
	movb	$115, 35(%rsp)
	movl	$0, 36(%rsp)
	leaq	33(%rsp), %rcx
	callq	puts
	nop
	addq	$40, %rsp
	retq
	.seh_endproc

	.def	printNo;
	.scl	2;
	.type	32;
	.endef
	.globl	printNo
	.p2align	4, 0x90
printNo:
.seh_proc printNo
	subq	$40, %rsp
	.seh_stackalloc 40
	.seh_endprologue
	movb	$78, 34(%rsp)
	movb	$111, 35(%rsp)
	movl	$0, 36(%rsp)
	leaq	34(%rsp), %rcx
	callq	puts
	nop
	addq	$40, %rsp
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
	jle	.LBB2_2
	jmp	.LBB2_5
	.p2align	4, 0x90
.LBB2_3:
	callq	printYes
	incl	36(%rsp)
	cmpl	$9, 36(%rsp)
	jg	.LBB2_5
.LBB2_2:
	cmpl	$4, 36(%rsp)
	jle	.LBB2_3
	callq	printNo
	incl	36(%rsp)
	cmpl	$9, 36(%rsp)
	jle	.LBB2_2
.LBB2_5:
	xorl	%eax, %eax
	addq	$40, %rsp
	retq
	.seh_endproc

