/**
 * Based off of entry_64.S and calling.h in the linux source,
 * and trys to emulate composite's kern_entry.S
 */


//#include <asm/cache.h>
//#include <asm/errno.h>
//#include <asm/dwarf2.h>
//#include <asm/msr.h>
//#include <asm/unistd.h>
//#include <asm/hw_irq.h>
//#include <asm/page_types.h>
//#include <asm/irqflags.h>
//#include <asm/paravirt.h>
//#include <asm/ftrace.h>


#include <asm/asm-offsets.h> // Need?
#include <asm/segment.h> // Need?
#include <asm/thread_info.h> // Need?
#include <asm/percpu.h>
#include <linux/linkage.h> // Need?
#include "../../asym_exec_dom.h"  // Need? - is called "deadweight" in it's comments. Only actually used in hijack.c
#include "../../../../../kernel/include/asm_ipc_defs.h"

#include "calling64.h"


.data
.align 16

.text
ALIGN # What does THIS do?
ENTRY(sysenter_interposition_entry)
	// Let's get stacked!
	swapgs
	movq 	%rsp, PER_CPU_VAR(composite_old_rsp)
	movq	PER_CPU_VAR(kernel_stack), %rsp

	SAVE_ALL
	movq	$0xdeadbeef, %rax
	pushq	%rax
	popq	%rbx
	cmpq	%rax, %rbx
	jne	test_return
	call	test_function

test_return:
	RESTORE_ALL
	movq	PER_CPU_VAR(composite_old_rsp), %rsp
	swapgs

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

	cmpq $((1<<COS_CAPABILITY_OFFSET)-1), %rax /* 2^20-1 shift for capability invocations */
	//ja composite_call_ipc /* component invocation */
	//ja composite_ret_ipc /* component return */

	cmpq $((1<<COS_SYSCALL_OFFSET)-1), %rax /* composite syscall */
	//ja cos_syscall_thunk

	/* hijack_syscall_monitor? Don't worry for now */

normal_syscall:
	/* I'm not sure that I need to mess with the TSS */
	/* Will investigate later */
	jmp	*(cos_default_sysenter_addr)

cos_syscall_thunk:
	/* cos syscalls may call linux functions... not supported yet */
	//jmp composite_make_syscall
