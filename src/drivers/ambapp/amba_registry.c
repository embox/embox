/**
 * @file
 * @brief Prepares data for lspnp user command
 *
 * @date 26.04.10
 * @author Anton Bondarev
 */

#include <linux/list.h>
#include <lib/libds/array.h>
#include <drivers/amba_registry.h>
#include <embox/unit.h>
#include <util/log.h>

EMBOX_UNIT_INIT(init);

static amba_registry_device_info_t const devices_table[] = {
    #include "pnp_devices_table.inc"
};

static amba_registry_vendor_info_t const vendors_table[] = {
    #include "pnp_vendors_table.inc"
};

static amba_registry_vendor_info_t unknown_vendor[1];

static amba_registry_vendor_entry_t vendors_pool[ARRAY_SIZE(vendors_table)];
static amba_registry_vendor_entry_t unknown_vendor_entry[1];

static amba_registry_device_entry_t devices_pool[ARRAY_SIZE(devices_table)];

static LIST_HEAD(head_vendor_list);

static inline amba_registry_vendor_entry_t *find_vendor_entry(uint8_t ven_id) {
	struct list_head * p;
	uint8_t id;
	list_for_each(p, &head_vendor_list) {
		id = ((amba_registry_vendor_entry_t *) p)->ven_info->ven_id;
		if (id == ven_id) {
			return (amba_registry_vendor_entry_t *) p;
		}
	}
	return NULL;
}

static inline amba_registry_device_entry_t *find_device_entry(
		struct list_head *dev_head, uint16_t dev_id) {
	struct list_head * p;
	uint8_t id;
	list_for_each(p, dev_head) {
		id = ((amba_registry_device_entry_t *) p)->dev_info->dev_id;
		if (id == dev_id) {
			return (amba_registry_device_entry_t *) p;
		}
	}
	return NULL;
}

inline void add_dev_to_ven(const amba_registry_device_info_t * dev,
		amba_registry_device_entry_t *entry) {
	amba_registry_vendor_entry_t *ven_entry;
	if (NULL == (ven_entry = find_vendor_entry(dev->ven_id))) {
		list_add((struct list_head *) &entry,
				&unknown_vendor_entry->dev_list);
		return;
	}

	entry->dev_info = (amba_registry_device_info_t *) dev;
	list_add((struct list_head *) entry, &ven_entry->dev_list);
}

static int init(void) {
	size_t i;
	for (i = 0; i < ARRAY_SIZE(vendors_table); i++) {
		(&vendors_pool[i])->ven_info
			= (amba_registry_vendor_info_t *) &vendors_table[i];
		INIT_LIST_HEAD(&((&vendors_pool[i])->dev_list));
		list_add((struct list_head *) &vendors_pool[i],
				&head_vendor_list);
	}
	unknown_vendor_entry->ven_info = unknown_vendor;

	for (i = 0; i < ARRAY_SIZE(devices_table); i++) {
		add_dev_to_ven(&devices_table[i], &devices_pool[i]);
	}

	return 0;
}

amba_registry_vendor_info_t *amba_registry_get_head(void) {
	return (amba_registry_vendor_info_t *) &head_vendor_list;
}

char* amba_registry_get_ven_name(uint8_t ven_id) {
	amba_registry_vendor_entry_t *entry;
	log_debug("ven_id=0x%X", ven_id);
	if (NULL == (entry = find_vendor_entry(ven_id))) {
		return "Unknown";
	}
	return (char*) entry->ven_info->ven_name;
}

char* amba_registry_get_dev_name(uint8_t ven_id, uint16_t dev_id) {
	amba_registry_vendor_entry_t *ven_entry;
	amba_registry_device_entry_t *dev_entry;

	if (NULL == (ven_entry = find_vendor_entry(ven_id))) {
		return "Unknown";
	}
	log_debug("ven_id=0x%X, dev_id=0x%X", ven_id, dev_id);
	dev_entry = find_device_entry(&ven_entry->dev_list, dev_id);
	if (NULL == dev_entry) {
		return "Unknown";
	}
	return (char*) dev_entry->dev_info->dev_name;
}
