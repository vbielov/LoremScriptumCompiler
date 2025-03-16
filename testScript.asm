	.text
	.def	@feat.00;
	.scl	3;
	.type	0;
	.endef
	.globl	@feat.00
.set @feat.00, 0
	.file	"testScript"
	.def	__chkstk;
	.scl	2;
	.type	32;
	.endef
	.globl	__chkstk
	.p2align	4, 0x90
__chkstk:
.seh_proc __chkstk
	subq	$40, %rsp
	.seh_stackalloc 40
	.seh_endprologue
	callq	___chkstk_ms
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
	pushq	%rbp
	.seh_pushreg %rbp
	subq	$32, %rsp
	.seh_stackalloc 32
	leaq	32(%rsp), %rbp
	.seh_setframe %rbp, 32
	.seh_endprologue
	callq	__main
	leaq	helloWorld(%rip), %rcx
	callq	printf
	xorl	%eax, %eax
	addq	$32, %rsp
	popq	%rbp
	retq
	.seh_endproc

	.data
	.weak	helloWorld
helloWorld:
	.asciz	"Hello World!"

