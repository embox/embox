/**
 * @file
 *
 * @brief Prepares data for lspnp user command
 *
 * @date 26.04.2010
 * @author Anton Bondarev
 */

#include <lib/list.h>
#include <embox/kernel.h>
#include <drivers/amba_registry.h>


static device_info_t const devices_table[] = {
	#include <pnp_devices_table.inc>
};

static vendor_info_t const vendors_table[] = {
	#include <vendors_table.inc>
};

static vendor_info_t unknown_vendor[1];

static vendor_entry_t vendors_pool[array_len(vendors_table)];
static vendor_entry_t unknown_vendors_entry[1];

static device_entry_t devices_pool[array_len(devices_table)];

static LIST_HEAD(head_vendor_list);

static inline vendor_entry_t *find_vendor_entry(uint8_t ven_id) {
	vendor_entry_t *entry;
	struct list_head * p;
	list_for_each(p, (&head_vendor_list)) {
		if (entry->ven_info->ven_id == ven_id){
			return entry;
		}
	}
	return NULL;
}

static inline void add_dev_to_ven(const device_info_t * dev, device_entry_t *entry) {
	vendor_entry_t *ven_entry = find_vendor_entry(dev->ven_id);

	list_add((struct list_head *)&entry, &ven_entry->dev_list);
}

static int init(void) {
	int i;
	for (i = 0; i < array_len(vendors_table); i ++) {
		(&vendors_pool[i])->ven_info = (vendor_info_t *)&vendors_table[i];
		list_add((struct list_head *)&vendors_pool[i], &head_vendor_list);
	}
	unknown_vendors_entry->ven_info = unknown_vendor;

	for (i = 0; i < array_len(devices_table); i ++) {
		add_dev_to_ven(&devices_table[i], &devices_pool[i]);
	}

	return 0;
}

struct list_head *get_amba_registry() {
	return &head_vendor_list;
}

