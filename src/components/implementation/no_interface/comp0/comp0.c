#include <cos_component.h>
//#include <boot_deps.h>
#include <sched_hier.h>

int nothing = 0, ret = 0;

extern int main(void);

int spd0_main(void)
{
        main();

	cos_print("In comp0:spd0_main\n", 20);

	nothing = sched_init();
	
	//prevent_tail_call(ret);
	return nothing;
}
