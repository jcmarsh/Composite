/**
 * Copyright 2011 by Gabriel Parmer, gparmer@gwu.edu
 *
 * Redistribution of this file is permitted under the GNU General
 * Public License v2.
 */

#include <cos_component.h>

extern struct cos_component_information cos_comp_info;
struct cobj_header *hs[MAX_NUM_SPDS+1];

/* dependencies */
#include <boot_deps.h>

#include <cobj_format.h>

/* interfaces */
//#include <failure_notif.h>
#include <cgraph.h>

/* local meta-data to track the components */
struct spd_local_md {
	spdid_t spdid;
	vaddr_t comp_info;
	char *page_start, *page_end;
	struct cobj_header *h;
} local_md[MAX_NUM_SPDS+1];

/* Component initialization info. */
#define INIT_STR_SZ 52
/* struct is 64 bytes, so we can have 64 entries in a page. */
struct component_init_str {
	unsigned int spdid, schedid;
	int startup;
	char init_str[INIT_STR_SZ];
}__attribute__((packed));
struct component_init_str *init_args;

static int 
boot_spd_set_symbs(struct cobj_header *h, spdid_t spdid, struct cos_component_information *ci)
{
	int i;

	if (cos_spd_cntl(COS_SPD_UCAP_TBL, spdid, ci->cos_user_caps, 0)) BUG();
	if (cos_spd_cntl(COS_SPD_UPCALL_ADDR, spdid, ci->cos_upcall_entry, 0)) BUG();
	for (i = 0 ; i < COS_NUM_ATOMIC_SECTIONS/2 ; i++) {
		if (cos_spd_cntl(COS_SPD_ATOMIC_SECT, spdid, ci->cos_ras[i].start, i*2)) BUG();
		if (cos_spd_cntl(COS_SPD_ATOMIC_SECT, spdid, ci->cos_ras[i].end,   (i*2)+1)) BUG();
	}

	return 0;
}

static int 
boot_spd_symbs(struct cobj_header *h, spdid_t spdid, vaddr_t *comp_info)
{
	unsigned int i;

	for (i = 0 ; i < h->nsymb ; i++) {
		struct cobj_symb *symb;

		symb = cobj_symb_get(h, i);
		assert(symb);
		if (COBJ_SYMB_UNDEF == symb->type) break;

		switch (symb->type) {
		case COBJ_SYMB_COMP_INFO:
			*comp_info = symb->vaddr;
			break;
		default:
			printc("boot: Unknown symbol type %d\n", symb->type);
			break;
		}
	}
	return 0;
}

static void 
boot_symb_reify(char *mem, vaddr_t d_addr, vaddr_t symb_addr, u32_t value)
{
	if (round_to_page(symb_addr) == d_addr) {
		u32_t *p;
		
		p = (u32_t*)(mem + ((PAGE_SIZE-1) & symb_addr));
		*p = value;
	}
}

static void 
boot_symb_reify_16(char *mem, vaddr_t d_addr, vaddr_t symb_addr, u16_t value)
{
	if (round_to_page(symb_addr) == d_addr) {
		u16_t *p;
		
		p = (u16_t*)(mem + ((PAGE_SIZE-1) & symb_addr));
		*p = value;
	}
}

static void 
boot_symb_process(struct cobj_header *h, spdid_t spdid, vaddr_t heap_val, 
		  char *mem, vaddr_t d_addr, vaddr_t symb_addr)
{
	int i;
	struct cos_component_information *ci;

	if (round_to_page(symb_addr) != d_addr) return;
		
	ci = (struct cos_component_information*)(mem + ((PAGE_SIZE-1) & symb_addr));
//		ci->cos_heap_alloc_extent = ci->cos_heap_ptr;
//		ci->cos_heap_allocated = heap_val;
	if (!ci->cos_heap_ptr) ci->cos_heap_ptr = heap_val;
	ci->cos_this_spd_id = spdid;
	ci->init_string[0]  = '\0';
	for (i = 0 ; init_args[i].spdid ; i++) {
		char *start, *end;
		int len;
		
		if (init_args[i].spdid != spdid) continue;
		
		start = strchr(init_args[i].init_str, '\'');
		if (!start) break;
		start++;
		end   = strchr(start, '\'');
		if (!end) break;
		len   = (int)(end-start);
		memcpy(&ci->init_string[0], start, len);
		ci->init_string[len] = '\0';
	}
	
	/* save the address of this page for later retrieval
	 * (e.g. to manipulate the stack pointer) */
	comp_info_record(h, spdid, ci);
}

static vaddr_t boot_spd_end(struct cobj_header *h)
{
	struct cobj_sect *sect;
	int max_sect;

	max_sect = h->nsect-1;
	sect     = cobj_sect_get(h, max_sect);
	
	return sect->vaddr + round_up_to_page(sect->bytes);
}

static int boot_spd_map_memory(struct cobj_header *h, spdid_t spdid, vaddr_t comp_info)
{
	unsigned int i;
	vaddr_t dest_daddr;

	local_md[spdid].spdid      = spdid;
	local_md[spdid].h          = h;
	local_md[spdid].page_start = cos_get_heap_ptr();
	local_md[spdid].comp_info  = comp_info;
	for (i = 0 ; i < h->nsect ; i++) {
		struct cobj_sect *sect;
		char *dsrc;
		int left;

		sect       = cobj_sect_get(h, i);
		dest_daddr = sect->vaddr;
		left       = cobj_sect_size(h, i);

		while (left > 0) {
			dsrc = cos_get_vas_page();
			if ((vaddr_t)dsrc != __mman_get_page(cos_spd_id(), (vaddr_t)dsrc, 0)) BUG();
			if (dest_daddr != (__mman_alias_page(cos_spd_id(), (vaddr_t)dsrc, spdid, dest_daddr))) BUG();

			dest_daddr += PAGE_SIZE;
			left       -= PAGE_SIZE;
		}
	}
	local_md[spdid].page_end = (void*)dest_daddr;

	return 0;
}

static int boot_spd_map_populate(struct cobj_header *h, spdid_t spdid, vaddr_t comp_info)
{
	unsigned int i;
	char *start_page;
	
	start_page = local_md[spdid].page_start;
	for (i = 0 ; i < h->nsect ; i++) {
		struct cobj_sect *sect;
		vaddr_t dest_daddr;
		char *lsrc, *dsrc;
		int left, page_left;

		sect       = cobj_sect_get(h, i);
		dest_daddr = sect->vaddr;
		lsrc       = cobj_sect_contents(h, i);
		left       = cobj_sect_size(h, i);

		while (left) {
			/* data left on a page to copy over */
			page_left   = (left > PAGE_SIZE) ? PAGE_SIZE : left;
			dsrc        = start_page;
			start_page += PAGE_SIZE;

			if (sect->flags & COBJ_SECT_ZEROS) {
				memset(dsrc, 0, PAGE_SIZE);
			} else {
				memcpy(dsrc, lsrc, page_left);
				if (page_left < PAGE_SIZE) memset(dsrc+page_left, 0, PAGE_SIZE - page_left);
			}

			/* Check if special symbols that need
			 * modification are in this page */
			boot_symb_process(h, spdid, boot_spd_end(h), dsrc, dest_daddr, comp_info);
			
			lsrc       += PAGE_SIZE;
			dest_daddr += PAGE_SIZE;
			left       -= page_left;
		}
	}
	return 0;
}

static int boot_spd_map(struct cobj_header *h, spdid_t spdid, vaddr_t comp_info)
{
	if (boot_spd_map_memory(h, spdid, comp_info)) return -1; 
	if (boot_spd_map_populate(h, spdid, comp_info)) return -1;

	return 0;
}

static int boot_spd_reserve_caps(struct cobj_header *h, spdid_t spdid)
{
	if (cos_spd_cntl(COS_SPD_RESERVE_CAPS, spdid, h->ncap, 0)) BUG();
	return 0;
}

/* /\* Deactivate or activate all capabilities to an spd (i.e. call faults */
/*  * on invocation, or use normal stubs) *\/ */
/* static int boot_spd_caps_chg_activation(spdid_t spdid, int activate) */
/* { */
/* 	int i; */

/* 	/\* Find all capabilities to spdid *\/ */
/* 	for (i = 0 ; hs[i] != NULL ; i++) { */
/* 		unsigned int j; */

/* 		if (hs[i]->id == spdid) continue; */
/* 		for (j = 0 ; j < hs[i]->ncap ; j++) { */
/* 			struct cobj_cap *cap; */

/* 			cap = cobj_cap_get(hs[i], j); */
/* 			if (cobj_cap_undef(cap)) break; */

/* 			if (cap->dest_id != spdid) continue; */

/* 			cos_cap_cntl(COS_CAP_SET_SSTUB, hs[i]->id, cap->cap_off, activate ? cap->sstub : 1); */
/* 		} */
/* 	} */
/* 	return 0; */
/* } */

static void
boot_edge_create(spdid_t src, spdid_t dest)
{
	int i , c, s;

	for (i = 0 ; (c = cgraph_client(i)) >= 0 ; i++) {
		s = cgraph_server(i);
		if (c == src && s == dest) return;
	}
	cgraph_add(dest, src);
}

/* 
 * This must happen _after_ the memory is mapped in so that the
 * ucap_tbl can be found in the spd
 */
static int boot_spd_caps(struct cobj_header *h, spdid_t spdid)
{
	struct cobj_cap *cap;
	unsigned int i;

	for (i = 0 ; i < h->ncap ; i++) {
		cap = cobj_cap_get(h, i);

		if (cobj_cap_undef(cap)) break;

		/* we have a fault handler... */
		if (cap->fault_num < COS_FLT_MAX) {
			if (cos_cap_cntl(COS_CAP_SET_FAULT, spdid, cap->cap_off, cap->fault_num)) BUG();
		}
		
		if (cos_cap_cntl(COS_CAP_SET_SERV_FN, spdid, cap->cap_off, cap->sfn) ||
		    cos_cap_cntl(COS_CAP_SET_CSTUB, spdid, cap->cap_off, cap->cstub) ||
		    cos_cap_cntl(COS_CAP_SET_SSTUB, spdid, cap->cap_off, cap->sstub) ||
		    cos_cap_cntl(COS_CAP_ACTIVATE, spdid, cap->cap_off, cap->dest_id)) BUG();
		
		boot_edge_create(spdid, cap->dest_id);
	}

	return 0;
}

/* The order of creating boot threads */
unsigned int *boot_sched;

static int 
boot_spd_thd(spdid_t spdid)
{
	union sched_param sp = {.c = {.type = SCHEDP_RPRIO, .value = 1}};

	/* Create a thread IF the component requested one */
	if ((sched_create_thread_default(spdid, sp.v, 0, 0)) < 0) return -1;
	return 0;
}

static void 
boot_find_cobjs(struct cobj_header *h, int n)
{
	int i;
	vaddr_t start, end;

	start = (vaddr_t)h;
	hs[0] = h;
	for (i = 1 ; i < n ; i++) {
		int j = 0, size = 0, tot = 0;

		size = h->size;
		for (j = 0 ; j < (int)h->nsect ; j++) {
			tot += cobj_sect_size(h, j);
		}
		printc("cobj %s:%d found at %p:%x, size %x -> %lx\n", 
		       h->name, h->id, hs[i-1], size, tot, cobj_sect_get(hs[i-1], 0)->vaddr);

		end   = start + round_up_to_cacheline(size);
		hs[i] = h = (struct cobj_header*)end;
		start = end;
	}
	hs[n] = NULL;
	printc("cobj %s:%d found at %p -> %lx\n", 
	       hs[n-1]->name, hs[n-1]->id, hs[n-1], cobj_sect_get(hs[n-1], 0)->vaddr);
}

static void 
boot_create_system(void)
{
	unsigned int i, min = ~0;

	for (i = 0 ; hs[i] != NULL ; i++) {
		if (hs[i]->id < min) min = hs[i]->id;
	}
	
	for (i = 0 ; hs[i] != NULL ; i++) {
		struct cobj_header *h;
		spdid_t spdid;
		struct cobj_sect *sect;
		vaddr_t comp_info = 0;
		
		h = hs[i];
		if ((spdid = cos_spd_cntl(COS_SPD_CREATE, 0, 0, 0)) == 0) BUG();
		//printc("spdid %d, h->id %d\n", spdid, h->id);
		assert(spdid == h->id);
		sect = cobj_sect_get(h, 0);
		if (cos_spd_cntl(COS_SPD_LOCATION, spdid, sect->vaddr, SERVICE_SIZE)) BUG();

		if (boot_spd_symbs(h, spdid, &comp_info))        BUG();
		if (boot_spd_map(h, spdid, comp_info))           BUG();
		if (boot_spd_reserve_caps(h, spdid))             BUG();
		if (cos_spd_cntl(COS_SPD_ACTIVATE, spdid, 0, 0)) BUG();
	}
	for (i = 0 ; hs[i] != NULL ; i++) {
		struct cobj_header *h;
		h = hs[i];

		if (boot_spd_caps(h, h->id)) BUG();
	}
	
	if (!boot_sched) return;
	for (i = 0 ; boot_sched[i] != 0 ; i++) {
		struct cobj_header *h;
		int j;

		h = NULL;
		for (j = 0 ; hs[j] != NULL; j++) {
			if (hs[j]->id == boot_sched[i]) h = hs[j];
		}
		assert(h);

		if (h->flags & COBJ_INIT_THD) boot_spd_thd(h->id);
	}
}

void 
failure_notif_wait(spdid_t caller, spdid_t failed)
{
	/* wait for the other thread to finish rebooting the component */
	LOCK();	
	UNLOCK();
}

/* Reboot the failed component! */
void 
failure_notif_fail(spdid_t caller, spdid_t failed)
{
	struct spd_local_md *md;

	LOCK();

//	boot_spd_caps_chg_activation(failed, 0);
	md = &local_md[failed];
	assert(md);
	if (boot_spd_map_populate(md->h, failed, md->comp_info)) BUG();
	/* can fail if component had no boot threads: */
	if (md->h->flags & COBJ_INIT_THD) boot_spd_thd(failed); 	
	if (boot_spd_caps(md->h, failed)) BUG();
//	boot_spd_caps_chg_activation(failed, 1);

	UNLOCK();
}

struct deps { short int client, server; };
struct deps *deps;
int ndeps;

int
cgraph_server(int iter)
{
	if (iter >= ndeps || iter < 0) return -1;
	return deps[iter].server;
}

int
cgraph_client(int iter)
{
	if (iter >= ndeps || iter < 0) return -1;
	return deps[iter].client;
}

#define MAX_DEP (PAGE_SIZE/sizeof(struct deps))
int
cgraph_add(int serv, int client)
{
	if (ndeps == MAX_DEP) return -1;
	deps[ndeps].server = serv;
	deps[ndeps].client = client;
	ndeps++;
	return 0;
}

#define NREGIONS 4

void cos_init(void)
{
	struct cobj_header *h;
	int num_cobj, i;

	LOCK();
	boot_deps_init();
	h         = (struct cobj_header *)cos_comp_info.cos_poly[0];
	num_cobj  = (int)cos_comp_info.cos_poly[1];

	deps      = (struct deps *)cos_comp_info.cos_poly[2];
	for (i = 0 ; deps[i].server ; i++) ;
	ndeps     = i;
	init_args = (struct component_init_str *)cos_comp_info.cos_poly[3];
	init_args++; 

	boot_sched = (unsigned int *)cos_comp_info.cos_poly[4];

	boot_find_cobjs(h, num_cobj);
	/* This component really might need more vas, get the next 4M region */
	// Only requesting 2 MB for x86_64 right now....
	// Start here. -jcm
	printc("NREGIONS: %d\t rutpp: %lx\t Size: %lx\n", NREGIONS, round_up_to_pgd_page(1), (NREGIONS-1) * round_up_to_pgd_page(1)); 
	if (cos_vas_cntl(COS_VAS_SPD_EXPAND, cos_spd_id(), 
			 round_up_to_pgd_page((unsigned long)&num_cobj), 
			 (NREGIONS-1) * round_up_to_pgd_page(1))) {
		printc("Could not expand boot component to %p:%lx\n",
		       (void *)round_up_to_pgd_page((unsigned long)&num_cobj), 
		       (unsigned long)round_up_to_pgd_page(1)*3); // *3? Is that NREGIONS-1? -jcm
		BUG();
	}
	printc("h @ %p, heap ptr @ %p\n", h, cos_get_heap_ptr());
	printc("header %p, size %d, num comps %d, new heap %p\n", 
	       h, h->size, num_cobj, cos_get_heap_ptr());

	/* Assumes that hs have been setup with boot_find_cobjs */
	boot_create_system();
	UNLOCK();
	boot_deps_run();

	return;
}
