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

	.def	scriborNum;
	.scl	2;
	.type	32;
	.endef
	.globl	scriborNum
	.p2align	4, 0x90
scriborNum:
.seh_proc scriborNum
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
	movl	(%rcx), %eax
	movl	%eax, -4(%rbp)
	testl	%eax, %eax
	jns	.LBB1_2
	movl	$16, %eax
	callq	___chkstk_ms
	subq	%rax, %rsp
	movq	%rsp, %rcx
	movw	$45, (%rcx)
	subq	$32, %rsp
	callq	printf
	addq	$32, %rsp
	negl	-4(%rbp)
.LBB1_2:
	movl	$16, %eax
	callq	___chkstk_ms
	subq	%rax, %rsp
	movq	%rsp, %rcx
	movl	$80, %eax
	callq	___chkstk_ms
	subq	%rax, %rsp
	movq	%rsp, %rsi
	movl	$0, (%rcx)
	cmpl	$10, -4(%rbp)
	jl	.LBB1_5
	.p2align	4, 0x90
.LBB1_4:
	movslq	(%rcx), %rax
	movslq	-4(%rbp), %rdx
	imulq	$1717986919, %rdx, %r8
	movq	%r8, %r9
	shrq	$63, %r9
	sarq	$34, %r8
	addl	%r9d, %r8d
	addl	%r8d, %r8d
	leal	(%r8,%r8,4), %r8d
	subl	%r8d, %edx
	addl	$48, %edx
	incl	(%rcx)
	movl	%edx, (%rsi,%rax,4)
	movslq	-4(%rbp), %rax
	imulq	$1717986919, %rax, %rax
	movq	%rax, %rdx
	shrq	$63, %rdx
	sarq	$34, %rax
	addl	%edx, %eax
	movl	%eax, -4(%rbp)
	cmpl	$10, -4(%rbp)
	jge	.LBB1_4
.LBB1_5:
	movl	$16, %eax
	callq	___chkstk_ms
	subq	%rax, %rsp
	movq	%rsp, %rdi
	movslq	(%rcx), %rax
	movl	-4(%rbp), %edx
	addl	$48, %edx
	movl	%edx, (%rsi,%rax,4)
	movl	(%rcx), %eax
	leal	1(%rax), %edx
	movl	%edx, (%rcx)
	movl	%eax, (%rdi)
	cmpl	$0, (%rdi)
	js	.LBB1_8
	.p2align	4, 0x90
.LBB1_7:
	movl	$16, %eax
	callq	___chkstk_ms
	subq	%rax, %rsp
	movq	%rsp, %rcx
	movslq	(%rdi), %rax
	movzbl	(%rsi,%rax,4), %eax
	movb	%al, (%rcx)
	movb	$0, 1(%rcx)
	subq	$32, %rsp
	callq	printf
	addq	$32, %rsp
	decl	(%rdi)
	cmpl	$0, (%rdi)
	jns	.LBB1_7
.LBB1_8:
	movl	$16, %eax
	callq	___chkstk_ms
	subq	%rax, %rsp
	movq	%rsp, %rcx
	movw	$10, (%rcx)
	subq	$32, %rsp
	callq	printf
	nop
	movq	%rbp, %rsp
	popq	%rdi
	popq	%rsi
	popq	%rbp
	retq
	.seh_endproc

	.def	foo;
	.scl	2;
	.type	32;
	.endef
	.globl	foo
	.p2align	4, 0x90
foo:
.seh_proc foo
	subq	$40, %rsp
	.seh_stackalloc 40
	.seh_endprologue
	callq	scriborNum
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
	subq	$48, %rsp
	.seh_stackalloc 48
	leaq	48(%rsp), %rbp
	.seh_setframe %rbp, 48
	.seh_endprologue
	callq	__main
	movl	$500, -4(%rbp)
	leaq	-4(%rbp), %rcx
	callq	foo
	movl	$500, -8(%rbp)
	leaq	-8(%rbp), %rcx
	callq	scriborNum
	xorl	%eax, %eax
	addq	$48, %rsp
	popq	%rbp
	retq
	.seh_endproc

