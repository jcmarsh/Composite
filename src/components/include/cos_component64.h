/**
 * Copyright 2007 by Gabriel Parmer, gabep1@cs.bu.edu
 *
 * Redistribution of this file is permitted under the GNU General
 * Public License v2.
 */

#ifndef COS_COMPONENT_H
#define COS_COMPONENT_H

#include "consts.h"
#include "cos_types.h"
#include "errno.h"

/**
 * FIXME: Please remove this since it is no longer needed
 */

extern long stkmgr_stack_space[ALL_TMP_STACKS_SZ];

//extern struct cos_sched_data_area cos_sched_notifications;
extern struct cos_component_information cos_comp_info;

/*
 * A note on the arguments to and for all system calls and on the
 * planned evolution of the system call layer:
 *
 * System calls started as they exist in any operating system, simply
 * passing some identifier to signify which system call we are
 * invoking (in eax), and passing arguments in the rest of the
 * registers.  Because we are sharing a system call namespace
 * (essentially) with Linux using Hijacking techiques, we pass
 * syscall<<COS_SYSCALL_OFFSET to signify our system calls.
 *
 * The second evolution required that we are able to identify which
 * spd makes an system call which is not self-evident (as it would be
 * in a normal system) when composite spds are taken into account.
 * When invoking capabilities for inter-component communication, this
 * information is explicit in the capability itself.  So we needed
 * this for system calls as well.  Version two is that we pass in edx
 * the spd id whenever we make a system call.  Of course component
 * writers themselves cannot be bothered with this, so we store the
 * spd_id in each component in memory and simply read this address,
 * passing its value on system calls.  This implies that the loader
 * initializes this memory location with the correct spd id when the
 * spd is loaded into memory, which is not altogether pleasant.
 *
 * The third evolution which has yet to occur is that system calls
 * will be pulled into the capability framework.  They will not invoke
 * the general path for ipc as the extra check would slow it down, but
 * will invoke capabilities within a separate region of the namespace,
 * allowing for the early demuxing in kern_entry.S.  This is an
 * important evolution because 1) it solves the problem we already
 * have with identifying which spd is making a system call in a more
 * natural (albeit slower) manner, and 2) it allows services to be
 * migrated into and out of the kernel itself dynamically, given
 * perceived system call overhead and application progress.
 */

#define cos_syscall_asm \
  __asm__ __volatile__("":::"rcx","r11"); \
  __asm__ __volatile__("syscall\n\t":"=a" (ret)
#define cos_syscall_clobber \
  :"memory", "cc");		    \
  return ret;

#define cos_syscall_0(num, rtype, name)              \
static inline rtype cos_##name(void)                 \
{                                                    \
  rtype ret;					     \
  cos_syscall_asm							\
    : "a" (num<<COS_SYSCALL_OFFSET), "d" (cos_comp_info.cos_this_spd_id) \
    cos_syscall_clobber							\
}


/*
#define cos_syscall_2(num, rtype, name, type0, name0, type1, name1)\
static inline rtype cos_##name(type0 name0, type1 name1) \
{                                                    \
	rtype ret;                                   \
cos_syscall_asm                                      \
		: "a" (num<<COS_SYSCALL_OFFSET), "d" (cos_comp_info.cos_this_spd_id), "b" (name0), "S" (name1) \
cos_syscall_clobber                                  \
}
*/

// cos_syscall_2(2,  int, print, char*, str, int, len);
cos_syscall_0(31,  int, null);

#endif
