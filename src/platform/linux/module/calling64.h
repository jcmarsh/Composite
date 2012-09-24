/**
 * Based off of entry_64.S and calling.h in the linux source,
 * and trys to emulate composite's kern_entry.S
 */

#define ASM_PAGE_MASK (~(4096-1)) // I don't know what this does, but it isn't used

/* from calling.h (linux source) */
#define R15		  0  // These are the callee-saved regs
#define R14		  8
#define R13		 16
#define R12		 24
#define RBP		 32
#define RBX		 40

#define R11		 48 // arguments (callee-clobbered)
#define R10		 56
#define R9		 64
#define R8		 72
#define RAX		 80
#define RCX		 88
#define RDX		 96
#define RSI		104
#define RDI		112
#define ORIG_RAX	120 // error cde

#define RIP		128 // cpu exception frame or undefined for fast syscall
#define CS		136
#define EFLAGS		144
#define RSP		152
#define SS		160

#define ARGOFFSET	R11
#define SWFRAME		ORIG_RAX

#define SAVE_ARGS \
	subq	$9*8, %rsp; \
	movq	%rdi, 8*8(%rsp); \
	movq	%rsi, 7*8(%rsp); \
	movq	%rdx, 6*8(%rsp); \
	movq	%rcx, 5*8(%rsp); \
	movq	%rax, 4*8(%rsp); \
	movq	%r8, 3*8(%rsp); \
	movq	%r9, 2*8(%rsp); \
	movq	%r10, 1*8(%rsp); \
	movq	%r11, (%rsp)

#define RESTORE_ARGS \
	movq	(%rsp), %r11; \
	movq	1*8(%rsp), %r10; \
	movq	2*8(%rsp), %r9; \
	movq	3*8(%rsp), %r8; \
	movq	4*8(%rsp), %rax; \
	movq	5*8(%rsp), %rcx; \
	movq	6*8(%rsp), %rdx; \
	movq	7*8(%rsp), %rsi; \
	movq	8*8(%rsp), %rdi; \
	addq	$9*8, %rsp

#define SAVE_REST \
	subq 	$6*8, %rsp; \
	movq 	%rbx, 5*8(%rsp); \
	movq 	%rbp, 4*8(%rsp); \
	movq 	%r12, 3*8(%rsp); \
	movq 	%r13, 2*8(%rsp); \
	movq 	%r14, 1*8(%rsp); \
	movq 	%r15, (%rsp)

#define RESTORE_REST \
	movq	(%rsp), %r15; \
	movq	1*8(%rsp), %r14; \
	movq	2*8(%rsp), %r13; \
	movq	3*8(%rsp), %r12; \
	movq	4*8(%rsp), %rbp; \
	movq	5*8(%rsp), %rbx; \
	addq	$6*8, %rsp

#define SAVE_ALL \
	SAVE_ARGS; \
	SAVE_REST

#define RESTORE_ALL \
	RESTORE_REST; \
	RESTORE_ARGS


#define COS_ENTRY	\
	swapgs;	\
	movq 	%rsp, PER_CPU_VAR(composite_old_rsp); \
	movq	PER_CPU_VAR(kernel_stack), %rsp; \
	pushq	%rcx

#define COS_EXIT	\
	popq	%rcx;				      \
	movq	PER_CPU_VAR(composite_old_rsp), %rsp; \
	swapgs

#define ERROR_OUT \
  movq $0, %rax; \
  movq (%rax), %rax
