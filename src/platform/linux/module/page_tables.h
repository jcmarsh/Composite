/* not necessarily compiled in */
#ifndef HPAGE_SHIFT
#ifdef X86_64
#define HPAGE_SHIFT	21
#else
#define HPAGE_SHIFT	22
#endif /* X86_64 */
#define HPAGE_SIZE	((1UL) << HPAGE_SHIFT)
#define HPAGE_MASK	(~(HPAGE_SIZE - 1))
#endif /* HPAGE_SHIFT */
#define PTE_MASK PAGE_MASK

void *va_to_pa(void *va);
void *pa_to_va(void *pa);
void *cos_alloc_page(void);
void cos_free_page(void *page);
inline unsigned int hpage_index(unsigned long n);
unsigned long __pgtbl_lookup_address(paddr_t pgtbl, unsigned long addr);
void __pgtbl_or_pgd(paddr_t pgtbl, unsigned long addr, unsigned long val);
int pgtbl_rem_middledir_range(paddr_t pt, unsigned long vaddr, long size);
int pgtbl_add_middledir_range(paddr_t pt, unsigned long vaddr, long size);
paddr_t pgtbl_rem_ret(paddr_t pgtbl, vaddr_t va);
unsigned long *pgtbl_module_to_vaddr(unsigned long addr);
inline pte_t *pgtbl_lookup_address(paddr_t pgtbl, unsigned long addr);
inline pte_t *lookup_address_mm(struct mm_struct *mm, unsigned long addr);
vaddr_t pgtbl_vaddr_to_kaddr(paddr_t pgtbl, unsigned long addr);

// Arch. Dependent
void pgtbl_print_path(paddr_t pgtbl, unsigned long addr);
int pgtbl_add_entry(paddr_t pgtbl, unsigned long vaddr, unsigned long paddr);
pte_t* pgtbl_fill_to_pte(pgd_t *pgd, unsigned long vaddr);
pmd_t* pgtbl_fill_to_pmd(pgd_t *pgd, unsigned long vaddr);
int pgtbl_add_middledir(paddr_t pt, unsigned long vaddr);
int pgtbl_rem_middledir(paddr_t pt, unsigned long vaddr);
int pgtbl_entry_absent(paddr_t pt, unsigned long addr); // HERE
unsigned long get_valid_pgtbl_entry(paddr_t pt, int n);
void print_valid_pgtbl_entries(paddr_t pt);
inline void copy_pgd_range(struct mm_struct *to_mm, struct mm_struct *from_mm, unsigned long lower_addr, unsigned long size);
void zero_pgtbl_range(paddr_t pt, unsigned long lower_addr, unsigned long size);
void copy_pgtbl_range(paddr_t pt_to, paddr_t pt_from, unsigned long lower_addr, unsigned long size);
void copy_pgtbl_range_nocheck(paddr_t pt_to, paddr_t pt_from, unsigned long lower_addr, unsigned long size);
void copy_pgtbl_range_nonzero(paddr_t pt_to, paddr_t pt_from, unsigned long lower_addr, unsigned long size);
void copy_pgtbl(paddr_t pt_to, paddr_t pt_from);
