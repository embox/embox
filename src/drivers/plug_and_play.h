/*
 * plug_and_play.c
 *
 *  Created on: 28.01.2009
 *      Authors: Alexandr Batyukov, Alexey Fomin, Eldar Abusalimov
 */

#ifndef PLUG_AND_PLAY_H_
#define PLUG_AND_PLAY_H_

#define TRY_CAPTURE_AHBM_DEV(dev,venID,devID) if (-1 == capture_ahbm_dev(dev, venID, devID)){\
	printf ("error : can't capture ahbm dev venID=0x%X, devID=0x%X\n", venID, devID);\
	return -1;\
}
#define TRY_CAPTURE_AHBSL_DEV(dev,venID,devID) if (-1 == capture_ahbsl_dev(dev, venID, devID)){\
	printf ("error : can't capture ahbsl dev venID=0x%X, devID=0x%X\n", venID, devID);\
	return -1;\
}

#define TRY_CAPTURE_APB_DEV(dev,venID,devID) if (-1 == capture_apb_dev(dev, venID, devID)){\
	printf ("error : can't capture apb dev venID=0x%X, devID=0x%X\n", venID, devID);\
	return -1;\
}


typedef struct _AMBA_BAR_INFO {
	UINT32 start;
	BOOL prefetchable;
	BOOL cacheable;
	UINT32 mask;
	BYTE type;
	BOOL used;
} AMBA_BAR_INFO;

typedef struct _AMBA_DEV_INFO
{
	BYTE venID;
	UINT16 devID;
	BYTE version;
	BYTE irq;
} AMBA_DEV_INFO;

typedef struct _AHB_DEV
{
	AMBA_DEV_INFO dev_info;
	UINT32 user_def[3];
	AMBA_BAR_INFO bar [4];
	BYTE slot;
	BOOL is_master;
} AHB_DEV;


typedef struct _APB_DEV{
	AMBA_DEV_INFO dev_info;
	AMBA_BAR_INFO bar;
	BYTE slot;
} APB_DEV;

/*
 * pnp_dev must be allocated by caller
 * returns 0 if ok, non-zero otherwise
 */
int capture_ahbm_dev(AHB_DEV *ahb_dev, BYTE vendor_id, UINT16 device_id);
/*
 * pnp_dev must be allocated by caller
 * returns 0 if ok, non-zero otherwise
 */
int capture_ahbsl_dev(AHB_DEV *ahb_dev, BYTE vendor_id, UINT16 device_id);
/*
 * pnp_dev must be allocated by caller
 * returns 0 if ok, non-zero otherwise
 */
int capture_apb_dev(APB_DEV *apb_dev, BYTE vendor_id, UINT16 device_id);

/*
 * Print list of all connected plug and play devices on ahb bus
 */
void print_all_pnp_devs();

#endif /* PLUG_AND_PLAY_H_ */
