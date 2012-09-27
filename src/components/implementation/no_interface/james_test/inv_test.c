#include <cos_component.h>
#include <sched.h>

volatile int blah = 0;

void cos_init(void *arg)
{
	int other = *((int*)0);//, other2 = 10/blah;

	other = 10;
}

void bin(void)
{
	sched_block(cos_spd_id(), 0);
}
