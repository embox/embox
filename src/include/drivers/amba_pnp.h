/**
 * @file
 * @details Driver for devices founded on the Amba Plug&Play bus
 * (Advanced Microcontroller Bus Architecture)
 *
 * @date 28.01.09
 * @author Alexandr Batyukov, Alexey Fomin, Eldar Abusalimov
 */

#ifndef AMBA_PNP_H_
#define AMBA_PNP_H_

#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

#include <drivers/ambapp.h>

#define TRY_CAPTURE_AHBM_DEV(dev,venID,devID) \
	if (-1 == capture_amba_dev(dev, venID, devID, true, true)) { \
	LOG_ERROR("can't capture ahbm dev venID=0x%X, devID=0x%X\n", \
			venID, devID); \
	return -1; \
}

#define TRY_CAPTURE_AHBSL_DEV(dev,venID,devID) \
	if (-1 == capture_amba_dev(dev, venID, devID, true, false)) { \
	LOG_ERROR("can't capture ahbsl dev venID=0x%X, devID=0x%X\n", \
			venID, devID); \
	return -1; \
}

#define TRY_CAPTURE_APB_DEV(dev,venID,devID) \
	if (-1 == capture_amba_dev(dev, venID, devID, false, false)) { \
	LOG_ERROR("can't capture apb dev venID=0x%X, devID=0x%X\n",    \
			venID, devID); \
	return -1; \
}

#define AHB_MASTER_BASE          0xFFFFF000 // max: 64 devices
#define AHB_SLAVE_BASE           0xFFFFF800 // max: 64 devices
#define APB_BASE                 0x800FF000 // max: 512 devices

/* max devices on AMBA bus */
#define AHB_MASTERS_QUANTITY     0x10
#define AHB_SLAVES_QUANTITY      0x40
#define APB_QUANTITY             0x40

/**
 * Amba Bank Address Register Info.
 * BAR0-BAR3 registers are presented.
 */
typedef struct amba_bar_info {
	uint32_t start;
	bool     prefetchable;
	bool     cacheable;
	uint32_t mask;
	uint8_t  type;
	bool     used;
} amba_bar_info_t;

/**
 * Amba device info
 */
typedef struct amba_dev_info {
	uint8_t   venID;
	uint16_t  devID;
	uint8_t   version;
	uint8_t   irq;
} amba_dev_info_t;

struct amba_dev;
/* each device must have handler */
typedef void (*HANDLER_DATA_FUNC)(struct amba_dev *dev);

/**
 * Amba device
 */
typedef struct amba_dev {
	amba_dev_info_t    dev_info;     /**< VendorID, DeviceID, version, IRQ */
	amba_bar_info_t    bar[4];
	uint8_t            slot;         /**< information about location */
	HANDLER_DATA_FUNC  show_info;    /**< show brief description */
	char               dev_name[16]; /**< logical name */
	bool               is_ahb;
	bool               is_master;
	uint32_t           user_def[3];  /**< info from user registers */
} amba_dev_t;

/** AHB master devices */
extern amba_dev_t *ahbm_devices[AHB_MASTERS_QUANTITY];
/** AHB slave devices */
extern amba_dev_t *ahbsl_devices[AHB_SLAVES_QUANTITY];
/** APB devices */
extern amba_dev_t *apb_devices[APB_QUANTITY];

/*
 * pnp_dev must be allocated by caller
 * returns 0 if ok, non-zero otherwise
 */
//int capture_ahbm_dev(AHB_DEV *ahb_dev, uint8_t vendor_id, uint16_t device_id);
/*
 * pnp_dev must be allocated by caller
 * returns 0 if ok, non-zero otherwise
 */
//int capture_ahbsl_dev(AHB_DEV *ahb_dev, uint8_t vendor_id, uint16_t device_id);

/**
 * Fill amba device.
 * @param dev amba device
 * @param slot_number slot number
 * @param is_ahb ahb/not ahb
 * @param is_master master/slave
 * @return true (1) if successed
 * @return false (0) slot is empty
 */
extern int fill_amba_dev(amba_dev_t *dev, uint8_t slot_number,
				bool is_ahb, bool is_master);

/**
 * Capture amba pnp device.
 * @param apb_dev amba_dev_t
 * @param vendor_id vendor ID
 * @param device_id device ID
 * @param is_ahb ahb/not ahb
 * @param is_master master/slave
 * @return slot number or -1 if error
 */
extern int capture_amba_dev(amba_dev_t *apb_dev, uint8_t vendor_id,
			uint16_t device_id, bool is_ahb, bool is_master);

/**
 * Free amba device.
 */
extern int free_amba_dev(amba_dev_t *dev);

#endif /* AMBA_PNP_H_ */
