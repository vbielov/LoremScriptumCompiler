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

	.def	someFunc;
	.scl	2;
	.type	32;
	.endef
	.globl	someFunc
	.p2align	4, 0x90
someFunc:
.seh_proc someFunc
	subq	$48, %rsp
	.seh_stackalloc 48
	.seh_endprologue
	movl	$0, 4(%rsp)
	cmpl	$9, 4(%rsp)
	jg	.LBB1_3
	.p2align	4, 0x90
.LBB1_2:
	movslq	4(%rsp), %rax
	movl	$10, %r8d
	subl	(%rcx,%rax,4), %r8d
	movl	%r8d, 8(%rsp,%rax,4)
	incl	%eax
	movl	%eax, 4(%rsp)
	cmpl	$9, 4(%rsp)
	jle	.LBB1_2
.LBB1_3:
	movups	8(%rsp), %xmm0
	movups	24(%rsp), %xmm1
	movq	40(%rsp), %rax
	movq	%rax, 32(%rdx)
	movups	%xmm1, 16(%rdx)
	movups	%xmm0, (%rdx)
	addq	$48, %rsp
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
	pushq	%rsi
	.seh_pushreg %rsi
	pushq	%rdi
	.seh_pushreg %rdi
	subq	$16, %rsp
	.seh_stackalloc 16
	leaq	16(%rsp), %rbp
	.seh_setframe %rbp, 16
	.seh_endprologue
	movl	$0, -4(%rbp)
	leaq	one(%rip), %rsi
	cmpl	$9, -4(%rbp)
	jg	.LBB2_3
	.p2align	4, 0x90
.LBB2_2:
	movslq	-4(%rbp), %rax
	movl	%eax, (%rsi,%rax,4)
	incl	%eax
	movl	%eax, -4(%rbp)
	cmpl	$9, -4(%rbp)
	jle	.LBB2_2
.LBB2_3:
	movl	$16, %eax
	callq	__chkstk
	subq	%rax, %rsp
	movq	%rsp, %rdi
	movl	$0, (%rdi)
	cmpl	$9, (%rdi)
	jg	.LBB2_6
	.p2align	4, 0x90
.LBB2_5:
	movl	$16, %eax
	callq	__chkstk
	subq	%rax, %rsp
	movq	%rsp, %rcx
	movl	$16, %eax
	callq	__chkstk
	subq	%rax, %rsp
	movq	%rsp, %rax
	movl	$0, (%rax)
	movslq	(%rdi), %rax
	movzbl	(%rsi,%rax,4), %eax
	addb	$48, %al
	movb	%al, (%rcx)
	subq	$32, %rsp
	callq	puts
	addq	$32, %rsp
	incl	(%rdi)
	cmpl	$9, (%rdi)
	jle	.LBB2_5
.LBB2_6:
	movl	$16, %eax
	callq	__chkstk
	subq	%rax, %rsp
	movq	%rsp, %rdi
	movl	$48, %eax
	callq	__chkstk
	subq	%rax, %rsp
	movq	%rsp, %rsi
	subq	$32, %rsp
	leaq	one(%rip), %rcx
	movq	%rsi, %rdx
	callq	someFunc
	addq	$32, %rsp
	movq	32(%rsi), %rax
	movups	(%rsi), %xmm0
	movups	16(%rsi), %xmm1
	movups	%xmm0, newOne(%rip)
	movups	%xmm1, newOne+16(%rip)
	movq	%rax, newOne+32(%rip)
	movl	$0, (%rdi)
	leaq	newOne(%rip), %rsi
	cmpl	$9, (%rdi)
	jg	.LBB2_9
	.p2align	4, 0x90
.LBB2_8:
	movl	$16, %eax
	callq	__chkstk
	subq	%rax, %rsp
	movq	%rsp, %rcx
	movl	$16, %eax
	callq	__chkstk
	subq	%rax, %rsp
	movq	%rsp, %rax
	movl	$0, (%rax)
	movslq	(%rdi), %rax
	movzbl	(%rsi,%rax,4), %eax
	addb	$48, %al
	movb	%al, (%rcx)
	subq	$32, %rsp
	callq	puts
	addq	$32, %rsp
	incl	(%rdi)
	cmpl	$9, (%rdi)
	jle	.LBB2_8
.LBB2_9:
	xorl	%eax, %eax
	movq	%rbp, %rsp
	popq	%rdi
	popq	%rsi
	popq	%rbp
	retq
	.seh_endproc

	.bss
	.weak	one
	.p2align	4, 0x0
one:
	.quad	0

	.weak	newOne
	.p2align	4, 0x0
newOne:
	.quad	0

