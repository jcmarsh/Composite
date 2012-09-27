#include <cos_component.h>
#include <pong_lower.h>

#include <print.h>

__attribute__((regparm(1))) void call_call_lower(struct usr_inv_cap *uc)
{
	int ret, fault = 0;

#ifdef X86_64
	/*
	 * cap#    -> rax
	 * sp      -> rsp
	 * 1st arg -> rsi
	 * 2nd arg -> rdx
	 * 3rd arg -> rcx
	 * 4th arg -> rdi
	 * I copied this over from cstub.h. Should probably fix it there first.
	 */
	__asm__ __volatile__(
		"pushq %%rbp\n\t"
	        "pushq %%rcx\n\t"
	        "pushq %%r11\n\t"
		"syscall\n\t"
	        "popq %%r11\n\t"
	        "popq %%rcx\n\t"
		"popq %%rbp\n\t"
		"movq $0, %%rcx\n\t"
		: "=a" (ret), "=c" (fault)
		: "a" (uc->cap_no)
		: "rsi", "rdx", "r10", "r8", "memory", "cc");
		
 
#else /* x86_32 implementation */

	/* 
	 * cap#    -> eax
	 * sp      -> ebp
	 * 1st arg -> ebx
	 * 2nd arg -> esi
	 * 3rd arg -> edi
	 * 4th arg -> edx
	 */
	__asm__ __volatile__(
		"pushl %%ebp\n\t"
		"movl %%esp, %%ebp\n\t"
		"movl $1f, %%ecx\n\t"
		"sysenter\n\t"
		".align 8\n\t"
		"jmp 2f\n\t"
		".align 8\n\t"
		"1:\n\t"
		"popl %%ebp\n\t"
		"movl $0, %%ecx\n\t"
		"jmp 3f\n\t"
		"2:\n\t"
		"popl %%ebp\n\t"
		"movl $1, %%ecx\n\t"
		"3:"
		: "=a" (ret), "=c" (fault)
		: "a" (uc->cap_no)
		: "ebx", "edx", "esi", "edi", "memory", "cc");
#endif /* X86_64 */

	return;
}
