/**
 * @file
 *
 * @date 26.04.2010
 * @author Anton Bondarev
 */

#ifndef AMBA_REGISTRY_H_
#define AMBA_REGISTRY_H_

#include <lib/list.h>

typedef struct vendor_info {
	const uint8_t ven_id;
	const char *ven_name;
} vendor_info_t;

typedef struct device_info {
	const uint8_t ven_id;
	const uint16_t dev_id;
	const char *dev_name;
} device_info_t;

typedef struct device_entry {
	struct list_head *next;
	struct list_head *prev;
	device_info_t *dev_info;
} device_entry_t;

typedef struct vendor_entry {
	struct list_head *next;
	struct list_head *prev;
	vendor_info_t *ven_info;
	struct list_head dev_list;
} vendor_entry_t;


#endif /* AMBA_REGISTRY_H_ */
