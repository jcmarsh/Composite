#ifndef COS_ASM_SIMPLE_STACKS_H
#define COS_ASM_SIMPLE_STACKS_H

#ifdef X86_64
/* UNTESTED */
#define COS_ASM_GET_STACK \
  movq $cos_static_stack, %rsp; \
  shl  $12, %rax; \
  addq %rax, %rsp;
#else
#define COS_ASM_GET_STACK                   \
	movl $cos_static_stack, %esp;	    \
	shl $12, %eax;			    \
	addl %eax, %esp;
#endif /*X86_64 */

#define COS_ASM_RET_STACK

#define COS_ASM_REQUEST_STACK

#endif
