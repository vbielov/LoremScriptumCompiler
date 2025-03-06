	.text
	.def	@feat.00;
	.scl	3;
	.type	0;
	.endef
	.globl	@feat.00
.set @feat.00, 0
	.file	"testScript"
	.def	emptyFunc;
	.scl	2;
	.type	32;
	.endef
	.globl	emptyFunc
	.p2align	4, 0x90
emptyFunc:
	retq

	.def	func;
	.scl	2;
	.type	32;
	.endef
	.globl	func
	.p2align	4, 0x90
func:
	movl	(%rcx), %eax
	leal	1(%rax), %r8d
	movl	%r8d, (%rcx)
	addl	$2, %eax
	movl	%eax, (%rdx)
	retq

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
	movl	$5, 40(%rsp)
	leaq	40(%rsp), %rcx
	leaq	44(%rsp), %rdx
	callq	func
	movl	44(%rsp), %eax
	movl	%eax, 48(%rsp)
	addl	$48, %eax
	movl	%eax, 38(%rsp)
	movb	$0, 39(%rsp)
	leaq	38(%rsp), %rcx
	leaq	52(%rsp), %rdx
	callq	puts
	xorl	%eax, %eax
	addq	$56, %rsp
	retq
	.seh_endproc

