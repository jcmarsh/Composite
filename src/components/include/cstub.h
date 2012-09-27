#ifndef CSTUB_H
#define CSTUB_H

#ifdef X86_64
// I got rid of the error checking code here; will always return 0 - jcm
#define CSTUB_ASM_PRE(name) \
	__asm__ __volatile__( \
		"pushq %%rbp\n\t" \
	        "pushq %%rcx\n\t" \
	        "pushq %%r11\n\t" \
		"syscall\n\t" \
	        "popq %%r11\n\t" \
	        "popq %%rcx\n\t" \
		"popq %%rbp\n\t" \
		"movq $0, %%rcx\n\t" \
		: "=a" (ret), "=c" (fault)

#define CSTUB_PRE(type, name) \
{					\
        long fault = 0; \
	type ret;	\
                        \
	/* \
	 * cap#    -> rax \
	 * sp      -> rbp \
	 * 1st arg -> rsi \
	 * 2nd arg -> rdx \
	 * 3rd arg -> r10 \
	 * 4th arg -> r8 \
	 */

#define CSTUB_POST \
 \
	return ret; \
}

#define CSTUB_ASM_0(name) \
        	CSTUB_ASM_PRE(name)	   \
                : "a" (uc->cap_no) \
		: "rsi", "rdx", "r10", "r8", "memory", "cc");

#define CSTUB_ASM_1(name, first)		   \
        	CSTUB_ASM_PRE(name)	   \
		: "a" (uc->cap_no), "S" (first)		\
		: "rdx", "r10", "r8", "memory", "cc");

#define CSTUB_ASM_2(name, first, second)   \
        	CSTUB_ASM_PRE(name)	   \
		: "a" (uc->cap_no), "S" (first), "d" (second)	\
		: "r10", "r8", "memory", "cc");

#define CSTUB_ASM_3(name, first, second, third)	\
        	CSTUB_ASM_PRE(name)	   \
		: "a" (uc->cap_no), "S" (first), "d" (second), "r" (third) \
		: "r8", "memory", "cc");

#define CSTUB_ASM_4(name, first, second, third, fourth)	\
        	CSTUB_ASM_PRE(name)	   \
		: "a" (uc->cap_no), "S" (first), "d" (second), "r" (third), "r" (fourth) \
		: "memory", "cc");

#else /* x86_32 implementation */
#define CSTUB_ASM_PRE(name) \
	__asm__ __volatile__( \
		"pushl %%ebp\n\t" \
		"movl %%esp, %%ebp\n\t" \
		"movl $1f, %%ecx\n\t" \
		"sysenter\n\t" \
		".align 8\n\t" \
		"jmp 2f\n\t" \
		".align 8\n\t" \
		"1:\n\t" \
		"popl %%ebp\n\t" \
		"movl $0, %%ecx\n\t" \
		"jmp 3f\n\t" \
		"2:\n\t" \
		"popl %%ebp\n\t" \
		"movl $1, %%ecx\n\t" \
		"3:" \
		: "=a" (ret), "=c" (fault)

#define CSTUB_PRE(type, name)			\
{					\
        long fault = 0; \
	type ret;	\
                        \
	/* \
	 * cap#    -> eax \
	 * sp      -> ebp \
	 * 1st arg -> ebx \
	 * 2nd arg -> esi \
	 * 3rd arg -> edi \
	 * 4th arg -> edx \
	 */

#define CSTUB_POST \
 \
	return ret; \
}

#define CSTUB_ASM_0(name) \
        	CSTUB_ASM_PRE(name)	   \
                : "a" (uc->cap_no) \
		: "ebx", "edx", "esi", "edi", "memory", "cc");

#define CSTUB_ASM_1(name, first)		   \
        	CSTUB_ASM_PRE(name)	   \
		: "a" (uc->cap_no), "b" (first)		\
		: "edx", "esi", "edi", "memory", "cc");

#define CSTUB_ASM_2(name, first, second)   \
        	CSTUB_ASM_PRE(name)	   \
		: "a" (uc->cap_no), "b" (first), "S" (second)	\
		: "edx", "edi", "memory", "cc");

#define CSTUB_ASM_3(name, first, second, third)	\
        	CSTUB_ASM_PRE(name)	   \
		: "a" (uc->cap_no), "b" (first), "S" (second), "D" (third) \
		: "edx", "memory", "cc");

#define CSTUB_ASM_4(name, first, second, third, fourth)	\
        	CSTUB_ASM_PRE(name)	   \
		: "a" (uc->cap_no), "b" (first), "S" (second), "D" (third), "d" (fourth) \
		: "memory", "cc");
#endif /* X86_64 */

#define CSTUB_FN_0(type, name)						\
	__attribute__((regparm(1))) type name##_call(struct usr_inv_cap *uc) \
        CSTUB_PRE(type, name)

#define CSTUB_FN_ARGS_1(type, name, type1, first)					\
	__attribute__((regparm(1))) type name##_call(struct usr_inv_cap *uc, type1 first) \
        CSTUB_PRE(type, name)
#define CSTUB_FN_1(type, name, type1) CSTUB_FN_ARGS_1(type, name, type1, first);

#define CSTUB_FN_ARGS_2(type, name, type1, first, type2, second)			\
	__attribute__((regparm(1))) type name##_call(struct usr_inv_cap *uc, type1 first, type2 second) \
	CSTUB_PRE(type, name)
#define CSTUB_FN_2(type, name, type1, type2) CSTUB_FN_ARGS_2(type, name, type1, first, type2, second)

#define CSTUB_FN_ARGS_3(type, name, type1, first, type2, second, type3, third)	\
	__attribute__((regparm(1))) type name##_call(struct usr_inv_cap *uc, type1 first, type2 second, type3 third) \
        CSTUB_PRE(type, name)
#define CSTUB_FN_3(type, name, type1, type2, type3) CSTUB_FN_ARGS_3(type, name, type1, first, type2, second, type3, third) 

#define CSTUB_FN_ARGS_4(type, name, type1, first, type2, second, type3, third, type4, fourth) \
	__attribute__((regparm(1))) type name##_call(struct usr_inv_cap *uc, type1 first, type2 second, type3 third, type4 fourth) \
        CSTUB_PRE(type, name)
#define CSTUB_FN_4(type, name, type1, type2, type3, type4) \
	CSTUB_FN_ARGS_4(type, name, type1, first, type2, second, type3, third, type4, fourth) 

#define CSTUB_FN_ARGS_5(type, name, type1, first, type2, second, type3, third, type4, fourth, type5, fifth) \
	__attribute__((regparm(1))) type name##_call(struct usr_inv_cap *uc, type1 first, type2 second, type3 third, type4 fourth, type5 fifth) \
        CSTUB_PRE(type, name)
#define CSTUB_FN_5(type, name, type1, type2, type3, type4) \
	CSTUB_FN_ARGS_5(type, name, type1, first, type2, second, type3, third, type4, fourth, type5, fifth) 

#define CSTUB_FN_ARGS_6(type, name, type1, first, type2, second, type3, third, type4, fourth, type5, fifth, type6, sixth) \
	__attribute__((regparm(1))) type name##_call(struct usr_inv_cap *uc, type1 first, type2 second, type3 third, type4 fourth, type5 fifth, type6 sixth) \
        CSTUB_PRE(type, name)
#define CSTUB_FN_6(type, name, type1, type2, type3, type4, type5, type6) \
	CSTUB_FN_ARGS_6(type, name, type1, first, type2, second, type3, third, type4, fourth, type5, fifth, type6, sixth)

#define CSTUB_FN_ARGS_7(type, name, type1, first, type2, second, type3, third, type4, fourth, type5, fifth, type6, sixth, type7, seventh) \
	__attribute__((regparm(1))) type name##_call(struct usr_inv_cap *uc, type1 first, type2 second, type3 third, type4 fourth, type5 fifth, type6 sixth, type7 seventh) \
        CSTUB_PRE(type, name)
#define CSTUB_FN_7(type, name, type1, type2, type3, type4, type5, type6, type7)	\
	CSTUB_FN_ARGS_7(type, name, type1, first, type2, second, type3, third, type4, fourth, type5, fifth, type6, sixth, type7, seventh)

#define CSTUB_0(type, name)						\
	CSTUB_FN_0(type, name)						\
        CSTUB_ASM_0(name)						\
        CSTUB_POST

#define CSTUB_1(type, name, type1)					        \
	CSTUB_FN_1(type, name, type1)						\
        CSTUB_ASM_1(name, first)					\
	CSTUB_POST

#define CSTUB_2(type, name, type1, type2)					\
	CSTUB_FN_2(type, name, type1, type2)					\
	CSTUB_ASM_2(name, first, second)				\
	CSTUB_POST

#define CSTUB_3(type, name, type1, type2, type3)				\
	CSTUB_FN_3(type, name, type1, type2, type3)				\
	CSTUB_ASM_3(name, first, second, third)				\
	CSTUB_POST

#define CSTUB_4(type, name, type1, type2, type3, type4)                       \
	CSTUB_FN_4(type, name, type1, type2, type3, type4)			\
	CSTUB_ASM_4(name, first, second, third, fourth)			\
	CSTUB_POST

#endif	/* CSTUB_H */
