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
	subq	$40, %rsp
	.seh_stackalloc 40
	.seh_endprologue
	movl	$1819043144, 34(%rsp)
	movw	$111, 38(%rsp)
	leaq	34(%rsp), %rcx
	callq	puts
	xorl	%eax, %eax
	addq	$40, %rsp
	retq
	.seh_endproc

