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
	callq	__chkstk
	subq	%rax, %rsp
	movq	%rsp, %rcx
	movw	$45, (%rcx)
	subq	$32, %rsp
	callq	printf
	addq	$32, %rsp
	negl	-4(%rbp)
.LBB2_2:
	movl	$16, %eax
	callq	__chkstk
	subq	%rax, %rsp
	movq	%rsp, %rcx
	movl	$80, %eax
	callq	__chkstk
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
	callq	__chkstk
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
	callq	__chkstk
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

	.def	createBinaryTree;
	.scl	2;
	.type	32;
	.endef
	.globl	createBinaryTree
	.p2align	4, 0x90
createBinaryTree:
.seh_proc createBinaryTree
	pushq	%rax
	.seh_stackalloc 8
	.seh_endprologue
	movl	(%r8), %eax
	movl	%eax, (%rcx)
	movl	(%rdx), %eax
	movl	%eax, 4(%rcx)
	movl	$2, 4(%rsp)
	.p2align	4, 0x90
.LBB5_1:
	movl	4(%rsp), %eax
	cmpl	(%rdx), %eax
	jge	.LBB5_3
	movslq	4(%rsp), %rax
	movl	(%r8), %r9d
	movl	%r9d, (%rcx,%rax,4)
	incl	4(%rsp)
	jmp	.LBB5_1
.LBB5_3:
	popq	%rax
	retq
	.seh_endproc

	.def	__getMiddle;
	.scl	2;
	.type	32;
	.endef
	.globl	__getMiddle
	.p2align	4, 0x90
__getMiddle:
	movl	(%rdx), %eax
	movl	(%rcx), %ecx
	subl	%ecx, %eax
	movl	%eax, %edx
	shrl	$31, %edx
	addl	%eax, %edx
	sarl	%edx
	addl	%ecx, %edx
	movl	%edx, (%r8)
	retq

	.def	__addValue;
	.scl	2;
	.type	32;
	.endef
	.globl	__addValue
	.p2align	4, 0x90
__addValue:
.seh_proc __addValue
	pushq	%rbp
	.seh_pushreg %rbp
	pushq	%r15
	.seh_pushreg %r15
	pushq	%r14
	.seh_pushreg %r14
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
	movq	%r9, %rsi
	movq	%r8, %rbx
	movq	%rdx, %r14
	movq	%rcx, %rdi
	movq	96(%rbp), %r15
	subq	$32, %rsp
	movq	%rbp, %r8
	movq	%rdx, %rcx
	movq	%rbx, %rdx
	callq	__getMiddle
	addq	$32, %rsp
	movslq	(%rbp), %rax
	movl	%eax, 4(%rbp)
	movl	(%rdi), %ecx
	cmpl	(%rdi,%rax,4), %ecx
	jne	.LBB7_2
	movslq	4(%rbp), %rax
	movl	(%rsi), %ecx
	movl	%ecx, (%rdi,%rax,4)
	movb	$1, (%r15)
	jmp	.LBB7_4
.LBB7_2:
	movl	4(%rbp), %eax
	cmpl	(%rbx), %eax
	je	.LBB7_3
	movl	4(%rbp), %eax
	cmpl	(%r14), %eax
	jne	.LBB7_6
.LBB7_3:
	movb	$0, (%r15)
.LBB7_4:
	leaq	8(%rbp), %rsp
	popq	%rbx
	popq	%rdi
	popq	%rsi
	popq	%r14
	popq	%r15
	popq	%rbp
	retq
.LBB7_6:
	movslq	4(%rbp), %rax
	movl	(%rdi,%rax,4), %eax
	cmpl	(%rsi), %eax
	jle	.LBB7_9
	movl	$16, %eax
	callq	__chkstk
	subq	%rax, %rsp
	movq	%rsp, %rbx
	movl	$16, %eax
	callq	__chkstk
	subq	%rax, %rsp
	movq	%rsp, %r8
	movl	4(%rbp), %eax
	decl	%eax
	movl	%eax, (%r8)
	subq	$48, %rsp
	movq	%rbx, 32(%rsp)
	movq	%rdi, %rcx
	movq	%r14, %rdx
	movq	%rsi, %r9
	callq	__addValue
	addq	$48, %rsp
	movzbl	(%rbx), %eax
	movb	%al, (%r15)
	jmp	.LBB7_4
.LBB7_9:
	movslq	4(%rbp), %rax
	movl	(%rdi,%rax,4), %eax
	cmpl	(%rsi), %eax
	jge	.LBB7_3
	movl	$16, %eax
	callq	__chkstk
	subq	%rax, %rsp
	movq	%rsp, %r14
	movl	$16, %eax
	callq	__chkstk
	subq	%rax, %rsp
	movq	%rsp, %rdx
	movl	4(%rbp), %eax
	incl	%eax
	movl	%eax, (%rdx)
	subq	$48, %rsp
	movq	%r14, 32(%rsp)
	movq	%rdi, %rcx
	movq	%rbx, %r8
	movq	%rsi, %r9
	callq	__addValue
	addq	$48, %rsp
	movzbl	(%r14), %eax
	movb	%al, (%r15)
	jmp	.LBB7_4
	.seh_endproc

	.def	addValue;
	.scl	2;
	.type	32;
	.endef
	.globl	addValue
	.p2align	4, 0x90
addValue:
.seh_proc addValue
	pushq	%rsi
	.seh_pushreg %rsi
	subq	$64, %rsp
	.seh_stackalloc 64
	.seh_endprologue
	movq	%r8, %rsi
	movq	%rdx, %r9
	movl	$2, 56(%rsp)
	movl	4(%rcx), %eax
	decl	%eax
	movl	%eax, 60(%rsp)
	leaq	55(%rsp), %rax
	movq	%rax, 32(%rsp)
	leaq	56(%rsp), %rdx
	leaq	60(%rsp), %r8
	callq	__addValue
	movzbl	55(%rsp), %eax
	movb	%al, (%rsi)
	addq	$64, %rsp
	popq	%rsi
	retq
	.seh_endproc

	.def	__getnextBounds;
	.scl	2;
	.type	32;
	.endef
	.globl	__getnextBounds
	.p2align	4, 0x90
__getnextBounds:
.seh_proc __getnextBounds
	pushq	%rbp
	.seh_pushreg %rbp
	pushq	%r15
	.seh_pushreg %r15
	pushq	%r14
	.seh_pushreg %r14
	pushq	%r12
	.seh_pushreg %r12
	pushq	%rsi
	.seh_pushreg %rsi
	pushq	%rdi
	.seh_pushreg %rdi
	pushq	%rbx
	.seh_pushreg %rbx
	movq	%rsp, %rbp
	.seh_setframe %rbp, 0
	.seh_endprologue
	movl	(%rcx), %eax
	cmpl	(%rdx), %eax
	je	.LBB9_6
	movq	%r8, %rsi
	movq	%rdx, %rdi
	movl	$16, %eax
	callq	__chkstk
	subq	%rax, %rsp
	movq	%rsp, %r15
	movl	$16, %eax
	callq	__chkstk
	subq	%rax, %rsp
	movq	%rsp, %rbx
	movl	$16, %eax
	callq	__chkstk
	subq	%rax, %rsp
	movq	%rsp, %r12
	subq	$32, %rsp
	movq	%rcx, %r14
	movq	%rbx, %r8
	callq	__getMiddle
	addq	$32, %rsp
	movl	(%rbx), %eax
	movl	%eax, (%r12)
	movl	(%r14), %eax
	movl	%eax, (%r15)
	movl	(%r12), %eax
	decl	%eax
	movl	%eax, 4(%r15)
	movl	(%r12), %eax
	incl	%eax
	movl	%eax, 8(%r15)
	movl	(%rdi), %eax
	movl	%eax, 12(%r15)
	movl	(%r12), %eax
	cmpl	(%r14), %eax
	jne	.LBB9_3
	movq	$0, (%r15)
	jmp	.LBB9_5
.LBB9_3:
	movl	(%r12), %eax
	cmpl	(%rdi), %eax
	jne	.LBB9_5
	movq	$0, 8(%r15)
.LBB9_5:
	movq	(%r15), %rax
	movq	%rax, (%rsi)
	movq	8(%r15), %rax
	movq	%rax, 8(%rsi)
.LBB9_6:
	movq	%rbp, %rsp
	popq	%rbx
	popq	%rdi
	popq	%rsi
	popq	%r12
	popq	%r14
	popq	%r15
	popq	%rbp
	retq
	.seh_endproc

	.def	scriborTree;
	.scl	2;
	.type	32;
	.endef
	.globl	scriborTree
	.p2align	4, 0x90
scriborTree:
.seh_proc scriborTree
	pushq	%rbp
	.seh_pushreg %rbp
	pushq	%r15
	.seh_pushreg %r15
	pushq	%r14
	.seh_pushreg %r14
	pushq	%r13
	.seh_pushreg %r13
	pushq	%r12
	.seh_pushreg %r12
	pushq	%rsi
	.seh_pushreg %rsi
	pushq	%rdi
	.seh_pushreg %rdi
	pushq	%rbx
	.seh_pushreg %rbx
	subq	$1608, %rsp
	.seh_stackalloc 1608
	leaq	128(%rsp), %rbp
	.seh_setframe %rbp, 128
	.seh_endprologue
	movq	%rcx, %rsi
	movl	$100, 1472(%rbp)
	movl	$0, 1476(%rbp)
	.p2align	4, 0x90
.LBB10_1:
	movl	1476(%rbp), %eax
	cmpl	1472(%rbp), %eax
	jge	.LBB10_3
	movslq	1476(%rbp), %rax
	movl	$0, 1072(%rbp,%rax,4)
	movl	$0, 672(%rbp,%rax,4)
	movl	$0, 272(%rbp,%rax,4)
	movl	$0, -128(%rbp,%rax,4)
	incl	%eax
	movl	%eax, 1476(%rbp)
	jmp	.LBB10_1
.LBB10_3:
	movl	$2, 1072(%rbp)
	movl	4(%rsi), %eax
	decl	%eax
	movl	%eax, 672(%rbp)
	.p2align	4, 0x90
.LBB10_4:
	cmpl	$0, 1072(%rbp)
	je	.LBB10_17
	movl	$16, %eax
	callq	__chkstk
	subq	%rax, %rsp
	movq	%rsp, %r12
	movl	$16, %eax
	callq	__chkstk
	subq	%rax, %rsp
	movq	%rsp, %r15
	movl	$0, (%r15)
	movl	$0, (%r12)
	jmp	.LBB10_6
	.p2align	4, 0x90
.LBB10_13:
	movslq	(%r12), %rax
	movl	$0, 1072(%rbp,%rax,4)
	movslq	(%r12), %rax
	movl	$0, 672(%rbp,%rax,4)
	incl	(%r12)
.LBB10_6:
	movl	(%r12), %eax
	cmpl	1472(%rbp), %eax
	jge	.LBB10_14
	movslq	(%r12), %rax
	cmpl	$0, 1072(%rbp,%rax,4)
	je	.LBB10_14
	movl	$16, %eax
	callq	__chkstk
	subq	%rax, %rsp
	movq	%rsp, %rdi
	movl	$16, %eax
	callq	__chkstk
	subq	%rax, %rsp
	movq	%rsp, %rbx
	movl	$16, %eax
	callq	__chkstk
	subq	%rax, %rsp
	movq	%rsp, %r14
	movslq	(%r12), %rax
	leaq	1072(,%rax,4), %rcx
	addq	%rbp, %rcx
	leaq	672(,%rax,4), %rdx
	addq	%rbp, %rdx
	subq	$32, %rsp
	movq	%rbx, %r8
	callq	__getMiddle
	addq	$32, %rsp
	movslq	(%rbx), %rax
	movl	%eax, (%r14)
	movl	(%rsi,%rax,4), %eax
	movl	%eax, (%rdi)
	cmpl	(%rsi), %eax
	je	.LBB10_13
	movl	$16, %eax
	callq	__chkstk
	subq	%rax, %rsp
	movq	%rsp, %rbx
	movl	$16, %eax
	callq	__chkstk
	subq	%rax, %rsp
	movq	%rsp, %r13
	movl	$16, %eax
	callq	__chkstk
	subq	%rax, %rsp
	movq	%rsp, %r14
	subq	$32, %rsp
	movq	%rdi, %rcx
	callq	scriborNumWithoutNewLine
	addq	$32, %rsp
	movw	$2336, (%r14)
	movb	$0, 2(%r14)
	subq	$32, %rsp
	movq	%r14, %rcx
	callq	printf
	addq	$32, %rsp
	movslq	(%r12), %rax
	leaq	1072(,%rax,4), %rcx
	addq	%rbp, %rcx
	leaq	672(,%rax,4), %rdx
	addq	%rbp, %rdx
	subq	$32, %rsp
	movq	%rbx, %r8
	callq	__getnextBounds
	addq	$32, %rsp
	movups	(%rbx), %xmm0
	movups	%xmm0, (%r13)
	cmpl	$0, (%r13)
	je	.LBB10_11
	movslq	(%r15), %rax
	movl	(%r13), %ecx
	movl	%ecx, 272(%rbp,%rax,4)
	movslq	(%r15), %rax
	movl	4(%r13), %ecx
	movl	%ecx, -128(%rbp,%rax,4)
	incl	(%r15)
.LBB10_11:
	cmpl	$0, 8(%r13)
	je	.LBB10_13
	movslq	(%r15), %rax
	movl	8(%r13), %ecx
	movl	%ecx, 272(%rbp,%rax,4)
	movslq	(%r15), %rax
	movl	12(%r13), %ecx
	movl	%ecx, -128(%rbp,%rax,4)
	incl	(%r15)
	jmp	.LBB10_13
	.p2align	4, 0x90
.LBB10_14:
	movl	$16, %eax
	callq	__chkstk
	subq	%rax, %rsp
	movq	%rsp, %rdi
	subq	$32, %rsp
	callq	scriborNewLine
	addq	$32, %rsp
	movl	$0, (%rdi)
	.p2align	4, 0x90
.LBB10_15:
	movl	(%rdi), %eax
	cmpl	(%r15), %eax
	jge	.LBB10_4
	movslq	(%rdi), %rax
	movl	272(%rbp,%rax,4), %ecx
	movl	%ecx, 1072(%rbp,%rax,4)
	movslq	(%rdi), %rax
	movl	-128(%rbp,%rax,4), %ecx
	movl	%ecx, 672(%rbp,%rax,4)
	incl	(%rdi)
	jmp	.LBB10_15
.LBB10_17:
	leaq	1480(%rbp), %rsp
	popq	%rbx
	popq	%rdi
	popq	%rsi
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
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
	pushq	%rsi
	.seh_pushreg %rsi
	subq	$64, %rsp
	.seh_stackalloc 64
	.seh_endprologue
	movl	$100, 40(%rsp)
	movl	$0, 44(%rsp)
	leaq	tree(%rip), %rsi
	leaq	40(%rsp), %rdx
	leaq	44(%rsp), %r8
	movq	%rsi, %rcx
	callq	createBinaryTree
	movl	$10, 48(%rsp)
	leaq	48(%rsp), %rdx
	leaq	36(%rsp), %r8
	movq	%rsi, %rcx
	callq	addValue
	movl	$5, 52(%rsp)
	leaq	52(%rsp), %rdx
	leaq	37(%rsp), %r8
	movq	%rsi, %rcx
	callq	addValue
	movl	$7, 56(%rsp)
	leaq	56(%rsp), %rdx
	leaq	38(%rsp), %r8
	movq	%rsi, %rcx
	callq	addValue
	movl	$20, 60(%rsp)
	leaq	60(%rsp), %rdx
	leaq	39(%rsp), %r8
	movq	%rsi, %rcx
	callq	addValue
	movq	%rsi, %rcx
	callq	scriborTree
	xorl	%eax, %eax
	addq	$64, %rsp
	popq	%rsi
	retq
	.seh_endproc

	.bss
	.weak	tree
	.p2align	4, 0x0
tree:
	.quad	0

	.weak	success
success:
	.quad	0

