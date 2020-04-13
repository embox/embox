/**
 * @file grant_table.c
 * @brief Grant table mechanism implementation
 *
 * @date 13.04.2020
 * @author Cherepanov Aleksei
 */

#include <xen_hypercall-x86_32.h>
#include <xen_memory.h>
#include <barrier.h>

#include "grant_table.h"

static grant_ref_t ref = 10; // first 8 entries are reserved 

grant_entry_v1_t *grant_table;

grant_ref_t gnttab_grant_access(domid_t domid, unsigned long frame, int readonly)
{
	//printk("grant access for ref:%d on addr:%p\n", ref, &grant_table[ref]);
    
    grant_table[ref].frame = frame;
    grant_table[ref].domid = domid;
    wmb();
    readonly *= GTF_readonly;
    grant_table[ref].flags = GTF_permit_access | readonly;

	//printk("frame setuped\n");
    return ref++;
}

int init_grant_table(int n) {
    //printk(">>>>>init_grant_table\n");

/*TODO detection
    int i;
    for (i = NR_RESERVED_ENTRIES; i < NR_GRANT_ENTRIES; i++)
        put_free_entry(i);
*/	
	struct gnttab_setup_table setup;
    setup.dom = DOMID_SELF;     //32752
    setup.nr_frames = n;
    unsigned long frames[n];
    set_xen_guest_handle(setup.frame_list, frames);

    int rc;
    rc = HYPERVISOR_grant_table_op(GNTTABOP_setup_table, &setup, 1);
    //printk("HYPERVISOR_grant_table_op returned:%d, status=%d\n", rc, setup.status);
    if(rc < 0) {
        return rc;
    }
    unsigned long va = (unsigned long)xen_mem_alloc(n);

    int count;    
    for(count = 0; count < n; count++)
    {
        //printk("entry %d mapped at %ld(mfn).\n", count, frames[count]);
        //printk("addr:%p\n", (void*)(va + count*PAGE_SIZE())); //+4k = +page
        rc = HYPERVISOR_update_va_mapping(va + count*PAGE_SIZE(),
                __pte((frames[count]<< PAGE_SHIFT) | 7),
                UVMF_INVLPG);
        if(rc < 0) {
            return rc;
        }
        //printk("HYPERVISOR_update_va_mapping:%d\n", rc);
    }
    
    //printk(">>>>>END OF init_grant_table\n");
   	grant_table = (void*)va;

    return 0;
}