/**
 * Copyright 2008 by Gabriel Parmer, gabep1@cs.bu.edu.  All rights
 * reserved.
 *
 * Redistribution of this file is permitted under the GNU General
 * Public License v2.
 */

#include <asm/asm-offsets.h>
#include <asm/segment.h>
#include <asm/thread_info.h>
#include <linux/linkage.h>
#include "asym_exec_dom.h"
#include "../../../kernel/include/asm_ipc_defs.h"

#ifdef X86_64
#include "calling64.h"
#define AX %rax
#define CMP cmpq
#else
#include "calling32.h"
#define AX %eax
#define CMP cmpl
#endif /* X86_64 */

#define ASM_PAGE_MASK (~(4096-1))

/* 
 * This is all information that is needed within this file, and
 * external to this file.  It is here, so that it is easier to
 * concurrently use hijack modules and composite modules.
 */
.data
.align 16
//.globl cos_sysenter_addr
//cos_sysenter_addr:	
//	.long 0
//.globl cos_temp_esp_storage
//cos_temp_esp_storage:	
//	.long 0

.text
ALIGN
ENTRY(sysenter_interposition_entry)
	COS_ENTRY

	/*
	 * Composite invocations are indicated by the contents of %eax:
	 * +-------------+----+--...
	 * |   cap_inv   | sc | normal OS (linux) syscalls
	 * +-------------+----+--...
	 * 32            |    COS_SYSCALL_OFFSET
	 *               COS_CAPABILITY_OFFSET
 	 *	
	 * Where each character represents a bit.
	 * cap_inv:	The capability to invoke
	 * sc:	 Make a composite OS system-call
	 */
	/*
	 * Composite invocations are indicated byt the contents of %rax:
	 * +--...---+-------------+----+--...
	 * | unused |   cap_inv   | sc | normal OS (linux) syscalls
	 * +--...---+-------------+----+--...
	 * 64       32            |    COS_SYSCALL_OFFSET
	 *                        COS_CAPABILITY_OFFSET
	 *
	 * Where each character represents a bit.
	 * cap_inv:	The capability to invoke
	 * sc:	Make a composite OS system-call
	 */


	CMP $((1<<COS_CAPABILITY_OFFSET)-1), AX /* 2^20-1 shift for capability invocations */
	ja composite_call_ipc /* component invocation */
	je composite_ret_ipc  /* component return */

	CMP $((1<<COS_SYSCALL_OFFSET)-1), AX /* composite syscall */
	ja cos_syscall_thunk

#ifdef X86_64
normal_syscall:
	/* I'm not sure that I need to mess with the TSS */
	/* Will investigate later */
	COS_EXIT
	jmp	*(cos_default_sysenter_addr)

cos_syscall_thunk:
	/* cos syscalls may call linux functions... not supported yet */
	jmp composite_make_syscall

composite_call_ipc:
	/* not yet implemented */
	call test_function2
	jmp cos_exit

composite_ret_ipc:
	/* not yet implemented */
	call test_function3
	jmp cos_exit

.globl cos_exit
cos_exit:
	COS_EXIT
	sysretq

/* All of these are not implemented in x86_64 */
cos_page_fault_page_tramp:
.globl cos_default_page_fault_handler_tramp
cos_default_page_fault_handler_tramp:
.long 0
.globl cos_interpose_page_fault_handler_tramp
cos_interpose_page_fault_handler_tramp:
.long 0
.globl page_fault_interposition_tramp
.align 16
page_fault_interposition_tramp:
.globl cos_post_interpose_deref_addr_tramp
cos_post_interpose_deref_addr_tramp: /* for the previous address */
1:
.globl cos_post_default_deref_addr_tramp
cos_post_default_deref_addr_tramp:   /* for the previous address */
	jmp cos_exit

#else /* X86_32 Implementation begins here! */
	asdfjsffiuiuiuiu
	SAVE_ALL
	pushl %eax
	call hijack_syscall_monitor
	addl $4, %esp
	RESTORE_REGS
normal_syscall:
	/* We reserve the position in the stack to get the TSS. */
	/* see the comments of get_TSS in hijack.c */	
	pushl $0
	SAVE_ALL
	movl %esp, %eax 	
	call get_TSS
	RESTORE_REGS
	popl %esp
	jmp  *(cos_default_sysenter_addr) /* sti will happen in syscall path */

	/* never get to here on syscall...*/

/* cos system calls might call linux functions, so we need fs */
cos_syscall_thunk:
	pushl %edx
	movl $(__KERNEL_PERCPU), %edx
	movl %edx, %fs
	popl %edx
	jmp composite_make_syscall

/*
 * Trap will cause this handler to trigger.  The stack
 * already contains cs, ds, flags, sp, eip, and 0.  We will save
 * registers, call a composite handler, decide if we can deal with it
 * there, or if we should call the linux handler, and if so,
 * return the machine state (stack etc) to how it was when the
 * trap occurred, and call the linux handler.
 */
#define COS_CREATE_TRAP_NO_ERRCODE(name)	 		\
.data;								\
.globl cos_default_##name##_handler; 				\
cos_default_##name##_handler:					\
.long 0;							\
.text;								\
.globl name##_interposition;	 				\
.align 16;					 		\
ENTRY(name##_interposition)					\
	pushl $0;						\
	SAVE_ALL; 						\
	movl %esp, %eax; 					\
	movl ORIG_EAX(%eax), %edx;				\
	call main_##name##_interposition;			\
	test %eax, %eax;					\
	jnz 1f;							\
	RESTORE_REGS;						\
	addl $4, %esp;						\
	iret;							\
1:	/* linux path */					\
	RESTORE_REGS;						\
	addl $8, %esp;						\
	popl %fs;						\
	popl %gs;						\
	addl $4, %esp;						\
	jmp *(cos_default_##name##_handler)	
	
/* Same as above, but this time, there is an errorcode provided by the HW */
#define COS_CREATE_TRAP_ERRCODE(name)		 		\
.data;								\
.globl cos_default_##name##_handler; 				\
cos_default_##name##_handler:					\
.long 0;							\
.text;								\
.globl name##_interposition;	 				\
.align 16;					 		\
ENTRY(name##_interposition)					\
	SAVE_ALL; 						\
	movl %esp, %eax; 					\
	movl ORIG_EAX(%eax), %edx;				\
	call main_##name##_interposition;			\
	test %eax, %eax;					\
	jnz 1f;							\
	RESTORE_REGS;						\
	addl $4, %esp;						\
	iret;							\
1:	/* linux path */					\
	RESTORE_REGS;						\
	jmp *(cos_default_##name##_handler)	

/*
	RESTORE_INT_REGS;					\
	addl $8, %esp;						\
	popl %fs;						\
	popl %gs;						\
	jmp *(cos_default_##name##_handler)	
*/
	
COS_CREATE_TRAP_NO_ERRCODE(div_fault)
COS_CREATE_TRAP_ERRCODE(page_fault)
COS_CREATE_TRAP_NO_ERRCODE(reg_save)

/* The page fault trampoline for kernel faults.  See explanation in relocate_page_fault_handler:hw_ints.c */
.data
.align 4096
.globl cos_page_fault_page_tramp
cos_page_fault_page_tramp:
.globl cos_default_page_fault_handler_tramp
cos_default_page_fault_handler_tramp:
.long 0
.globl cos_interpose_page_fault_handler_tramp
cos_interpose_page_fault_handler_tramp:
.long 0
.globl page_fault_interposition_tramp
.align 16
page_fault_interposition_tramp:
	testl $4,(%esp)  /* *sp = error code, 0x4 = user (=1) or kern (=0) */
	jz 1f
	jmp *(cos_interpose_page_fault_handler_tramp)
.globl cos_post_interpose_deref_addr_tramp
cos_post_interpose_deref_addr_tramp: /* for the previous address */
1:	
	jmp *(cos_default_page_fault_handler_tramp)
.globl cos_post_default_deref_addr_tramp
cos_post_default_deref_addr_tramp:   /* for the previous address */
	.align 4096
#endif /* X86_64 */
