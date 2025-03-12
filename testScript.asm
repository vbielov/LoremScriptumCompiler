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
	incl	(%rsp)
	cmpl	$4, (%rsp)
	jle	.LBB0_2
.LBB0_3:
	leaq	4(%rsp), %rax
	movq	%rax, (%rdx)
	addq	$24, %rsp
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
	pushq	%rbx
	.seh_pushreg %rbx
	pushq	%rax
	.seh_stackalloc 8
	movq	%rsp, %rbp
	.seh_setframe %rbp, 0
	.seh_endprologue
	movl	$0, 4(%rbp)
	leaq	one(%rip), %rdi
	leaq	char(%rip), %rsi
	cmpl	$4, 4(%rbp)
	jg	.LBB1_3
	.p2align	4, 0x90
.LBB1_2:
	movslq	4(%rbp), %rax
	movzbl	(%rdi,%rax,4), %eax
	addb	$48, %al
	movb	%al, char(%rip)
	movl	$0, terminator(%rip)
	subq	$32, %rsp
	movq	%rsi, %rcx
	callq	puts
	addq	$32, %rsp
	incl	4(%rbp)
	cmpl	$4, 4(%rbp)
	jle	.LBB1_2
.LBB1_3:
	movl	$16, %eax
	callq	__chkstk
	subq	%rax, %rsp
	movq	%rsp, %rdi
	movl	$0, (%rdi)
	leaq	newOne(%rip), %rbx
	leaq	char(%rip), %rsi
	cmpl	$4, (%rdi)
	jg	.LBB1_6
	.p2align	4, 0x90
.LBB1_5:
	movslq	(%rdi), %rax
	movzbl	(%rbx,%rax,4), %eax
	addb	$48, %al
	movb	%al, char(%rip)
	movl	$0, terminator(%rip)
	subq	$32, %rsp
	movq	%rsi, %rcx
	callq	puts
	addq	$32, %rsp
	incl	(%rdi)
	cmpl	$4, (%rdi)
	jle	.LBB1_5
.LBB1_6:
	xorl	%eax, %eax
	leaq	8(%rbp), %rsp
	popq	%rbx
	popq	%rdi
	popq	%rsi
	popq	%rbp
	retq
	.seh_endproc

	.data
	.weak	one
	.p2align	4, 0x0
one:
	.long	0
	.long	1
	.long	2
	.long	3
	.long	4

	.bss
	.weak	newOne
	.p2align	4, 0x0
newOne:
	.quad	0

	.weak	char
char:
	.quad	0

	.weak	terminator
	.p2align	2, 0x0
terminator:
	.quad	0

