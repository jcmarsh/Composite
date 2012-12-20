#include <cos_component.h>
//#include <boot_deps.h>
#include <sched_hier.h>

int nothing = 0, ret = 0;

extern int main(void);

int spd0_main(void)
{
        main();

	//	cos_null();

	cos_print("In comp0:spd0_main\n", 20);

	cos_print("BLALABLABLALAAAAAA\n", 20);
	cos_print("BLALABLABLALAAAAAA\n", 20);

	
	nothing = sched_init();
	
	
	// nothing = ret + 1;
	//prevent_tail_call(ret);
	return nothing;
}
