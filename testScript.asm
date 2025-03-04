	.text
	.def	@feat.00;
	.scl	3;
	.type	0;
	.endef
	.globl	@feat.00
.set @feat.00, 0
	.file	"testScript"
	.def	main;
	.scl	2;
	.type	32;
	.endef
	.globl	main
	.p2align	4, 0x90
main:
.seh_proc main
	subq	$56, %rsp
	.seh_stackalloc 56
	.seh_endprologue
	movl	$0, 48(%rsp)
	movl	$0, 52(%rsp)
	movw	$25928, 43(%rsp)
	movb	$108, 45(%rsp)
	movl	45(%rsp), %eax
	movl	%eax, 46(%rsp)
	movb	$111, 47(%rsp)
	leaq	43(%rsp), %rcx
	callq	puts
	xorl	%eax, %eax
	addq	$56, %rsp
	retq
	.seh_endproc

