#include "../../kernel/include/asm_ipc_defs.h" // Fix this include? -jcm

#ifdef X86_64
/* All un-implemented */
#define RET_CAP  ((1<<20)-1)

.globl SS_ipc_client_marshal_args
.type  SS_ipc_client_marshal_args, @function
.align 16
SS_ipc_client_marshal_args:
	/*
	 * The layout here is important.  If we change the registers
	 * pushed after the stack frame, we need to change the stack
	 * tracking functions.
	 */
	pushq %rsi
	pushq %rdx
	pushq %r10
	pushq %rdi
	pushq %r9
	movq CAPNUM(%rax), %rax /* typeof(%rax) = (usr_inv_cap*) */

	/* arguments */
	movq 48(%rsp), %rsi  /* Used to be 24 here, not sure why -jcm */
	movq 56(%rsp), %rdx
	movq 64(%rsp), %r10
	movq 72(%rsp), %rdi

	movq %rsp, %r9 /* save the stack */
	movq $SS_ipc_client_ret_unmarshal, %r8 /* save the return ip */

	syscall

.align 8
	jmp SS_ipc_client_ret_fatal_err
.globl SS_ipc_client_ret_unmarshal
.align 8
SS_ipc_client_ret_unmarshal:
	jmp SS_ipc_client_ret_unmarshal
	ret
SS_ipc_client_ret_fatal_err:
	ret
#else /* x86_32 implementation */
#define RET_CAP  ((1<<20)-1)

.globl SS_ipc_client_marshal_args
.type  SS_ipc_client_marshal_args, @function
.align 16
SS_ipc_client_marshal_args:
	/*
	 * The layout here is important.  If we change the registers
	 * pushed after the stack frame, we need to change the stack
	 * tracking functions.
	 */
	pushl %ebx
	pushl %esi
	pushl %edi
	pushl %edx
	pushl %ebp
	movl CAPNUM(%eax), %eax /* typeof(%eax) = (usr_inv_cap*) */

	/* arguments */
	movl 24(%esp), %ebx
	movl 28(%esp), %esi
	movl 32(%esp), %edi
	movl 36(%esp), %edx

	movl %esp, %ebp /* save the stack */
	movl $SS_ipc_client_ret_unmarshal, %ecx /* save the return ip */

	sysenter 

.align 8
	jmp SS_ipc_client_ret_fatal_err
.globl SS_ipc_client_ret_unmarshal
.align 8
SS_ipc_client_ret_unmarshal:
	popl %ebp
	popl %edx
	popl %edi
	popl %esi
	popl %ebx
	
	ret
SS_ipc_client_ret_fatal_err:	
	popl %ebp
	popl %edx
	popl %edi
	popl %esi
	popl %ebx
	
	movl $-0xDEAD, %eax
	ret
#endif /* X86_64 */