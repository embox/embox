/**
 * @file
 *
 * @date Feb 9, 2022
 * @author Anton Bondarev
 */

#include <lib/libds/dlist.h>

#include <mem/sysmalloc.h>

#include <kernel/irq/irq_msi.h>

struct msi_desc *msi_entry_alloc(void *dev, int nvec) {
	struct msi_desc *desc;

	desc = sysmalloc(sizeof(*desc));
	if (!desc) {
		return NULL;
	}

	dlist_head_init(&desc->list);
	desc->dev = dev;
	desc->nvec_used = nvec;

	return desc;
}

void msi_entry_free(struct msi_desc *entry) {

	sysfree(entry);
}
