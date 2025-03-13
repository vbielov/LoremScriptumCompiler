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
	pushq	%rbx
	.seh_pushreg %rbx
	pushq	%rax
	.seh_stackalloc 8
	movq	%rsp, %rbp
	.seh_setframe %rbp, 0
	.seh_endprologue
	movq	%rcx, %rsi
	cmpl	$0, (%rcx)
	jns	.LBB1_2
	movl	$16, %eax
	callq	__chkstk
	subq	%rax, %rsp
	movq	%rsp, %rcx
	movw	$45, (%rcx)
	subq	$32, %rsp
	callq	printf
	addq	$32, %rsp
	negl	(%rsi)
.LBB1_2:
	movl	$16, %eax
	callq	__chkstk
	subq	%rax, %rsp
	movq	%rsp, %rcx
	movl	$80, %eax
	callq	__chkstk
	subq	%rax, %rsp
	movq	%rsp, %rdi
	movl	$0, (%rcx)
	cmpl	$10, (%rsi)
	jl	.LBB1_5
	.p2align	4, 0x90
.LBB1_4:
	movslq	(%rcx), %rax
	movslq	(%rsi), %rdx
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
	movl	%edx, (%rdi,%rax,4)
	movslq	(%rsi), %rax
	imulq	$1717986919, %rax, %rax
	movq	%rax, %rdx
	shrq	$63, %rdx
	sarq	$34, %rax
	addl	%edx, %eax
	movl	%eax, (%rsi)
	cmpl	$10, (%rsi)
	jge	.LBB1_4
.LBB1_5:
	movl	$16, %eax
	callq	__chkstk
	subq	%rax, %rsp
	movq	%rsp, %rbx
	movslq	(%rcx), %rax
	movl	(%rsi), %edx
	addl	$48, %edx
	movl	%edx, (%rdi,%rax,4)
	movl	(%rcx), %eax
	leal	1(%rax), %edx
	movl	%edx, (%rcx)
	movl	%eax, (%rbx)
	cmpl	$0, (%rbx)
	js	.LBB1_8
	.p2align	4, 0x90
.LBB1_7:
	movl	$16, %eax
	callq	__chkstk
	subq	%rax, %rsp
	movq	%rsp, %rcx
	movl	$16, %eax
	callq	__chkstk
	subq	%rax, %rsp
	movq	%rsp, %rax
	movslq	(%rbx), %rdx
	movl	(%rdi,%rdx,4), %edx
	movl	%edx, (%rax)
	movb	%dl, (%rcx)
	movb	$0, 1(%rcx)
	subq	$32, %rsp
	callq	printf
	addq	$32, %rsp
	decl	(%rbx)
	cmpl	$0, (%rbx)
	jns	.LBB1_7
.LBB1_8:
	movl	$16, %eax
	callq	__chkstk
	subq	%rax, %rsp
	movq	%rsp, %rcx
	movw	$10, (%rcx)
	subq	$32, %rsp
	callq	printf
	nop
	leaq	8(%rbp), %rsp
	popq	%rbx
	popq	%rdi
	popq	%rsi
	popq	%rbp
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
	movl	$2325, 36(%rsp)
	leaq	36(%rsp), %rcx
	callq	scriborNum
	xorl	%eax, %eax
	addq	$40, %rsp
	retq
	.seh_endproc

