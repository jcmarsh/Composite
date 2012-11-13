#include <linux/sched.h>
#include <asm/uaccess.h>
#include <asm/cacheflush.h>
#include <asm/pgtable.h>
#include "../../../kernel/include/shared/consts.h"
#include "../../../kernel/include/shared/cos_types.h"

#include "page_tables.h"

/*
 * FIXME: types for these are messed up.  This is due to difficulty in
 * using them both in the composite world and in the Linux world.  We
 * should just use them in the composite world and be done with it.
 */
void *va_to_pa(void *va) 
{
	return (void*)__pa(va);
}

void *pa_to_va(void *pa) 
{
	return (void*)__va(pa);
}

/*
 * FIXME: error checking
 */
void *cos_alloc_page(void)
{
	// Using replaced __get_free_pages(GFP_KERNEL, 0) and memset 0 call. -jcm
	void *page = (void*)get_zeroed_page(GFP_KERNEL);
	//	printk("Allocating a page!!!!!!!!!!!\n");
	if (!page) {
	        printk("ERROR: cos_alloc_page failed!\n");
        }
	
	return page;
}

void cos_free_page(void *page)
{
	free_pages((unsigned long int)page, 0);
}

// TODO: Change me maybe? -jcm
inline unsigned long hpage_index(unsigned long n)
{
        unsigned long idx = n >> PG_LVL2_SHIFT;
        return (idx << PG_LVL2_SHIFT) != n ? idx + 1 : idx;
}

/* returns the page table entry */
unsigned long
__pgtbl_lookup_address(paddr_t pgtbl, unsigned long addr)
{
	pte_t *pte;

	pte = pgtbl_lookup_address(pgtbl, addr);
	if (!pte) return 0;
	return pte->pte;
}

/* returns the page table entry */
void
__pgtbl_or_pgd(paddr_t pgtbl, unsigned long addr, unsigned long val)
{
	pgd_t *pt = ((pgd_t *)pa_to_va((void*)pgtbl)) + pgd_index(addr);

	pt->pgd = pgd_val(*pt) | val;
}

int pgtbl_rem_middledir_range(paddr_t pt, unsigned long vaddr, long size)
{
	unsigned long a;

	for (a = vaddr ; a < vaddr + size ; a += HPAGE_SIZE) {
		BUG_ON(pgtbl_rem_middledir(pt, a));
	}
	return 0;
}

int pgtbl_add_middledir_range(paddr_t pt, unsigned long vaddr, long size)
{
	unsigned long a;

	for (a = vaddr ; a < vaddr + size ; a += HPAGE_SIZE) {
		if (pgtbl_add_middledir(pt, a)) {
			pgtbl_rem_middledir_range(pt, vaddr, a-vaddr);
			return -1;
		}
	}
	return 0;
}

/*
 * Remove a given virtual mapping from a page table.  Return 0 if
 * there is no present mapping, and the physical address mapped if
 * there is an existant mapping.
 */
paddr_t pgtbl_rem_ret(paddr_t pgtbl, vaddr_t va)
{
	pte_t *pte = pgtbl_lookup_address(pgtbl, va);
	paddr_t val;

	if (!pte || !(pte_val(*pte) & _PAGE_PRESENT)) {
		return 0;
	}
	val = (paddr_t)(pte_val(*pte) & PTE_MASK);
	pte->pte = 0;

	return val;
}


unsigned long *pgtbl_module_to_vaddr(unsigned long addr)
{
	return (unsigned long *)pgtbl_vaddr_to_kaddr((paddr_t)va_to_pa(current->mm->pgd), addr);
}

inline pte_t *pgtbl_lookup_address(paddr_t pgtbl, unsigned long addr)
{
	pgd_t *pgd = ((pgd_t *)pa_to_va((void*)pgtbl)) + pgd_index(addr);
	pud_t *pud;
	pmd_t *pmd;
	pte_t *pte;
	pteval_t fix_pte;
	unsigned long mask = (unsigned long)1 << 63;

	printk("XXXXXXXXXXX pgtbl %lx\t addr %lx\n", pgtbl, addr);
	//	printk("XXXXXXXXXXX pgd: %p\t *pgd: %lx\n", pgd, *pgd);
	if (pgd_none(*pgd) || pgd_bad(*pgd)) {
		return NULL;
	}
	pud = pud_offset(pgd, addr);
	//	printk("XXXXXXXXXXX pud: %p\t *pud: %lx\n", pud, *pud);
	if (pud_none(*pud) || pud_bad(*pud)) {
		return NULL;
	}
	pmd = pmd_offset(pud, addr);
	//	printk("XXXXXXXXXXX pmd: %p\t *pmd: %lx\n", pmd, *pmd);
	if (pmd_none(*pmd) || pmd_bad(*pmd)) {
		return NULL;
	}
	if (pmd_large(*pmd)) {
	  printk("XXXXXXXXXXX Big page son!\n");
	  return (pte_t *)pmd;
	}
	pte = pte_offset_kernel(pmd, addr);
	if (pte_none(*pte)) {
	  printk("Well, that explains it. I didn't know it was possible.\n");
	}
	printk("XXXXXXXXXXX pte: %p\t *pte: %lx\n", pte, pte_val(*pte));
	
        fix_pte = pte_val(*pte);
	fix_pte = fix_pte & (~mask);
	pte->pte = fix_pte;

	printk("XXXXXXXXXXX pte: %p\t *pte: %lx\n", pte, pte_val(*pte));
	return pte;
}

/*
 * Find the corresponding pte for the address and return its virtual
 * address.  Mainly a debugging tool to see if we are setting up the
 * correct page mappings, but also used to set characteristics in all
 * pages.
 */
// TODO: This is almost identical to pgtbl_lookup_address, may be able to refactor -jcm
inline pte_t *lookup_address_mm(struct mm_struct *mm, unsigned long addr)
{
	pgd_t *pgd = pgd_offset(mm, addr);
	pud_t *pud;
	pmd_t *pmd;
	pte_t *pte;
	pteval_t fix_pte;
	unsigned long mask = (unsigned long)1 << 63;
	
	printk("WWWWWWWWWWW mm %p\t addr %lx\n", mm, addr);
	//	printk("WWWWWWWWWWW pgd: %p\t *pgd: %lx\n", pgd, *pgd);
	//printk("WWWWWWWWWWW va_*pgd: %lx\n", __va(pgd));
	if (pgd_none(*pgd) || pgd_bad(*pgd)) {
		printk("Failure in lookup_address_mm, pgd is bad.\n");	 
		return NULL;
	}
	pud = pud_offset(pgd, addr);
	//	printk("WWWWWWWWWWW pud: %p\t *pud: %lx\n", pud, *pud);
	if (pud_none(*pud) || pud_bad(*pud)) {
		printk("Failure in lookup_address_mm, pud is bad.\n");
		return NULL;
	}
	pmd = pmd_offset(pud, addr);
	//	printk("WWWWWWWWWWW pmd: %p\t *pmd: %lx\n", pmd, *pmd);
	if (pmd_none(*pmd) || pmd_bad(*pmd)) {
		printk("Failure in lookup_address_mm, pmd is bad.\n");
		return NULL;
	}
	if (pmd_large(*pmd)) {
		printk("WWWWWWW Big page here!.\n");	        
		return (pte_t *)pmd;
	}
	pte = pte_offset_kernel(pmd, addr);
	printk("WWWWWWWWWWW pte: %p\t *pte: %lx\n", pte, pte_val(*pte));

        fix_pte = pte_val(*pte);
	fix_pte = fix_pte & (~mask);
	pte->pte = fix_pte;

	printk("WWWWWWWWWWW pte: %p\t *pte: %lx\n", pte, pte_val(*pte));

        return pte;
}

/* 
 * This won't work to find the translation for the argument region as
 * __va doesn't work on module-mapped memory. 
 */
vaddr_t pgtbl_vaddr_to_kaddr(paddr_t pgtbl, unsigned long addr)
{
	pte_t *pte = pgtbl_lookup_address(pgtbl, addr);
	unsigned long kaddr;

	if (!pte || !(pte_val(*pte) & _PAGE_PRESENT)) {
		return 0;
	}
	
	/*
	 * 1) get the value in the pte
	 * 2) map out the non-address values to get the physical address
	 * 3) convert the physical address to the vaddr
	 * 4) offset into that vaddr the appropriate amount from the addr arg.
	 * 5) return value
	 */

	printk("In page_tables.c:pgtble_vaddr_to_kaddr\n");
	printk("\t*pte: %p\t, pte_val: %lx\n", *pte, pte_val(*pte));
	printk("\t MASKS, PTE: %lx, PAGE: %lx\n", PTE_MASK, PAGE_MASK);

	kaddr = (unsigned long)__va(pte_val(*pte) & PTE_MASK) + (~PAGE_MASK & addr);

	return (vaddr_t)kaddr;
}

////////////////////////////////////////////////////////////////////////
/* FIXME: change to clone_pgd_range */
// TODO: This is incorrect for x86_64 -jcm Should probably be pmd.
inline void copy_pgd_range(struct mm_struct *to_mm, struct mm_struct *from_mm,
				  unsigned long lower_addr, unsigned long size)
{
	pgd_t *tpgd = pgd_offset(to_mm, lower_addr);
	pgd_t *fpgd = pgd_offset(from_mm, lower_addr);
	unsigned long span = hpage_index(size);

	printk("COPY_PGD_RANGE: size: %lx\t span: %lx\n", size, span);
	//#ifdef NIL
	if (!(pgd_val(*fpgd) & _PAGE_PRESENT)) {
		printk("cos: BUG: nothing to copy in mm %p's pgd @ %x.\n", 
		       from_mm, (unsigned long)lower_addr);
	}
	//#endif

	/* sizeof(pgd entry) is intended */
	memcpy(tpgd, fpgd, span*sizeof(pgd_t));
}

void copy_lvl2_range(struct mm_struct *to_mm, struct mm_struct *from_mm,
		     unsigned long lower_addr, unsigned long size)
{
  // This needs to change. It should get the pmd, but without mapping anything in. -jcm
  //  pmd_t *fpmd = lookup_address_mm(from_mm, lower_addr); // Should already exist (if not mapped, can't copy)
  pmd_t *fpmd = pgtbl_fill_to_pmd(pgd_offset(from_mm, lower_addr), lower_addr); // Fix this -jcm
  pmd_t *tpmd = pgtbl_fill_to_pmd(pgd_offset(to_mm, lower_addr), lower_addr); // May not exist yet, so fill as needed.
  unsigned int span = hpage_index(size);

  printk("copy_lvl2_range: size: %lx\t span: %lx\n", size, span);
  //#ifdef NIL
  if (!(pmd_val(*fpmd) & _PAGE_PRESENT)) {
    printk("cos: BUG: nothing to copy in mm %p's pgd @ %x.\n", 
	   from_mm, (unsigned long)lower_addr);
  }
  //#endif
  
  /* sizeof(pgd entry) is intended */
  memcpy(tpmd, fpmd, span*sizeof(pmd_t));
}

void zero_pgtbl_range(paddr_t pt, unsigned long lower_addr, unsigned long size)
{
	pgd_t *pgd = ((pgd_t *)pa_to_va((void*)pt)) + pgd_index(lower_addr);
	unsigned long span = hpage_index(size);

	if (!(pgd_val(*pgd)) & _PAGE_PRESENT) {
		printk("cos: BUG: nothing to copy from pgd @ %x.\n", 
		       (unsigned int)lower_addr);
	}

	/* sizeof(pgd entry) is intended */
	memset(pgd, 0, span*sizeof(pgd_t));
}

// FIX THESE: need to differentiate between copy_pgtbl and cpy_pgd -jcm
void copy_pgtbl_range(paddr_t pt_to, paddr_t pt_from, 
		      unsigned long lower_addr, unsigned long size)
{
	pgd_t *tpgd = ((pgd_t *)pa_to_va((void*)pt_to)) + pgd_index(lower_addr);
	pgd_t *fpgd = ((pgd_t *)pa_to_va((void*)pt_from)) + pgd_index(lower_addr);
	unsigned long span = hpage_index(size);

	if (!(pgd_val(*fpgd)) & _PAGE_PRESENT) {
		printk("cos: BUG: nothing to copy from pgd @ %x.\n", 
		       (unsigned int)lower_addr);
	}

	/* sizeof(pgd entry) is intended */
	memcpy(tpgd, fpgd, span*sizeof(pgd_t));
}

void copy_pgtbl_range_nocheck(paddr_t pt_to, paddr_t pt_from, 
			      unsigned long lower_addr, unsigned long size)
{
	pgd_t *tpgd = ((pgd_t *)pa_to_va((void*)pt_to)) + pgd_index(lower_addr);
	pgd_t *fpgd = ((pgd_t *)pa_to_va((void*)pt_from)) + pgd_index(lower_addr);
	unsigned long span = hpage_index(size);

	printk("Here I go, no check!: pt_to: %lx\t pt_from: %lx\t lower_addr: %lx\t size: %lx\n", pt_to, pt_from, lower_addr, size);
	printk("Span: %lx\n", span);

	/* sizeof(pgd entry) is intended */
	memcpy(tpgd, fpgd, span*sizeof(pgd_t));
}

/* Copy pages non-empty in from, and empty in to */
void copy_pgtbl_range_nonzero(paddr_t pt_to, paddr_t pt_from, 
			      unsigned long lower_addr, unsigned long size)
{
	pgd_t *tpgd = ((pgd_t *)pa_to_va((void*)pt_to)) + pgd_index(lower_addr);
	pgd_t *fpgd = ((pgd_t *)pa_to_va((void*)pt_from)) + pgd_index(lower_addr);
	unsigned long span = hpage_index(size);
	unsigned long i; // TODO: Shouldn't need to be... 
	// TODO: Check back.


	printk("Copying from %p:%d to %p.\n", fpgd, span, tpgd);

	/* sizeof(pgd entry) is intended */
	for (i = 0 ; i < span ; i++) {
		if (!(pgd_val(tpgd[i]) & _PAGE_PRESENT)) {
			if (pgd_val(fpgd[i]) & _PAGE_PRESENT) printk("\tcopying vaddr %lx.\n", lower_addr + i * HPAGE_SHIFT);
			memcpy(&tpgd[i], &fpgd[i], sizeof(pgd_t));
		}
	}
}

void copy_pgtbl(paddr_t pt_to, paddr_t pt_from)
{
  //	copy_pgtbl_range_nocheck(pt_to, pt_from, 0, 0xFFFFFFFF);
  	copy_pgtbl_range_nocheck(pt_to, pt_from, 0, 0x3FFFFFFF);
}

/****************************************************************************/
#ifdef X86_64

void pgtbl_print_path(paddr_t pgtbl, unsigned long addr)
{
  // TODO: fix -jcm
	pgd_t *pt = ((pgd_t *)pa_to_va((void*)pgtbl)) + pgd_index(addr);
	pte_t *pe = pgtbl_lookup_address(pgtbl, addr);
	
	printk("cos: addr %x, pgd entry - %x, pte entry - %x\n", 
	       (unsigned int)addr, (unsigned int)pgd_val(*pt), (unsigned int)pte_val(*pe));

	return;
}

int pgtbl_add_entry(paddr_t pgtbl, unsigned long vaddr, unsigned long paddr)
{
	pte_t *pte = pgtbl_lookup_address(pgtbl, vaddr);
	pgd_t *pgd;
	printk("You AAAHHHHHH? pgtbl: %lx\t vaddr: %lx\t paddr: %lx\n", pgtbl, vaddr, paddr);

	if (pte == NULL) {
	        // Did not find address, add in.
	  	pgd = ((pgd_t *)pa_to_va((void*)pgtbl)) + pgd_index(vaddr);
		pte = pgtbl_fill_to_pte(pgd, vaddr);
	}

	if (!pte || pte_val(*pte) & _PAGE_PRESENT) {
		return -1;
	}
	/*pte_val(*pte)*/
	pte->pte = paddr | (_PAGE_PRESENT | _PAGE_RW | _PAGE_USER | _PAGE_ACCESSED);

	return 0;
}

/*
 * Ensures that the intermediate pages exist (pgd, pud, and pmd).
 * Does not pte
 */
pte_t * pgtbl_fill_to_pte(pgd_t *pgd, unsigned long vaddr) {
        pmd_t *pmd = pgtbl_fill_to_pmd(pgd, vaddr);
	pte_t *pte;
	void *page;

	if (pmd == NULL) {
	        // error out
	        goto err0;
	}
	if (pmd_none(*pmd) || pmd_bad(*pmd)) {
	        page = cos_alloc_page();
		if (!page) {
		        // error out
		        goto err0;
		}
		pmd->pmd = (unsigned long)(__pa(page)) | _PAGE_TABLE;
		if (pmd_none(*pmd) || pmd_bad(*pmd)) {
		        // error out and free page
		        goto err1;
		}
	}
	pte = pte_offset_kernel(pmd, vaddr);

 done:
	return pte;
 err1:
	cos_free_page(page);
 err0:
	pte = NULL;
	goto done;
}

/*
 * Ensures that the intermediate pages exist (pgd, pud).
 * Does not fill in the pmd.
 */
pmd_t * pgtbl_fill_to_pmd(pgd_t *pgd, unsigned long vaddr) {
	pud_t *pud;
	pmd_t *pmd;
	void *page0, *page1;

	if (pgd_none(*pgd) || pgd_bad(*pgd)) {
	  page0 = cos_alloc_page();
	  if (!page0) {
	    // error out
	    goto err0;
	  }
	  pgd->pgd = (unsigned long)(__pa(page0)) | _PAGE_TABLE;
	  if (pgd_none(*pgd) || pgd_bad(*pgd)) {
	    // error out and free page
	    goto err1;
	  }
	}
	pud = pud_offset(pgd, vaddr);
	if (pud_none(*pud) || pud_bad(*pud)) {
	  page1 = cos_alloc_page();
	  pud->pud = (unsigned long)(__pa(page1)) | _PAGE_TABLE;
	  if (pud_none(*pud) || pud_bad(*pud)) {
	    // error out and free both pages
	    goto err2;
	  }
	}
	pmd = pmd_offset(pud, vaddr);

 done:
	return pmd;
 err2:
	cos_free_page(page1);
 err1:
	cos_free_page(page0);
 err0:
	pmd = NULL;
	goto done;
}

/* allocate and link in a page middle directory */
int pgtbl_add_middledir(paddr_t pt, unsigned long vaddr)
{
	pgd_t *pgd = ((pgd_t *)pa_to_va((void*)pt)) + pgd_index(vaddr);
	pmd_t *pmd;
	unsigned long *page;

	page = cos_alloc_page(); /* zeroed */
	if (!page) return -1;

	pmd = pgtbl_fill_to_pmd(pgd, vaddr);
	if (pmd == NULL) {
	        cos_free_page(page); // Added free -jcm
	        return -1;
	}
	
	pmd->pmd = (unsigned long)va_to_pa(page) | _PAGE_PRESENT | _PAGE_RW | _PAGE_USER | _PAGE_ACCESSED;
	return 0;
}

// TODO: add error checking, and consider clearing empty pud and pgd pages -jcm
int pgtbl_rem_middledir(paddr_t pt, unsigned long vaddr)
{
	pgd_t *pgd = ((pgd_t *)pa_to_va((void*)pt)) + pgd_index(vaddr);
	pud_t *pud;
	pmd_t *pmd;
	unsigned long *page;

	pud = pud_offset(pgd, vaddr);
	pmd = pmd_offset(pud, vaddr);

	page = (unsigned long *)pa_to_va((void*)(pgd->pgd & PTE_PFN_MASK));
	pmd->pmd = 0;
	cos_free_page(page);

	return 0;
}

/*
 * Verify that the given address in the page table is present.  Return
 * 0 if present, 1 if not.  *This will check the pgd, not for the pte.*
 * For x86_64 checks the pmd. -jcm
 */
int pgtbl_entry_absent(paddr_t pt, unsigned long addr)
{
	pgd_t *pgd = ((pgd_t *)pa_to_va((void*)pt)) + pgd_index(addr);
	pud_t *pud;
	pmd_t *pmd;

	if (pgd_none(*pgd) || pgd_bad(*pgd)) {
	  return 1;
	}
	pud = pud_offset(pgd, addr);
	if (pud_none(*pud) || pud_bad(*pud)) {
	  return 1;
	}
	pmd = pmd_offset(pud, addr);

	return !((pmd_val(*pmd)) & _PAGE_PRESENT);
}

/* Find the nth valid pgd entry */
// TODO: Fix. Only used in debugging code. -jcm
unsigned long get_valid_pgtbl_entry(paddr_t pt, int n)
{
	int i;

	for (i = 1 ; i < PTRS_PER_PGD ; i++) {
		if (!pgtbl_entry_absent(pt, i*PGDIR_SIZE)) {
			n--;
			if (n == 0) {
				return i*PGDIR_SIZE;
			}
		}
	}
	return 0;
}

// TODO: Fix. Only used in debugging code. -jcm
void print_valid_pgtbl_entries(paddr_t pt) 
{
	int n = 1;
	unsigned long ret;
	printk("cos: valid pgd addresses:\ncos: ");
	while ((ret = get_valid_pgtbl_entry(pt, n++)) != 0) {
		printk("%lx\t", ret);
	}
	printk("\ncos: %d valid addresses.\n", n-1);

	return;
}

#else /* x86_32 implementation */

void pgtbl_print_path(paddr_t pgtbl, unsigned long addr)
{
	pgd_t *pt = ((pgd_t *)pa_to_va((void*)pgtbl)) + pgd_index(addr);
	pte_t *pe = pgtbl_lookup_address(pgtbl, addr);
	
	printk("cos: addr %x, pgd entry - %x, pte entry - %x\n", 
	       (unsigned int)addr, (unsigned int)pgd_val(*pt), (unsigned int)pte_val(*pe));

	return;
}

int pgtbl_add_entry(paddr_t pgtbl, unsigned long vaddr, unsigned long paddr)
{
	pte_t *pte = pgtbl_lookup_address(pgtbl, vaddr);

	if (!pte || pte_val(*pte) & _PAGE_PRESENT) {
		return -1;
	}
	/*pte_val(*pte)*/
	pte->pte = paddr | (_PAGE_PRESENT | _PAGE_RW | _PAGE_USER | _PAGE_ACCESSED);

	return 0;
}

// TODO: I'm going to need to figure something out for this.
// It's not needed in x86_32
pmd_t* pgtbl_fill_to_pmd(pgd_t *pgd, unsigned long vaddr) {
  return NULL:
}

/* allocate and link in a page middle directory */
int pgtbl_add_middledir(paddr_t pt, unsigned long vaddr)
{
	pgd_t *pgd = ((pgd_t *)pa_to_va((void*)pt)) + pgd_index(vaddr);
	unsigned long *page;

	printk("You RAAAANNNGGGG? pt: %lx\t vaddr: %lx\n", pt, vaddr);

	page = cos_alloc_page(); /* zeroed */
	if (!page) return -1;

	pgd->pgd = (unsigned long)va_to_pa(page) | _PAGE_PRESENT | _PAGE_RW | _PAGE_USER | _PAGE_ACCESSED;
	return 0;
}

int pgtbl_rem_middledir(paddr_t pt, unsigned long vaddr)
{
	pgd_t *pgd = ((pgd_t *)pa_to_va((void*)pt)) + pgd_index(vaddr);
	unsigned long *page;

	page = (unsigned long *)pa_to_va((void*)(pgd->pgd & PTE_PFN_MASK));
	pgd->pgd = 0;
	cos_free_page(page);

	return 0;
}

inline pte_t *pgtbl_lookup_address(paddr_t pgtbl, unsigned long addr)
{
	pgd_t *pgd = ((pgd_t *)pa_to_va((void*)pgtbl)) + pgd_index(addr);
	pud_t *pud;
	pmd_t *pmd;

	if (pgd_none(*pgd)) {
		return NULL;
	}
	pud = pud_offset(pgd, addr);
	if (pud_none(*pud)) {
		return NULL;
	}
	pmd = pmd_offset(pud, addr);
	if (pmd_none(*pmd)) {
		return NULL;
	}
	if (pmd_large(*pmd))
		return (pte_t *)pmd;
        return pte_offset_kernel(pmd, addr);
}

/*
 * Verify that the given address in the page table is present.  Return
 * 0 if present, 1 if not.  *This will check the pgd, not for the pte.*
 */
int pgtbl_entry_absent(paddr_t pt, unsigned long addr)
{
	pgd_t *pgd = ((pgd_t *)pa_to_va((void*)pt)) + pgd_index(addr);

	return !((pgd_val(*pgd)) & _PAGE_PRESENT);
}

/* Find the nth valid pgd entry */
unsigned long get_valid_pgtbl_entry(paddr_t pt, int n)
{
	int i;

	for (i = 1 ; i < PTRS_PER_PGD ; i++) {
		if (!pgtbl_entry_absent(pt, i*PGDIR_SIZE)) {
			n--;
			if (n == 0) {
				return i*PGDIR_SIZE;
			}
		}
	}
	return 0;
}

void print_valid_pgtbl_entries(paddr_t pt) 
{
	int n = 1;
	unsigned long ret;
	printk("cos: valid pgd addresses:\ncos: ");
	while ((ret = get_valid_pgtbl_entry(pt, n++)) != 0) {
		printk("%lx\t", ret);
	}
	printk("\ncos: %d valid addresses.\n", n-1);

	return;
}
#endif /* X86_64 */
