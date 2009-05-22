/**
 * \file amba_pnp.h
 *
 *  \date 28.01.2009
 *  \author Alexandr Batyukov, Alexey Fomin, Eldar Abusalimov
 */

#ifndef AMBA_PNP_H_
#define AMBA_PNP_H_

#include "pnp_id.h"
#include "common.h"

#define TRY_CAPTURE_AHBM_DEV(dev,venID,devID) if (-1 == capture_amba_dev(dev, venID, devID, TRUE, TRUE)){\
	ERROR("can't capture ahbm dev venID=0x%X, devID=0x%X\n", venID, devID);\
	return -1;\
}
#define TRY_CAPTURE_AHBSL_DEV(dev,venID,devID) if (-1 == capture_amba_dev(dev, venID, devID, TRUE, FALSE)){\
	ERROR("can't capture ahbsl dev venID=0x%X, devID=0x%X\n", venID, devID);\
	return -1;\
}

#define TRY_CAPTURE_APB_DEV(dev,venID,devID) if (-1 == capture_amba_dev(dev, venID, devID, FALSE, FALSE)){\
	ERROR("can't capture apb dev venID=0x%X, devID=0x%X\n", venID, devID);\
	return -1;\
}

#define AHB_MASTER_BASE          0xFFFFF000
#define AHB_SLAVE_BASE           0xFFFFF800
#define APB_BASE                 0x800FF000

#define AHB_MASTERS_QUANTITY     0x10
#define AHB_SLAVES_QUANTITY      0x40
#define APB_QUANTITY             0x40

/**
 * \struct AMBA_BAR_INFO
 */
typedef struct _AMBA_BAR_INFO {
	UINT32 start;
	BOOL   prefetchable;
	BOOL   cacheable;
	UINT32 mask;
	BYTE   type;
	BOOL   used;
} AMBA_BAR_INFO;

/**
 * \struct AMBA_DEV_INFO
 */
typedef struct _AMBA_DEV_INFO {
	BYTE   venID;
	UINT16 devID;
	BYTE   version;
	BYTE   irq;
} AMBA_DEV_INFO;

/**
 * \struct PNP_DEVICE_INFO
 */
typedef struct _PNP_DEVICE_INFO {
	const BYTE vendor_id;
        const UINT16 device_id;
        const char *name;
} PNP_DEVICE_INFO;

/**
 * \struct PNP_VENDOR_INFO
 */
typedef struct _PNP_VENDOR_INFO {
        const BYTE vendor_id;
        const char *name;
} PNP_VENDOR_INFO;

/**
 * \struct AHB_SLOT
 */
typedef struct _AHB_SLOT {
        UINT32 id_reg;
        UINT32 user_defined[3];
        UINT32 ba_reg[4];
}AHB_SLOT;

/**
 * \struct APB_SLOT
 */
typedef struct _APB_SLOT {
	UINT32 id_reg;
        UINT32 ba_reg[1];
}APB_SLOT;

//typedef struct _AHB_DEV
//{
//	AMBA_DEV_INFO dev_info;
//	UINT32 user_def[3];
//	AMBA_BAR_INFO bar [4];
//	BYTE slot;
//	BOOL is_master;
//	void *handler_data;
//	char dev_name[16];
//} AHB_DEV;

//typedef struct _APB_DEV{
//	AMBA_DEV_INFO dev_info;
//	AMBA_BAR_INFO bar;
//	BYTE slot;
//	void *handler_data;
//	char dev_name[16];
//} APB_DEV;

struct _AMBA_DEV;
typedef void (*HANDLER_DATA_FUNC)(struct _AMBA_DEV *dev);

/**
 * \struct AMBA_DEV
 */
typedef struct _AMBA_DEV{
	AMBA_DEV_INFO      dev_info;
	AMBA_BAR_INFO      bar[4];
	BYTE               slot;
	HANDLER_DATA_FUNC  show_info;
	char               dev_name[16];
	BOOL               is_ahb;
	BOOL               is_master;
	UINT32             user_def[3];
} AMBA_DEV;

static AMBA_DEV *ahbm_devices[AHB_MASTERS_QUANTITY];
static AMBA_DEV *ahbsl_devices[AHB_SLAVES_QUANTITY];
static AMBA_DEV *apb_devices[APB_QUANTITY];

/*
 * pnp_dev must be allocated by caller
 * returns 0 if ok, non-zero otherwise
 */
//int capture_ahbm_dev(AHB_DEV *ahb_dev, BYTE vendor_id, UINT16 device_id);
/*
 * pnp_dev must be allocated by caller
 * returns 0 if ok, non-zero otherwise
 */
//int capture_ahbsl_dev(AHB_DEV *ahb_dev, BYTE vendor_id, UINT16 device_id);

/**
 * Capture amba pnp device.
 * @param apb_dev AMBA_DEV
 * @param vendor_id vendor ID
 * @param device_id device ID
 * @param is_ahb ahb/not ahb
 * @param is_master master/slave
 * @return slot number or -1 if error
 */
int capture_amba_dev(AMBA_DEV *apb_dev, BYTE vendor_id, UINT16 device_id, BOOL is_ahb, BOOL is_master);

/**
 * Fill amba device.
 * @param dev amba device
 * @param slot_number slot number
 * @param is_ahb ahb/not ahb
 * @param is_master master/slave
 */
void fill_amba_dev(AMBA_DEV *dev, BYTE slot_number, BOOL is_ahb, BOOL is_master);

/**
 * Get vendor name of amba pnp device.
 * @param ven_id vendor ID
 */
const char* amba_get_ven_name(BYTE ven_id);

/**
 * Get device name of amba pnp device.
 * @param ven_id vendor ID
 * @param dev_id device ID
 */
const char* amba_get_dev_name(BYTE ven_id, UINT16 dev_id);

#endif /* AMBA_PNP_H_ */
