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
	.globl	main
	.p2align	4, 0x90
main:
.seh_proc main
	pushq	%rsi
	.seh_pushreg %rsi
	pushq	%rdi
	.seh_pushreg %rdi
	pushq	%rbx
	.seh_pushreg %rbx
	subq	$80, %rsp
	.seh_stackalloc 80
	.seh_endprologue
	movl	$0, 44(%rsp)
	leaq	one(%rip), %rbx
	leaq	char(%rip), %rsi
	leaq	52(%rsp), %rdi
	cmpl	$4, 44(%rsp)
	jg	.LBB1_3
	.p2align	4, 0x90
.LBB1_2:
	movslq	44(%rsp), %rax
	movzbl	(%rbx,%rax,4), %eax
	addb	$48, %al
	movb	%al, char(%rip)
	movl	$0, terminator(%rip)
	movq	%rsi, %rcx
	movq	%rdi, %rdx
	callq	puts
	incl	44(%rsp)
	cmpl	$4, 44(%rsp)
	jle	.LBB1_2
.LBB1_3:
	leaq	one(%rip), %rcx
	leaq	60(%rsp), %rdx
	callq	someFunc
	movl	76(%rsp), %eax
	movups	60(%rsp), %xmm0
	movups	%xmm0, newOne(%rip)
	movl	%eax, newOne+16(%rip)
	movl	$0, 48(%rsp)
	leaq	newOne(%rip), %rbx
	leaq	char(%rip), %rsi
	leaq	56(%rsp), %rdi
	cmpl	$4, 48(%rsp)
	jg	.LBB1_6
	.p2align	4, 0x90
.LBB1_5:
	movslq	48(%rsp), %rax
	movzbl	(%rbx,%rax,4), %eax
	addb	$48, %al
	movb	%al, char(%rip)
	movl	$0, terminator(%rip)
	movq	%rsi, %rcx
	movq	%rdi, %rdx
	callq	puts
	incl	48(%rsp)
	cmpl	$4, 48(%rsp)
	jle	.LBB1_5
.LBB1_6:
	xorl	%eax, %eax
	addq	$80, %rsp
	popq	%rbx
	popq	%rdi
	popq	%rsi
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

