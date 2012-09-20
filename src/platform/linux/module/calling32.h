
/* from entry.S */
EBX		= 0x00
ECX		= 0x04
EDX		= 0x08
ESI		= 0x0C
EDI		= 0x10
EBP		= 0x14
EAX		= 0x18
DS		= 0x1C
ES		= 0x20
FS              = 0x24
GS              = 0x28
ORIG_EAX	= 0x2C // Of all of these, only ORIG_EAX seems to be used -jcm
EIP		= 0x30
CS		= 0x34
EFLAGS		= 0x38
OLDESP		= 0x3C
OLDSS		= 0x40

CF_MASK		= 0x00000001 // None of thes are used - jcm
TF_MASK		= 0x00000100
IF_MASK		= 0x00000200
DF_MASK		= 0x00000400
NT_MASK		= 0x00004000
VM_MASK		= 0x00020000

#define SAVE_ALL    \
	cld;        \
	pushl $0 ;  \
	pushl %fs;  \
	pushl %es;  \
	pushl %ds;  \
	pushl %eax; \
	pushl %ebp; \
	pushl %edi; \
	pushl %esi; \
	pushl %edx; \
	pushl %ecx; \
	pushl %ebx; \
	movl $(__USER_DS), %edx; \
	movl %edx, %ds; \
	movl %edx, %es; \
	movl $(__KERNEL_PERCPU), %edx; \
	movl %edx, %fs

#define RESTORE_INT_REGS \
	popl %ebx;	\
	popl %ecx;	\
	popl %edx;	\
	popl %esi;	\
	popl %edi;	\
	popl %ebp;	\
	popl %eax

#define RESTORE_REGS	\
	RESTORE_INT_REGS; \
	popl %ds;	\
	popl %es;	\
	popl %fs;       \
	addl $4, %esp

#define RESTORE_ALL \
  RESTORE_REGS

#define COS_ENTRY \
	movl TSS_sysenter_sp0(%esp),%esp
