#include <xen_memory.h>
#include <kernel/printk.h>
#include <xen/xen.h>
#include <xen/memory.h>
#include <xen_hypercall-x86_32.h>
#include <string.h>
#include <embox/test.h>

#define	ENOMEM		12	/* Out of memory */
#if 1
//phymem reuse
#include <util/log.h>

#include <stdint.h>
#include <sys/mman.h>
#include <mem/page.h>
#include <util/binalign.h>

#include <embox/unit.h>

struct page_allocator *__xen_mem_allocator;

static int xen_mem_init(char *const xen_mem_alloc_start, char *const xen_mem_alloc_end) {
	const size_t mem_len = xen_mem_alloc_end - xen_mem_alloc_start;
	void *va;

	printk("start=%p end=%p size=%zu\n", xen_mem_alloc_start, xen_mem_alloc_end, mem_len);

	va = mmap_device_memory(xen_mem_alloc_start,
			mem_len,
			PROT_WRITE | PROT_READ,
			MAP_FIXED,
			(uint64_t)((uintptr_t) xen_mem_alloc_start));

	if (va) {
		__xen_mem_allocator = page_allocator_init(xen_mem_alloc_start, mem_len, PAGE_SIZE());
	}
    
	return xen_mem_alloc_start == va ? 0 : -EIO;
}

void *xen_mem_alloc(size_t page_number) {
	return page_alloc(__xen_mem_allocator, page_number);
}

void xen_mem_free(void *page, size_t page_number) {
	page_free(__xen_mem_allocator, page, page_number);
}

struct page_allocator *xen_allocator(void) {
	return __xen_mem_allocator;
}
// end of phymem
#endif

extern start_info_t my_start_info;


typedef uint64_t pgentry_t;

unsigned long *phys_to_machine_mapping;
pgentry_t *pt_base;
unsigned long mfn_zero;
static unsigned long first_free_pfn;
static unsigned long last_free_pfn;
unsigned long nr_max_pages;
unsigned long nr_mem_pages;

void get_max_pages(void)
{
    long ret;
    domid_t domid = DOMID_SELF;

    ret = HYPERVISOR_memory_op(XENMEM_maximum_reservation, &domid);
    if ( ret < 0 )
    {
        printk("Could not get maximum pfn\n");
        return;
    }

    nr_max_pages = ret;
    printk("Maximum memory size: %ld pages\n", nr_max_pages);
}

void do_exit(void) 
{
    test_assert_equal(0,1);
}
/*
 * Make pt_pfn a new 'level' page table frame and hook it into the page
 * table at offset in previous level MFN (pref_l_mfn). pt_pfn is a guest
 * PFN.
 */
static pgentry_t pt_prot[PAGETABLE_LEVELS] = {
    L1_PROT,
    L2_PROT,
    L3_PROT
};

static void new_pt_frame(unsigned long *pt_pfn, unsigned long prev_l_mfn, 
                         unsigned long offset, unsigned long level)
{   
    pgentry_t *tab;
    unsigned long pt_page = (unsigned long)pfn_to_virt(*pt_pfn); 
    mmu_update_t mmu_updates[1];
    int rc;

    
    printk("Allocating new L%ld pt frame for pfn=%lx, "
          "prev_l_mfn=%lx, offset=%lx\n", 
          level, *pt_pfn, prev_l_mfn, offset);

    /* We need to clear the page, otherwise we might fail to map it
       as a page table page */
    memset((void*) pt_page, 0, PAGE_SIZE());  


    if(!(level >= 1 && level <= PAGETABLE_LEVELS))
    {
        printk("CRITICAL ERROR: FIX me!!!!\n");
    }

    /* Make PFN a page table page */
    tab = pt_base;

    tab = pte_to_virt(tab[l3_table_offset(pt_page)]);

    mmu_updates[0].ptr = (tab[l2_table_offset(pt_page)] & PAGE_MASK) + 
        sizeof(pgentry_t) * l1_table_offset(pt_page);
    mmu_updates[0].val = (pgentry_t)pfn_to_mfn(*pt_pfn) << PAGE_SHIFT | 
        (pt_prot[level - 1] & ~_PAGE_RW);
    
    if ( (rc = HYPERVISOR_mmu_update(mmu_updates, 1, NULL, DOMID_SELF)) < 0 )
    {
        printk("ERROR: PTE for new page table page could not be updated\n");
        printk("       mmu_update failed with rc=%d\n", rc);
        do_exit();
    }

    /* Hook the new page table page into the hierarchy */
    mmu_updates[0].ptr =
        ((pgentry_t)prev_l_mfn << PAGE_SHIFT) + sizeof(pgentry_t) * offset;
    mmu_updates[0].val = (pgentry_t)pfn_to_mfn(*pt_pfn) << PAGE_SHIFT |
        pt_prot[level];

    if ( (rc = HYPERVISOR_mmu_update(mmu_updates, 1, NULL, DOMID_SELF)) < 0 ) 
    {
        printk("ERROR: mmu_update failed with rc=%d\n", rc);
        do_exit();
    }

    *pt_pfn += 1;
}
/*
 * Build the initial pagetable.
 */
static void build_pagetable(unsigned long *start_pfn, unsigned long *max_pfn)
{
    unsigned long start_address, end_address;
    unsigned long pfn_to_map, pt_pfn = *start_pfn;
    pgentry_t *tab = pt_base, page;
    unsigned long pt_mfn = pfn_to_mfn(virt_to_pfn(pt_base));
    unsigned long offset;
    static mmu_update_t mmu_updates[L1_PAGETABLE_ENTRIES + 1];
    int count = 0;
    int rc;

    /* Be conservative: even if we know there will be more pages already
       mapped, start the loop at the very beginning. */
    pfn_to_map = *start_pfn;

    if ( *max_pfn >= virt_to_pfn(HYPERVISOR_VIRT_START) )
    {
        printk("WARNING: Mini-OS trying to use Xen virtual space. "
               "Truncating memory from %luMB to ",
               ((unsigned long)pfn_to_virt(*max_pfn) - VIRT_START)>>20);
        *max_pfn = virt_to_pfn(HYPERVISOR_VIRT_START - PAGE_SIZE());
        printk("%luMB\n",
               ((unsigned long)pfn_to_virt(*max_pfn) - VIRT_START)>>20);
    }


    start_address = (unsigned long)pfn_to_virt(pfn_to_map);
    end_address = (unsigned long)pfn_to_virt(*max_pfn);

    /* We worked out the virtual memory range to map, now mapping loop */
    printk("Mapping memory range 0x%lx - 0x%lx\n", start_address, end_address);

    while ( start_address < end_address )
    {
        tab = pt_base;
        pt_mfn = pfn_to_mfn(virt_to_pfn(pt_base));


        offset = l3_table_offset(start_address);
        /* Need new L2 pt frame */
        if ( !(tab[offset] & _PAGE_PRESENT) )
            new_pt_frame(&pt_pfn, pt_mfn, offset, L2_FRAME);

        page = tab[offset];
        pt_mfn = pte_to_mfn(page);
        tab = to_virt(mfn_to_pfn(pt_mfn) << PAGE_SHIFT);
        offset = l2_table_offset(start_address);        

        /* Need new L1 pt frame */
        if ( !(tab[offset] & _PAGE_PRESENT) )
            new_pt_frame(&pt_pfn, pt_mfn, offset, L1_FRAME);

        page = tab[offset];
        pt_mfn = pte_to_mfn(page);
        tab = to_virt(mfn_to_pfn(pt_mfn) << PAGE_SHIFT);
        offset = l1_table_offset(start_address);

        if ( !(tab[offset] & _PAGE_PRESENT) )
        {
            mmu_updates[count].ptr =
                ((pgentry_t)pt_mfn << PAGE_SHIFT) + sizeof(pgentry_t) * offset;
            mmu_updates[count].val =
                (pgentry_t)pfn_to_mfn(pfn_to_map) << PAGE_SHIFT | L1_PROT;
            count++;
        }
        pfn_to_map++;
        if ( count == L1_PAGETABLE_ENTRIES ||
             (count && pfn_to_map == *max_pfn) )
        {
            rc = HYPERVISOR_mmu_update(mmu_updates, count, NULL, DOMID_SELF);
            if ( rc < 0 )
            {
                printk("ERROR: build_pagetable(): PTE could not be updated\n");
                printk("       mmu_update failed with rc=%d\n", rc);
                do_exit();
            }
            count = 0;
        }
        start_address += PAGE_SIZE();
    }

    *start_pfn = pt_pfn;
}
#if 0
/*
 * Clear some of the bootstrap memory
 */
static void clear_bootstrap(void)
{
    pte_t nullpte = { };
    int rc;

    /* Use first page as the CoW zero page */
    memset((void*)VIRT_START, 0, PAGE_SIZE());
    mfn_zero = virt_to_mfn(VIRT_START);

    if ( (rc = HYPERVISOR_update_va_mapping(0, nullpte, UVMF_INVLPG)) )
        printk("Unable to unmap NULL page. rc=%d\n", rc);
}
#endif

#define MAX_MEM_SIZE            0x3f000000ULL
void arch_init_mm(unsigned long* start_pfn_p, unsigned long* max_pfn_p)
{
    unsigned long start_pfn, max_pfn;

    /* First page follows page table pages. */
    start_pfn = first_free_pfn;
    max_pfn = last_free_pfn;

    if ( max_pfn >= MAX_MEM_SIZE / PAGE_SIZE() )
    {
        max_pfn = MAX_MEM_SIZE / PAGE_SIZE() - 1;
    }

    printk("  start_pfn: %lx\n", start_pfn);
    printk("    max_pfn: %lx\n", max_pfn);

    build_pagetable(&start_pfn, &max_pfn);
    //clear_bootstrap();
    //set_readonly(&_text, &_erodata); //TODO: remove _text!!!!

    *start_pfn_p = start_pfn;
    *max_pfn_p = max_pfn;
}
#if 0 //allocator
/*********************
 * ALLOCATION BITMAP
 *  One bit per page of memory. Bit set => page is allocated.
 */

unsigned long *mm_alloc_bitmap;
unsigned long mm_alloc_bitmap_size;

#define PAGES_PER_MAPWORD (sizeof(unsigned long) * 8)

#define allocated_in_map(_pn) \
    (mm_alloc_bitmap[(_pn) / PAGES_PER_MAPWORD] & \
     (1UL << ((_pn) & (PAGES_PER_MAPWORD - 1))))

unsigned long nr_free_pages;

static void map_free(unsigned long first_page, unsigned long nr_pages)
{
    unsigned long start_off, end_off, curr_idx, end_idx;

    curr_idx = first_page / PAGES_PER_MAPWORD;
    start_off = first_page & (PAGES_PER_MAPWORD-1);
    end_idx   = (first_page + nr_pages) / PAGES_PER_MAPWORD;
    end_off   = (first_page + nr_pages) & (PAGES_PER_MAPWORD-1);

    nr_free_pages += nr_pages;

    if ( curr_idx == end_idx )
    {
        mm_alloc_bitmap[curr_idx] &= -(1UL<<end_off) | ((1UL<<start_off)-1);
    }
    else 
    {
        mm_alloc_bitmap[curr_idx] &= (1UL<<start_off)-1;
        while ( ++curr_idx != end_idx ) mm_alloc_bitmap[curr_idx] = 0;
        mm_alloc_bitmap[curr_idx] &= -(1UL<<end_off);
    }
}

/*************************
 * BINARY BUDDY ALLOCATOR
 */

typedef struct chunk_head_st chunk_head_t;
typedef struct chunk_tail_st chunk_tail_t;

struct chunk_head_st {
    chunk_head_t  *next;
    chunk_head_t **pprev;
    int            level;
};

struct chunk_tail_st {
    int level;
};

/* Linked lists of free chunks of different powers-of-two in size. */
#define FREELIST_SIZE ((sizeof(void*)<<3)-PAGE_SHIFT)
static chunk_head_t *free_head[FREELIST_SIZE];
static chunk_head_t  free_tail[FREELIST_SIZE];
#define FREELIST_EMPTY(_l) ((_l)->next == NULL)

struct __attribute__((__packed__)) e820entry {
    uint64_t addr;
    uint64_t size;
    uint32_t type;
};
#define E820_RAM          1
struct e820entry e820_map[1] = {
    {
        .addr = 0,
        .size = ULONG_MAX - 1,
        .type = E820_RAM
    }
};
unsigned e820_entries = 1;


#define N_BALLOON_FRAMES 64
static unsigned long balloon_frames[N_BALLOON_FRAMES];

int balloon_up(unsigned long n_pages)
{
    unsigned long page, pfn;
    int rc;
    struct xen_memory_reservation reservation = {
        .domid        = DOMID_SELF
    };

    if ( n_pages > nr_max_pages - nr_mem_pages )
        n_pages = nr_max_pages - nr_mem_pages;
    if ( n_pages > N_BALLOON_FRAMES )
        n_pages = N_BALLOON_FRAMES;

    /* Resize alloc_bitmap if necessary. */
    while ( mm_alloc_bitmap_size * 8 < nr_mem_pages + n_pages )
    {
        page = alloc_pages(0);
        if ( !page )
            return -ENOMEM;

        memset((void *)page, ~0, PAGE_SIZE);
        if ( map_frame_rw((unsigned long)mm_alloc_bitmap + mm_alloc_bitmap_size,
                          virt_to_mfn(page)) )
        {
            free_page((void *)page);
            return -ENOMEM;
        }

        mm_alloc_bitmap_size += PAGE_SIZE;
    }

    rc = arch_expand_p2m(nr_mem_pages + n_pages);
    if ( rc )
        return rc;

    /* Get new memory from hypervisor. */
    for ( pfn = 0; pfn < n_pages; pfn++ )
    {
        balloon_frames[pfn] = nr_mem_pages + pfn;
    }
    set_xen_guest_handle(reservation.extent_start, balloon_frames);
    reservation.nr_extents = n_pages;
    rc = HYPERVISOR_memory_op(XENMEM_populate_physmap, &reservation);
    if ( rc <= 0 )
        return rc;

    for ( pfn = 0; pfn < rc; pfn++ )
    {
        arch_pfn_add(nr_mem_pages + pfn, balloon_frames[pfn]);
        free_page(pfn_to_virt(nr_mem_pages + pfn));
    }

    nr_mem_pages += rc;

    return rc;
}

static int in_balloon;

int chk_free_pages(unsigned long needed)
{
    unsigned long n_pages;

    /* No need for ballooning if plenty of space available. */
    if ( needed + BALLOON_EMERGENCY_PAGES <= nr_free_pages )
        return 1;

    /* If we are already ballooning up just hope for the best. */
    if ( in_balloon )
        return 1;

    /* Interrupts disabled can't be handled right now. */
    if ( irqs_disabled() )
        return 1;

    in_balloon = 1;

    while ( needed + BALLOON_EMERGENCY_PAGES > nr_free_pages )
    {
        n_pages = needed + BALLOON_EMERGENCY_PAGES - nr_free_pages;
        if ( !balloon_up(n_pages) )
            break;
    }

    in_balloon = 0;

    return needed <= nr_free_pages;
}

/* Allocate 2^@order contiguous pages. Returns a VIRTUAL address. */
unsigned long alloc_pages(int order)
{
    int i;
    chunk_head_t *alloc_ch, *spare_ch;
    chunk_tail_t            *spare_ct;

    if ( !chk_free_pages(1UL << order) )
        goto no_memory;

    /* Find smallest order which can satisfy the request. */
    for ( i = order; i < FREELIST_SIZE; i++ ) {
	if ( !FREELIST_EMPTY(free_head[i]) ) 
	    break;
    }

    if ( i == FREELIST_SIZE ) goto no_memory;
 
    /* Unlink a chunk. */
    alloc_ch = free_head[i];
    free_head[i] = alloc_ch->next;
    alloc_ch->next->pprev = alloc_ch->pprev;

    /* We may have to break the chunk a number of times. */
    while ( i != order )
    {
        /* Split into two equal parts. */
        i--;
        spare_ch = (chunk_head_t *)((char *)alloc_ch + (1UL<<(i+PAGE_SHIFT)));
        spare_ct = (chunk_tail_t *)((char *)spare_ch + (1UL<<(i+PAGE_SHIFT)))-1;

        /* Create new header for spare chunk. */
        spare_ch->level = i;
        spare_ch->next  = free_head[i];
        spare_ch->pprev = &free_head[i];
        spare_ct->level = i;

        /* Link in the spare chunk. */
        spare_ch->next->pprev = &spare_ch->next;
        free_head[i] = spare_ch;
    }
    
    map_alloc(PHYS_PFN(to_phys(alloc_ch)), 1UL<<order);

    return((unsigned long)alloc_ch);

 no_memory:

    printk("Cannot handle page request order %d!\n", order);

    return 0;
}
/*
 * return a valid PTE for a given virtual address. If PTE does not exist,
 * allocate page-table pages.
 */
pgentry_t *need_pgt(unsigned long va)
{
    unsigned long pt_mfn;
    pgentry_t *tab;
    unsigned long pt_pfn;
    unsigned offset;

    tab = pt_base;
    pt_mfn = virt_to_mfn(pt_base);

    offset = l3_table_offset(va);
    if ( !(tab[offset] & _PAGE_PRESENT) ) 
    {
        pt_pfn = virt_to_pfn(alloc_pages(0));
        if ( !pt_pfn )
            return NULL;
        new_pt_frame(&pt_pfn, pt_mfn, offset, L2_FRAME);
    }
    ASSERT(tab[offset] & _PAGE_PRESENT);
    pt_mfn = pte_to_mfn(tab[offset]);
    tab = mfn_to_virt(pt_mfn);
    offset = l2_table_offset(va);
    if ( !(tab[offset] & _PAGE_PRESENT) )
    {
        pt_pfn = virt_to_pfn(alloc_pages(0));
        if ( !pt_pfn )
            return NULL;
        new_pt_frame(&pt_pfn, pt_mfn, offset, L1_FRAME);
    }
    ASSERT(tab[offset] & _PAGE_PRESENT);
    if ( tab[offset] & _PAGE_PSE )
        return &tab[offset];

    pt_mfn = pte_to_mfn(tab[offset]);
    tab = mfn_to_virt(pt_mfn);

    offset = l1_table_offset(va);
    return &tab[offset];
}

/*
 * Map an array of MFNs contiguously into virtual address space starting at
 * va. map f[i*stride]+i*increment for i in 0..n-1.
 */
#define MAP_BATCH ((STACK_SIZE / 2) / sizeof(mmu_update_t))
int do_map_frames(unsigned long va,
                  const unsigned long *mfns, unsigned long n,
                  unsigned long stride, unsigned long incr,
                  domid_t id, int *err, unsigned long prot)
{
    pgentry_t *pgt = NULL;
    unsigned long done = 0;

    if ( !mfns ) 
    {
        printk("do_map_frames: no mfns supplied\n");
        return -EINVAL;
    }
    printk("va=%p n=0x%lx, mfns[0]=0x%lx stride=0x%lx incr=0x%lx prot=0x%lx\n",
          va, n, mfns[0], stride, incr, prot);

    if ( err )
        memset(err, 0x00, n * sizeof(int));
    while ( done < n )
    {

        unsigned long i;
        int rc;
        unsigned long todo;

        if ( err )
            todo = 1;
        else
            todo = n - done;

        if ( todo > MAP_BATCH )
            todo = MAP_BATCH;

        {
            mmu_update_t mmu_updates[todo];

            for ( i = 0; i < todo; i++, va += PAGE_SIZE, pgt++) 
            {
                if ( !pgt || !(va & L1_MASK) )
                    pgt = need_pgt(va);
                if ( !pgt )
                    return -ENOMEM;

                mmu_updates[i].ptr = virt_to_mach(pgt) | MMU_NORMAL_PT_UPDATE;
                mmu_updates[i].val = ((pgentry_t)(mfns[(done + i) * stride] +
                                                  (done + i) * incr)
                                      << PAGE_SHIFT) | prot;
            }

            rc = HYPERVISOR_mmu_update(mmu_updates, todo, NULL, id);
            if ( rc < 0 )
            {
                if (err)
                    err[done * stride] = rc;
                else {
                    printk("Map %ld (%lx, ...) at %lx failed: %d.\n",
                           todo, mfns[done * stride] + done * incr, va, rc);
                    do_exit();
                }
            }
        }
        done += todo;
    }

    return 0;
}


void mm_alloc_bitmap_remap(void)
{
    unsigned long i, new_bitmap;

    if ( mm_alloc_bitmap_size >= ((nr_max_pages + 1) >> 3) )
        return;

    new_bitmap = alloc_virt_kernel(PFN_UP((nr_max_pages + 1) >> 3));
    for ( i = 0; i < mm_alloc_bitmap_size; i += PAGE_SIZE() )
    {
        do_map_frames(new_bitmap + i, &(virt_to_mfn((unsigned long)(mm_alloc_bitmap) + i)), 1, 1, 1, DOMID_SELF, NULL, L1_PROT);
    }

    mm_alloc_bitmap = (unsigned long *)new_bitmap;
}
/*
 * Initialise allocator, placing addresses [@min,@max] in free pool.
 * @min and @max are PHYSICAL addresses.
 */
static void init_page_allocator(unsigned long min, unsigned long max)
{
    int i, m;
    unsigned long range;
    unsigned long r_min, r_max;
    chunk_head_t *ch;
    chunk_tail_t *ct;

    printk("MM: Initialise page allocator for %lx(%lx)-%lx(%lx)\n",
           (unsigned long)to_virt(min), min, (unsigned long)to_virt(max), max);
    for ( i = 0; i < FREELIST_SIZE; i++ )
    {
        free_head[i]       = &free_tail[i];
        free_tail[i].pprev = &free_head[i];
        free_tail[i].next  = NULL;
    }

    min = round_pgup  (min);
    max = round_pgdown(max);

    /* Allocate space for the allocation bitmap. */
    mm_alloc_bitmap_size  = (max + 1) >> (PAGE_SHIFT + 3);
    mm_alloc_bitmap_size  = round_pgup(mm_alloc_bitmap_size);
    mm_alloc_bitmap = (unsigned long *)to_virt(min);
    min         += mm_alloc_bitmap_size;

    /* All allocated by default. */
    memset(mm_alloc_bitmap, ~0, mm_alloc_bitmap_size);

    for ( m = 0; m < e820_entries; m++ )
    {
        if ( e820_map[m].type != E820_RAM )
            continue;
        if ( e820_map[m].addr + e820_map[m].size >= ULONG_MAX )
            test_assert_equal(0,1);

        r_min = e820_map[m].addr;
        r_max = r_min + e820_map[m].size;
        if ( r_max <= min || r_min >= max )
            continue;
        if ( r_min < min )
            r_min = min;
        if ( r_max > max )
            r_max = max;

        printk("    Adding memory range %lx-%lx\n", r_min, r_max);

        /* The buddy lists are addressed in high memory. */
        r_min = (unsigned long)to_virt(r_min);
        r_max = (unsigned long)to_virt(r_max);
        range = r_max - r_min;

        /* Free up the memory we've been given to play with. */
        map_free(PHYS_PFN(r_min), range >> PAGE_SHIFT);

        while ( range != 0 )
        {
            /*
             * Next chunk is limited by alignment of min, but also
             * must not be bigger than remaining range.
             */
            for ( i = PAGE_SHIFT; (1UL << (i + 1)) <= range; i++ )
                if ( r_min & (1UL << i) ) break;

            ch = (chunk_head_t *)r_min;
            r_min += 1UL << i;
            range -= 1UL << i;
            ct = (chunk_tail_t *)r_min - 1;
            i -= PAGE_SHIFT;
            ch->level       = i;
            ch->next        = free_head[i];
            ch->pprev       = &free_head[i];
            ch->next->pprev = &ch->next;
            free_head[i]    = ch;
            ct->level       = i;
        }
    }

    mm_alloc_bitmap_remap();
}
#endif

void memory_init()
{
    phys_to_machine_mapping = (unsigned long *)my_start_info.mfn_list;
    pt_base = (pgentry_t *)my_start_info.pt_base;
    first_free_pfn = PFN_UP(to_phys(pt_base)) + my_start_info.nr_pt_frames;
    last_free_pfn = my_start_info.nr_pages;



    unsigned long start_pfn, max_pfn;

    printk("MM: Init\n");

    get_max_pages();

    arch_init_mm(&start_pfn, &max_pfn);
    printk("start_pfn=%lu, max_pfn=%lu\n", start_pfn, max_pfn);
    xen_mem_init(pfn_to_virt(start_pfn), pfn_to_virt(max_pfn));
    /*printk("xen_mem_alloc=%lu\n", (unsigned long)xen_mem_alloc(256));
    printk("xen_mem_alloc=%p\n", xen_mem_alloc(1));
    printk("xen_mem_alloc=%lu\n", virt_to_mfn(xen_mem_alloc(1)));
    printk("xen_mem_alloc=%lu\n", virt_to_mfn(xen_mem_alloc(1)));
    */
    /*
     * now we can initialise the page allocator
     */
    //init_page_allocator(PFN_PHYS(start_pfn), PFN_PHYS(max_pfn));
    printk("MM: done\n");
#if 0
    arch_init_p2m(max_pfn);
    
    arch_init_demand_mapping_area();
//CONFIG_BALLOON
    nr_mem_pages = max_pfn;
#endif
}
