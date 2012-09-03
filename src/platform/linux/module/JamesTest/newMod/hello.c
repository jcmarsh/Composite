#include <linux/module.h> // for all modules
#include <linux/kernel.h> // needed for KERN_INFO

int init_module(void)
{
  printk(KERN_INFO "Hello World.\n");
  return 0;
}

void cleanup_module(void)
{
  printk(KERN_INFO "Goodbye, cruel world.\n");
}
