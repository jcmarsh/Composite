#include <cos_component.h>
//#include <boot_deps.h>
#include <sched_hier.h>

int nothing = 0, ret = 0;

extern int main(void);

int spd0_main(void)
{
        main();

	ret = cos_test3_syscall(8, 16, 33);
	//	cos_null();

	cos_print("In comp0:spd0_main\n", 20);

	cos_print("BLALABLABLALAAAAAA\n", 20);
	cos_print("BLALABLABLALAAAAAA\n", 20);

	cos_print("WHUTWHUT!\n", 11);
	cos_print("WHUTWHUT!\n", 11); 
	
	//	nothing = sched_init();
	
	
	// nothing = ret + 1;
	//prevent_tail_call(ret);
	return ret; //nothing;
}
