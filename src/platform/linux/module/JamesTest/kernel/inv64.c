/**
 * Redistribution of this file is permitted under the GNU General
 * Public License v2.
 *
 * Copyright 2007 by Boston University.
 * Author: Gabriel Parmer, gabep1@cs.bu.edu, 2007
 *
 * Copyright The George Washington University, Gabriel Parmer,
 * gparmer@gwu.edu, 2009
 */

#include <linux/kernel.h>

#define COS_SYSCALL __attribute__((regparm(0)))

/********** Composite system calls **********/

COS_SYSCALL long
cos_syscall_void(long spdid)
{
  //printk("cos: error - %d made void system call from %d\n", thd_get_id(thd_get_current()), spdid);
  printk("cos: error - void system call.\n");
  return 0;
}

COS_SYSCALL long
cos_syscall_null(long spdid)
{
  printk("NULL SYSCALL! spd id: %lx\n", spdid);
  return 0;
}

COS_SYSCALL long
cos_syscall_print(long spdid, char *str, long len)
{
  printk("%s", str);

  return 0;
}

/*
 * Composite's system call table that is indexed and invoked by ipc.S.
 * The user-level stubs are created in cos_component.h.
 */
void *cos_syscall_tbl[32] = {
	(void*)cos_syscall_void,
	//(void*)cos_syscall_stats,
	(void*)cos_syscall_void,
	(void*)cos_syscall_print,
	//(void*)cos_syscall_create_thread,
	(void*)cos_syscall_void,
	//(void*)cos_syscall_switch_thread,
	(void*)cos_syscall_void,
	//(void*)cos_syscall_brand_wait,
	(void*)cos_syscall_void,
	//(void*)cos_syscall_brand_upcall,
	(void*)cos_syscall_void,
	//(void*)cos_syscall_brand_cntl,
	(void*)cos_syscall_void,
	//(void*)cos_syscall_upcall,
	(void*)cos_syscall_void,
	//(void*)cos_syscall_sched_cntl,
	(void*)cos_syscall_void,
	//(void*)cos_syscall_mpd_cntl,
	(void*)cos_syscall_void,
	//(void*)cos_syscall_mmap_cntl,
	(void*)cos_syscall_void,
	//(void*)cos_syscall_brand_wire,
	(void*)cos_syscall_void,
	//(void*)cos_syscall_cap_cntl,
	(void*)cos_syscall_void,
	//(void*)cos_syscall_buff_mgmt,
	(void*)cos_syscall_void,
	//(void*)cos_syscall_thd_cntl,
	(void*)cos_syscall_void,
	//(void*)cos_syscall_idle,
	(void*)cos_syscall_void,
	//(void*)cos_syscall_spd_cntl,
	(void*)cos_syscall_void,
	//(void*)cos_syscall_vas_cntl,
	(void*)cos_syscall_void,
	//(void*)cos_syscall_trans_cntl,
	(void*)cos_syscall_void,
	//(void*)cos_syscall_pfn_cntl,
	(void*)cos_syscall_void,
	(void*)cos_syscall_void,
	(void*)cos_syscall_void,
	(void*)cos_syscall_void,
	(void*)cos_syscall_void,
	(void*)cos_syscall_void,
	(void*)cos_syscall_void,
	(void*)cos_syscall_void,
	(void*)cos_syscall_void,
	(void*)cos_syscall_void,
	(void*)cos_syscall_void,
	//(void*)cos_syscall_void
	(void*)cos_syscall_null
};
