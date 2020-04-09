/**
 * @file    xen_memory.h
 * @brief
 *
 * @author  Aleksei Cherepanov
 * @date    30.03.2020
 */

#ifndef XEN_MEMORY_
#define XEN_MEMORY_

#include <unistd.h>
#include <xen/xen.h>

#if 1
//phymem reuse
#include <mem/page.h>
extern struct page_allocator *xen_mem_allocator;

#define __xen_mem_end (__xen_mem_allocator->pages_start + \
		(__xen_mem_allocator->page_size * __xen_mem_allocator->pages_n))

extern void *xen_mem_alloc(size_t page_number);
extern void xen_mem_free(void *page, size_t page_number);

extern struct page_allocator *xen_allocator(void);
// end of phymem
#endif

extern unsigned long *phys_to_machine_mapping;
extern start_info_t xen_start_info;
extern shared_info_t xen_shared_info;

#define pfn_to_mfn(_pfn) (phys_to_machine_mapping[(_pfn)])
#define mfn_to_pfn(_mfn) (machine_to_phys_mapping[(_mfn)])


#define L1_PAGETABLE_SHIFT      12
#define L2_PAGETABLE_SHIFT      21
#define L3_PAGETABLE_SHIFT      30

#define L1_PAGETABLE_ENTRIES    512
#define L2_PAGETABLE_ENTRIES    512
#define L3_PAGETABLE_ENTRIES    4

/* Given a virtual address, get an entry offset into a page table. */
#define l1_table_offset(_a) \
  (((_a) >> L1_PAGETABLE_SHIFT) & (L1_PAGETABLE_ENTRIES - 1))
#define l2_table_offset(_a) \
  (((_a) >> L2_PAGETABLE_SHIFT) & (L2_PAGETABLE_ENTRIES - 1))
#define l3_table_offset(_a) \
  (((_a) >> L3_PAGETABLE_SHIFT) & (L3_PAGETABLE_ENTRIES - 1))

#define VIRT_START                 ((unsigned long)0x100000L) //hardcode of ELFNOTE(XEN_ELFNOTE_VIRT_BASE, .long 0x100000)
#define to_phys(x)                 ((unsigned long)(x)-VIRT_START)
#define PFN_DOWN(x)	((x) >> L1_PAGETABLE_SHIFT)
#define virt_to_pfn(_virt)         (PFN_DOWN(to_phys(_virt)))

#define virt_to_mfn(_virt)         (pfn_to_mfn(virt_to_pfn(_virt)))

#define CONST(x) x ## ULL
#define _PAGE_PRESENT  CONST(0x001)
#define _PAGE_RW       CONST(0x002)
#define _PAGE_USER     CONST(0x004)
#define _PAGE_ACCESSED CONST(0x020)
#define _PAGE_DIRTY    CONST(0x040)
#define _PAGE_PSE      CONST(0x080)
#define L1_PROT (_PAGE_PRESENT|_PAGE_RW|_PAGE_ACCESSED)
#define L1_PROT_RO (_PAGE_PRESENT|_PAGE_ACCESSED)
#define L2_PROT (_PAGE_PRESENT|_PAGE_RW|_PAGE_ACCESSED|_PAGE_DIRTY |_PAGE_USER)
#define L3_PROT (_PAGE_PRESENT)
#define PAGETABLE_LEVELS        3

#define L1_MASK  ((1UL << L2_PAGETABLE_SHIFT) - 1)
#define L1_FRAME                1
#define L2_FRAME                2

#define PAGE_SHIFT      12
//#define PAGE_SIZE() 				(1UL << PAGE_SHIFT) // or 1?? // in mini-os 1UL
#define PAGE_MASK       (~(PAGE_SIZE()-1))
#define PADDR_BITS              44
#define PADDR_MASK              ((1ULL << PADDR_BITS)-1)
#define to_virt(x)                 ((void *)((unsigned long)(x)+VIRT_START))

#define pte_to_mfn(_pte)           (((_pte) & (PADDR_MASK&PAGE_MASK)) >> L1_PAGETABLE_SHIFT)
#define pte_to_virt(_pte)          to_virt(mfn_to_pfn(pte_to_mfn(_pte)) << PAGE_SHIFT)

#define mfn_to_virt(_mfn)          (to_virt(mfn_to_pfn(_mfn) << PAGE_SHIFT))
#define pfn_to_virt(_pfn)          (to_virt((_pfn) << PAGE_SHIFT))


typedef unsigned long long paddr_t;
typedef unsigned long long maddr_t;
static __inline__ maddr_t phys_to_machine(paddr_t phys)
{
	maddr_t machine = pfn_to_mfn(phys >> PAGE_SHIFT);
	machine = (machine << PAGE_SHIFT) | (phys & ~PAGE_MASK);
	return machine;
}
#define virt_to_mach(_virt)        (phys_to_machine(to_phys(_virt)))


#define PFN_UP(x)	(((x) + PAGE_SIZE()-1) >> L1_PAGETABLE_SHIFT)
#define PFN_PHYS(x)	((uint64_t)(x) << L1_PAGETABLE_SHIFT)
#define PHYS_PFN(x)	((x) >> L1_PAGETABLE_SHIFT)

extern unsigned long nr_max_pages;
extern unsigned long nr_mem_pages;

extern void xen_memory_init();

#define round_pgdown(_p)  ((_p) & PAGE_MASK)
#define round_pgup(_p)    (((_p) + (PAGE_SIZE() - 1)) & PAGE_MASK)

extern unsigned long nr_free_pages;

extern unsigned long *mm_alloc_bitmap;
extern unsigned long mm_alloc_bitmap_size;
int chk_free_pages(unsigned long needed);
#define BALLOON_EMERGENCY_PAGES   64
#define smp_processor_id() 0
#define irqs_disabled()			\
    xen_shared_info.vcpu_info[smp_processor_id()].evtchn_upcall_mask
void get_max_pages(void);
int balloon_up(unsigned long n_pages);
unsigned long alloc_pages(int order);
#endif /* XEN_MEMORY_ */
