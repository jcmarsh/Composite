#include <cos_component.h>
//#include <boot_deps.h>
#include <sched_hier.h>

int nothing = 0, ret = 0;

extern int main(void);

int spd0_main(void)
{
  main();
	sched_init();
	
	nothing = ret + 1;
	//prevent_tail_call(ret);
	return ret;
}
