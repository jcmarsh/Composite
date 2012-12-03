#include <cos_component.h>
#include <print.h>
#include <sched_hier.h>

int blah = 0;
//extern int main(void);

void cos_init(void *arg)
//int spd0_main(void)
{
  int other = 10, i;
  long * pointer;

  pointer = 0;

  if (&pointer == 0) {
    printc("This hopefully threw an error\n");
  }
  while (1) {
    other++;
  }
  printc("***************HELLLO****************\n");
  /// main();
}
