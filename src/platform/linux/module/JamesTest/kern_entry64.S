/**
 * Based off of entry_64.S and calling.h in the linux source,
 * and trys to emulate composite's kern_entry.S
 */

//#include <asm/asm-offsets.h> // Need?
//#include <asm/segment.h> // Need?
//#include <asm/thread_info.h> // Need?
//#include <linux/linkage.h> // Need?
//#include "asym_exec_dom.h"  // Need? - is called "deadweight" in it's comments. Only actually used in hijack.c
//#include "../../../../kernel/include/asm_ipc_defs.h"

#include "calling64.h"

.data
.align 16

.text
ALIGN # What does THIS do?
ENTRY(sysenter_interposition_entry)
