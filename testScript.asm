	.text
	.def	@feat.00;
	.scl	3;
	.type	0;
	.endef
	.globl	@feat.00
.set @feat.00, 0
	.file	"testScript"
	.def	someFunc;
	.scl	2;
	.type	32;
	.endef
	.globl	someFunc
	.p2align	4, 0x90
someFunc:
.seh_proc someFunc
	subq	$24, %rsp
	.seh_stackalloc 24
	.seh_endprologue
	movl	$0, (%rsp)
	cmpl	$4, (%rsp)
	jg	.LBB0_3
	.p2align	4, 0x90
.LBB0_2:
	movslq	(%rsp), %rax
	movl	$5, %r8d
	subl	(%rcx,%rax,4), %r8d
	movl	%r8d, 4(%rsp,%rax,4)
	incl	%eax
	movl	%eax, (%rsp)
	cmpl	$4, (%rsp)
	jle	.LBB0_2
.LBB0_3:
	movl	20(%rsp), %eax
	movups	4(%rsp), %xmm0
	movups	%xmm0, (%rdx)
	movl	%eax, 16(%rdx)
	addq	$24, %rsp
	retq
	.seh_endproc

	.def	main;
	.scl	2;
	.type	32;
	.endef
	.globl	__xmm@00000003000000020000000100000000
	.section	.rdata,"dr",discard,__xmm@00000003000000020000000100000000
	.p2align	4, 0x0
__xmm@00000003000000020000000100000000:
	.long	0
	.long	1
	.long	2
	.long	3
	.text
	.globl	main
	.p2align	4, 0x90
main:
.seh_proc main
	pushq	%rsi
	.seh_pushreg %rsi
	pushq	%rdi
	.seh_pushreg %rdi
	subq	$120, %rsp
	.seh_stackalloc 120
	.seh_endprologue
	movaps	__xmm@00000003000000020000000100000000(%rip), %xmm0
	movups	%xmm0, 60(%rsp)
	movl	$4, 76(%rsp)
	leaq	60(%rsp), %rcx
	leaq	100(%rsp), %rdx
	callq	someFunc
	movl	116(%rsp), %eax
	movups	100(%rsp), %xmm0
	movups	%xmm0, 80(%rsp)
	movl	%eax, 96(%rsp)
	movl	$0, 40(%rsp)
	leaq	39(%rsp), %rsi
	leaq	52(%rsp), %rdi
	cmpl	$4, 40(%rsp)
	jg	.LBB1_3
	.p2align	4, 0x90
.LBB1_2:
	movslq	40(%rsp), %rax
	movzbl	60(%rsp,%rax,4), %eax
	addb	$48, %al
	movb	%al, 39(%rsp)
	movl	$0, 48(%rsp)
	movq	%rsi, %rcx
	movq	%rdi, %rdx
	callq	puts
	incl	40(%rsp)
	cmpl	$4, 40(%rsp)
	jle	.LBB1_2
.LBB1_3:
	movl	$0, 44(%rsp)
	leaq	39(%rsp), %rsi
	leaq	56(%rsp), %rdi
	cmpl	$4, 44(%rsp)
	jg	.LBB1_6
	.p2align	4, 0x90
.LBB1_5:
	movslq	44(%rsp), %rax
	movzbl	80(%rsp,%rax,4), %eax
	addb	$48, %al
	movb	%al, 39(%rsp)
	movl	$0, 48(%rsp)
	movq	%rsi, %rcx
	movq	%rdi, %rdx
	callq	puts
	incl	44(%rsp)
	cmpl	$4, 44(%rsp)
	jle	.LBB1_5
.LBB1_6:
	xorl	%eax, %eax
	addq	$120, %rsp
	popq	%rdi
	popq	%rsi
	retq
	.seh_endproc

