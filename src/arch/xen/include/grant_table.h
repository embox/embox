/**
 * @file grant_table.h
 * @brief Grant table mechanism implementation
 *
 * @date 13.04.2020
 * @author Cherepanov Aleksei
 */

#ifndef GRANT_TABLE_H_
#define GRANT_TABLE_H_
#include <stdint.h>
#include <xen/grant_table.h>

#define NR_GRANT_FRAMES 4
#define NR_GRANT_ENTRIES (NR_GRANT_FRAMES * PAGE_SIZE() / sizeof(grant_entry_v1_t)) //make changable

int init_grant_table(int n);
grant_ref_t gnttab_grant_access(domid_t domid, unsigned long frame, int readonly);
int gnttab_end_access(grant_ref_t ref);
grant_ref_t gnttab_regrant_access(grant_ref_t gref, int readonly);
#endif /* GRANT_TABLE_H_ */