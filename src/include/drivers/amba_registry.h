/**
 * @file
 * @brief Interface for working with amba registry. It's used in lspnp command.
 * @details AMBA registry is simply database contains information about devices
 *      identificators visible on AMBA bus, and links ID to character name
 *      device and vendor.
 *
 * @date 26.04.2010
 * @author Anton Bondarev
 */

#ifndef AMBA_REGISTRY_H_
#define AMBA_REGISTRY_H_

#include <drivers/amba_pnp.h>
#include <lib/list.h>

/**
 * Information describing vendor.
 */
typedef struct amba_registry_vendor_info {
	const uint8_t    ven_id;
	const char      *ven_name;
} amba_registry_vendor_info_t;

/**
 * Information describing device.
 */
typedef struct amba_registry_device_info {
	const uint8_t    ven_id;
	const uint16_t   dev_id;
	const char      *dev_name;
} amba_registry_device_info_t;

/**
 * Device entry in AMBA registry.
 */
typedef struct amba_registry_device_entry {
	struct list_head            *next;
	struct list_head            *prev;
	amba_registry_device_info_t *dev_info;
} amba_registry_device_entry_t;

/**
 * Vendor entry in AMBA registry. It contains also
 * list of device belong to this vendors.
 */
typedef struct amba_registry_vendor_entry {
	struct list_head            *next;
	struct list_head            *prev;
	amba_registry_vendor_info_t *ven_info;
	struct list_head             dev_list;
} amba_registry_vendor_entry_t;

/**
 * Returns head of amba registry. It's used adding new entities to registry,
 * for example if platform part contains own list of device.
 */
extern amba_registry_vendor_info_t *amba_registry_get_head(void);

/**
 * Get vendor name of amba pnp device.
 * @param ven_id vendor ID
 */
extern char *amba_registry_get_ven_name(uint8_t ven_id);

/**
 * Get device name of amba pnp device.
 * @param ven_id vendor ID
 * @param dev_id device ID
 */
extern char *amba_registry_get_dev_name(uint8_t ven_id, uint16_t dev_id);

/**
 * Add device to vendor list.
 */
void add_dev_to_ven(const amba_registry_device_info_t *dev,
                amba_registry_device_entry_t *entry);

#endif /* AMBA_REGISTRY_H_ */
