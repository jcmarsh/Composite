/**
 * Copyright 2007 by Gabriel Parmer, gabep1@cs.bu.edu
 *
 * Redistribution of this file is permitted under the GNU General
 * Public License v2.
 */

/* 
 * This file is included by both the kernel and by components.  Thus
 * any defines might need to be, unfortunately, made using ifdefs
 */

#ifndef CONSTS_H
#define CONSTS_H

#ifndef __ASM__
#ifdef __KERNEL__
#include <linux/thread_info.h> /* for PAGE_SIZE */
#else 
#ifdef X86_64
struct pt_regs { /* ptrace.h in linux source */
  unsigned long r15;
  unsigned long r14;
  unsigned long r13;
  unsigned long r12;
  unsigned long bp;
  unsigned long bx;
  /* arguments: non interrupts/non tracing syscalls only save upto here */
  unsigned long r11;
  unsigned long r10;
  unsigned long r9;
  unsigned long r8;
  unsigned long ax;
  unsigned long cx;
  unsigned long dx;
  unsigned long si;
  unsigned long di;
  unsigned long orig_ax;
  /* end of arguments */
  /* cpu exception frame or undefined */
  unsigned long ip;
  unsigned long cs;
  unsigned long eflags;
  unsigned long sp;
  unsigned long ss;
  /* top of stack page */
};

#else
struct pt_regs {
        long bx;
        long cx;
        long dx;
        long si;
        long di;
        long bp;
        long ax;
        long ds;
        long es;
        long fs;
        long gs;
        long orig_ax;
        long ip;
        long cs;
        long flags;
        long sp;
        long ss;
};
#endif /* X86_64 */

//struct pt_regs { int dummy[16]; };
#endif
#endif
#define PAGE_ORDER 12 // Should be fine, same in linux kernel -jcm
#ifndef __KERNEL__
#define PAGE_SIZE (1<<PAGE_ORDER)
#endif

#define MAX_SERVICE_DEPTH 31
#define MAX_NUM_THREADS 40
/* Stacks are 2 * page_size (expressed in words) */
#define MAX_STACK_SZ    (PAGE_SIZE/4) /* a page */
#define ALL_STACK_SZ    (MAX_NUM_THREADS*MAX_STACK_SZ)
#define MAX_SPD_VAS_LOCATIONS 8

/* a kludge:  should not use a tmp stack on a stack miss */
#define TMP_STACK_SZ       (128/4) 
#define ALL_TMP_STACKS_SZ  (MAX_NUM_THREADS*TMP_STACK_SZ)

#define MAX_SCHED_HIER_DEPTH 4

#define MAX_NUM_SPDS   64
#define MAX_STATIC_CAP 1024

#ifdef X86_64
// I'm going to change the nameing scheme to break things. Levels 4 - 1, 4 being the highest (old PGDIR)
// Okay, that did not go over well. -jcm
//#define SHIFT_4      39
//#define SIZE_4       (unsigned long)(1 << SHIFT_4)
//#define MASK_4       (unsigned long)(~(SIZE_4 - 1))
//#define SHIFT_3      30
//#define SIZE_3       (unsigned long)(1 << SHIFT_3)
//#define MASK_3       (unsigned long)(~(SIZE_3 - 1))
//#define SHIFT_2      21
//#define SIZE_2       (unsigned long)(1 << SHIFT_2)
//#define MASK_2       (unsigned long)(~(SIZE_2 - 1))

#define PAGE_MASK    (~(PAGE_SIZE-1))
#define PG_LVL2_SHIFT    21
#define PG_LVL2_RANGE    (unsigned long)((unsigned long)1<<PG_LVL2_SHIFT) // Added cast jcm
#define PG_LVL2_SIZE     PG_LVL2_RANGE
#define PGD_LVL2_MASK     (~(PG_LVL2_RANGE-1))

#define PGD_PER_PTBL 512
#define WORD_SIZE    64

#define round_to_pgd_page(x)    round_to_pow2(x, PG_LVL2_SIZE)
#define round_up_to_pgd_page(x) round_up_to_pow2(x, PG_LVL2_SIZE)

#else /* x86_32 implementation */
#define PAGE_MASK    (~(PAGE_SIZE-1))
#define PGD_SHIFT    22
#define PGD_RANGE    (unsigned long)((unsigned long)1<<PGD_SHIFT) // Added cast jcm
#define PGD_SIZE     PGD_RANGE
#define PGD_MASK     (~(PGD_RANGE-1))
#define PGD_PER_PTBL 1024
#define WORD_SIZE 32

#define round_to_pgd_page(x)    round_to_pow2(x, PGD_SIZE)
#define round_up_to_pgd_page(x) round_up_to_pow2(x, PGD_SIZE)
#endif /* X86_64 */

#define MEGS_4 1 << 22

#define round_to_pow2(x, pow2)    (((unsigned long)(x))&(~(pow2-1)))
#define round_up_to_pow2(x, pow2) (round_to_pow2(((unsigned long)x)+pow2-1, pow2))

#define round_to_page(x)        round_to_pow2(x, PAGE_SIZE)
#define round_up_to_page(x)     round_up_to_pow2(x, PAGE_SIZE)


//#define CACHE_LINE (32)
#define CACHE_LINE (64) // Huh? -jcm
#define CACHE_ALIGNED __attribute__ ((aligned (CACHE_LINE)))
#define HALF_CACHE_ALIGNED __attribute__ ((aligned (CACHE_LINE/2)))
#define PAGE_ALIGNED __attribute__ ((aligned(PAGE_SIZE)))

#define round_to_cacheline(x)    round_to_pow2(x, CACHE_LINE)
#define round_up_to_cacheline(x) round_up_to_pow2(x, CACHE_LINE)

#define SHARED_REGION_START (unsigned long)(1<<30)  // 1 gig // Added cast jcm
#define SHARED_REGION_SIZE (MEGS_4) // used to be pgd_range... see comment for SERVICE_SIZE -jcm
#define SERVICE_START ((SHARED_REGION_START)+(SHARED_REGION_SIZE))
#define SERVICE_END   (SHARED_REGION_START+(unsigned long)(1<<30))
/* size of virtual address spanned by one pgd entry */
#define SERVICE_SIZE (MEGS_4) // Used to pgd_range, since 4MB used to correspond to 1 big page but this is no longer true -jcm
#define COS_INFO_REGION_ADDR SHARED_REGION_START
#define COS_DATA_REGION_LOWER_ADDR (COS_INFO_REGION_ADDR+PAGE_SIZE)
#define COS_DATA_REGION_MAX_SIZE (MAX_NUM_THREADS*PAGE_SIZE)

#define COS_NUM_ATOMIC_SECTIONS 10

#define COS_MAX_MEMORY 1024 // -jcm 2048

#include "../asm_ipc_defs.h"

#define KERN_BASE_ADDR 0xc0000000 //CONFIG_PAGE_OFFSET, gap: worry about this later when we do the page fault handling path

#endif
