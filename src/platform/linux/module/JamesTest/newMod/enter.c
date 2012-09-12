#include <linux/module.h> // for all modules
#include <linux/kernel.h> // needed for KERN_INFO
#include <asm/percpu.h>

/*
 * As far as I can tell, the descriptor structure has become
 * more complicated with x86_64, not less. There are now two types.
 */

MODULE_LICENSE("GPL");

extern void sysenter_interposition_entry(void);

struct cos_desc_struct_gate {
  unsigned short address_low;
  unsigned int dont_care;
  unsigned short address_middle;
  unsigned int address_high;
  unsigned int reserved;
} __attribute__((packed));

struct cos_desc_ptr {
  unsigned short idt_limit; // why short? - should only be 2 bytes
  unsigned long idt_base;  // should be 8 bytes
} __attribute__((packed));

unsigned int cos_default_sysenter_addr_low;
unsigned int cos_default_sysenter_addr_high;
void *cos_default_sysenter_addr;

struct cos_desc_ptr default_idt_desc;
struct cos_desc_struct_gate *default_idt;

DEFINE_PER_CPU(unsigned long, composite_old_rsp);

int print_count0 = 0;
void test_print_reg(unsigned long a) {
  bool print = false;
  if (print_count0 < 10) {
    print = true;
  }
  print_count0++;
  if (print) {
    printk(KERN_INFO "We've got something: %lx\n", a);
  }
}

int count0 = 0;
void test_function0(void) {
  bool print = false;
  if (count0 == 0) {
    print = true;
  }
  count0++;
  if (print) {
    printk(KERN_INFO "BASE: Reached Test Function Zero!\n");
  }
}

int count1 = 0;
void test_function1(void) {
  bool print = false;
  if (count1 == 0) {
    print = true;
  }
  count1++;
  if (print) {
    printk(KERN_INFO "BASE: Reached Test Function One!\n");
  }
}

int count2 = 0;
void test_function2(void) {
  bool print = false;
  if (count2 == 0) {
    print = true;
  }
  count2++;
  if (print) {
    printk(KERN_INFO "BASE: Reached Test Function Two! Call IPC\n");
  }
}

int count3 = 0;
void test_function3(void) {
  bool print = false;
  if (count3 == 0) {
    print = true;
  }
  count3++;
  if (print) {
    printk(KERN_INFO "BASE: Reached Test Function Three! Return IPC\n");
  }
}

void hw_int_init(void) {
  int se_addr, trash, i;
  unsigned long addr;

  __asm__ __volatile__("sidt %0" : "=m" (default_idt_desc.idt_limit));
  default_idt = (struct cos_desc_struct_gate *)default_idt_desc.idt_base;

  printk(KERN_INFO "BASE: idt base: %lx\n", default_idt_desc.idt_base);
  addr = default_idt->address_high;
  addr = addr << 16;
  addr += default_idt->address_middle;
  addr = addr << 16;
  addr += default_idt->address_low;
  printk(KERN_INFO "BASE: idt default: %lx\n", addr);
  printk(KERN_INFO "BASE: RESERVED: %x\n", default_idt->reserved);
}

void hw_int_get_se_addr(void) {
  unsigned int mask = 1;
  rdmsr(MSR_LSTAR, cos_default_sysenter_addr_low, cos_default_sysenter_addr_high);

  // Need to have the default address as a pointer to use in kern_entry
  cos_default_sysenter_addr = (void *) ((unsigned long) cos_default_sysenter_addr_high);
  cos_default_sysenter_addr = (void *) ((unsigned long) cos_default_sysenter_addr << 32);
  cos_default_sysenter_addr += (cos_default_sysenter_addr_low & (~mask));
  printk(KERN_INFO "BASE: Syscall handling code: %x:%x = %p\n",
	 cos_default_sysenter_addr_high, cos_default_sysenter_addr_low,
	 cos_default_sysenter_addr);
}

void hw_int_reset(void) {
  wrmsr(MSR_LSTAR, cos_default_sysenter_addr_low, cos_default_sysenter_addr_high);
}

void hw_int_override_sysenter(void *handler) {
  unsigned int low;
  unsigned int high;
  unsigned int mask = 0;

  mask = ~mask;
  low = (unsigned int) ((unsigned long)handler & mask);
  high = (unsigned int) ((unsigned long)handler >> 32);
  wrmsr(MSR_LSTAR, low, high);
  //  printk(KERN_INFO "BASE: Overriding sysenter handler (%x:%x) with %p\n",
  // cos_default_sysenter_addr_low, cos_default_sysenter_addr_high, handler);
}

int init_module(void)
{
  unsigned int low = 0;
  unsigned int high = 0;
  printk(KERN_INFO "BASE: Installing module.\n");

  printk(KERN_INFO "BASE: interposition %p\n", &sysenter_interposition_entry);

  hw_int_get_se_addr(); // init cos_default_sysenter_addr
  hw_int_override_sysenter(&sysenter_interposition_entry); // write a value
  //  rdmsr(MSR_LSTAR, low, high);
  //  hw_int_reset(); // reset

  //printk(KERN_INFO "BASE: wrote %x:%x, should match %p\n", high, low, &sysenter_interposition_entry);

  //hw_int_init();
  //hw_int_override_sysenter(sysenter_interposition_entry);

  return 0;
}

void cleanup_module(void)
{
  printk(KERN_INFO "BASE: Cleaning up module.\n");
  hw_int_reset();
}
