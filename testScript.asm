	.text
	.file	"testScript"
	.globl	scriborNum
	.p2align	4, 0x90
	.type	scriborNum,@function
scriborNum:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%r14
	pushq	%rbx
	subq	$16, %rsp
	.cfi_offset %rbx, -32
	.cfi_offset %r14, -24
	movl	(%rdi), %eax
	movl	%eax, -20(%rbp)
	testl	%eax, %eax
	jns	.LBB0_2
	movq	%rsp, %rax
	leaq	-16(%rax), %rdi
	movq	%rdi, %rsp
	movw	$45, -16(%rax)
	callq	printf@PLT
	negl	-20(%rbp)
.LBB0_2:
	movq	%rsp, %rcx
	leaq	-16(%rcx), %rax
	movq	%rax, %rsp
	movq	%rsp, %rbx
	addq	$-80, %rbx
	movq	%rbx, %rsp
	movl	$0, -16(%rcx)
	cmpl	$10, -20(%rbp)
	jl	.LBB0_5
	.p2align	4, 0x90
.LBB0_4:
	movslq	(%rax), %rcx
	movslq	-20(%rbp), %rdx
	imulq	$1717986919, %rdx, %rsi
	movq	%rsi, %rdi
	shrq	$63, %rdi
	sarq	$34, %rsi
	addl	%edi, %esi
	addl	%esi, %esi
	leal	(%rsi,%rsi,4), %esi
	subl	%esi, %edx
	addl	$48, %edx
	incl	(%rax)
	movl	%edx, (%rbx,%rcx,4)
	movslq	-20(%rbp), %rcx
	imulq	$1717986919, %rcx, %rcx
	movq	%rcx, %rdx
	shrq	$63, %rdx
	sarq	$34, %rcx
	addl	%edx, %ecx
	movl	%ecx, -20(%rbp)
	cmpl	$10, -20(%rbp)
	jge	.LBB0_4
.LBB0_5:
	movq	%rsp, %rcx
	leaq	-16(%rcx), %r14
	movq	%r14, %rsp
	movslq	(%rax), %rdx
	movl	-20(%rbp), %esi
	addl	$48, %esi
	movl	%esi, (%rbx,%rdx,4)
	movl	(%rax), %edx
	leal	1(%rdx), %esi
	movl	%esi, (%rax)
	movl	%edx, -16(%rcx)
	cmpl	$0, (%r14)
	js	.LBB0_8
	.p2align	4, 0x90
.LBB0_7:
	movq	%rsp, %rax
	leaq	-16(%rax), %rdi
	movq	%rdi, %rsp
	movslq	(%r14), %rcx
	movzbl	(%rbx,%rcx,4), %ecx
	movb	%cl, -16(%rax)
	movb	$0, -15(%rax)
	callq	printf@PLT
	decl	(%r14)
	cmpl	$0, (%r14)
	jns	.LBB0_7
.LBB0_8:
	movq	%rsp, %rax
	leaq	-16(%rax), %rdi
	movq	%rdi, %rsp
	movw	$10, -16(%rax)
	callq	printf@PLT
	leaq	-16(%rbp), %rsp
	popq	%rbx
	popq	%r14
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.Lfunc_end0:
	.size	scriborNum, .Lfunc_end0-scriborNum
	.cfi_endproc

	.globl	main
	.p2align	4, 0x90
	.type	main,@function
main:
	.cfi_startproc
	pushq	%rax
	.cfi_def_cfa_offset 16
	movl	$-2325, 4(%rsp)
	leaq	4(%rsp), %rdi
	callq	scriborNum@PLT
	xorl	%eax, %eax
	popq	%rcx
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end1:
	.size	main, .Lfunc_end1-main
	.cfi_endproc

	.section	".note.GNU-stack","",@progbits
