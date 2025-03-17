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

	.def	scriborNewLine;
	.scl	2;
	.type	32;
	.endef
	.globl	scriborNewLine
	.p2align	4, 0x90
scriborNewLine:
.seh_proc scriborNewLine
	subq	$40, %rsp
	.seh_stackalloc 40
	.seh_endprologue
	movw	$10, 38(%rsp)
	leaq	38(%rsp), %rcx
	callq	printf
	nop
	addq	$40, %rsp
	retq
	.seh_endproc

	.def	__scriborNum;
	.scl	2;
	.type	32;
	.endef
	.globl	__scriborNum
	.p2align	4, 0x90
__scriborNum:
.seh_proc __scriborNum
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
	jns	.LBB2_2
	movl	$16, %eax
	callq	___chkstk_ms
	subq	%rax, %rsp
	movq	%rsp, %rcx
	movw	$45, (%rcx)
	subq	$32, %rsp
	callq	printf
	addq	$32, %rsp
	negl	-4(%rbp)
.LBB2_2:
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
	jl	.LBB2_5
	.p2align	4, 0x90
.LBB2_4:
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
	jge	.LBB2_4
.LBB2_5:
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
	js	.LBB2_8
	.p2align	4, 0x90
.LBB2_7:
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
	jns	.LBB2_7
.LBB2_8:
	movq	%rbp, %rsp
	popq	%rdi
	popq	%rsi
	popq	%rbp
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
	subq	$40, %rsp
	.seh_stackalloc 40
	.seh_endprologue
	callq	__scriborNum
	callq	scriborNewLine
	nop
	addq	$40, %rsp
	retq
	.seh_endproc

	.def	scriborNumWithoutNewLine;
	.scl	2;
	.type	32;
	.endef
	.globl	scriborNumWithoutNewLine
	.p2align	4, 0x90
scriborNumWithoutNewLine:
.seh_proc scriborNumWithoutNewLine
	subq	$40, %rsp
	.seh_stackalloc 40
	.seh_endprologue
	callq	__scriborNum
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
	pushq	%rsi
	.seh_pushreg %rsi
	subq	$40, %rsp
	.seh_stackalloc 40
	leaq	32(%rsp), %rbp
	.seh_setframe %rbp, 32
	.seh_endprologue
	callq	__main
	movabsq	$25769803781, %rax
	movq	%rax, point(%rip)
	leaq	point(%rip), %rcx
	leaq	point+4(%rip), %rsi
	callq	scriborNum
	movq	%rsi, %rcx
	callq	scriborNum
	xorl	%eax, %eax
	addq	$40, %rsp
	popq	%rsi
	popq	%rbp
	retq
	.seh_endproc

	.bss
	.weak	point
	.p2align	3, 0x0
point:
	.quad	0

