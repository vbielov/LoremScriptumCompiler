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
	pushq	%rax
	.seh_stackalloc 8
	.seh_endprologue
	movl	$2, (%rsp)
	movl	$1, 4(%rsp)
	movl	(%rsp), %eax
	.p2align	4, 0x90
.LBB0_1:
	cmpl	4(%rsp), %eax
	jle	.LBB0_1
	xorl	%eax, %eax
	popq	%rcx
	retq
	.seh_endproc

