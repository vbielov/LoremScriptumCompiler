	.text
	.def	@feat.00;
	.scl	3;
	.type	0;
	.endef
	.globl	@feat.00
.set @feat.00, 0
	.file	"my cool jit"
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
	callq	___chkstk_ms
	movb	$72, %cl
	callq	putchar
	movb	$101, %cl
	callq	putchar
	movb	$108, %cl
	callq	putchar
	movb	$108, %cl
	callq	putchar
	movb	$111, %cl
	callq	putchar
	movb	$32, %cl
	callq	putchar
	movb	$87, %cl
	callq	putchar
	movb	$111, %cl
	callq	putchar
	movb	$114, %cl
	callq	putchar
	movb	$108, %cl
	callq	putchar
	movb	$100, %cl
	callq	putchar
	movb	$33, %cl
	callq	putchar
	movl	$10, %ecx
	callq	putchar
	movl	$10, %ecx
	callq	putchar
	xorl	%eax, %eax
	addq	$40, %rsp
	retq
	.seh_endproc

