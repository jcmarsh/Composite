#include <cos_component.h>
#include <initf.h>
#include <sched.h>

__attribute__((weak)) int _binary_init_tar_start = 0;
__attribute__((weak)) int _binary_init_tar_size  = 0;

struct init_fs_info {
	long size;
	char *start;
} info;

int __initf_read(int offset, struct cos_array *da)
{
	int max = da->sz, amnt, left;
	
	if (offset > info.size) return 0;
	left = info.size - offset;
	amnt = (left > max) ? max : left;

	memcpy(da->mem, info.start+offset, amnt);
	return amnt;
}

int initf_size(void)
{
	return info.size;
}

void cos_init(void)
{
	info.start = (char*)&_binary_init_tar_start;
	info.size  = (unsigned long)  &_binary_init_tar_size;
}
