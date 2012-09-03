#include <linux/module.h> // for all modules
#include <linux/kernel.h> // needed for KERN_INFO

/*
 * As far as I can tell, the descriptor structure has become
 * more complicated with x86_64, not less. There are now two types.
 */
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



struct cos_desc_ptr default_idt_desc;
struct cos_desc_struct_gate *default_idt;

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

int init_module(void)
{
  printk(KERN_INFO "BASE: Installing module.\n");

  hw_int_init();
  //hw_int_override_sysenter(sysenter_interposition_entry);

  return 0;
}

void cleanup_module(void)
{
  printk(KERN_INFO "BASE: Cleaning up module.\n");
  //hw_int_reset();
}
