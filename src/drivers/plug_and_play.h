/*
 * plug_and_play.c
 *
 *  Created on: 28.01.2009
 *      Authors: Alexandr Batyukov, Alexey Fomin, Eldar Abusalimov
 */

#ifndef PLUG_AND_PLAY_H_
#define PLUG_AND_PLAY_H_

#define TRY_CAPTURE_AHBM_DEV(dev,venID,devID) if (-1 == capture_amba_dev(dev, venID, devID, TRUE, TRUE)){\
	printf ("error : can't capture ahbm dev venID=0x%X, devID=0x%X\n", venID, devID);\
	return -1;\
}
#define TRY_CAPTURE_AHBSL_DEV(dev,venID,devID) if (-1 == capture_amba_dev(dev, venID, devID, TRUE, FALSE)){\
	printf ("error : can't capture ahbsl dev venID=0x%X, devID=0x%X\n", venID, devID);\
	return -1;\
}

#define TRY_CAPTURE_APB_DEV(dev,venID,devID) if (-1 == capture_amba_dev(dev, venID, devID, FALSE, FALSE)){\
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
//
//
//typedef struct _APB_DEV{
//	AMBA_DEV_INFO dev_info;
//	AMBA_BAR_INFO bar;
//	BYTE slot;
//	void *handler_data;
//	char dev_name[16];
//} APB_DEV;

typedef struct _AMBA_DEV{
	AMBA_DEV_INFO dev_info;
	AMBA_BAR_INFO  bar[4];
	BYTE slot;
	void *handler_data;
	char dev_name[16];

	BOOL is_ahb;

	BOOL is_master;
	UINT32 user_def[3];
} AMBA_DEV;

typedef void (*HANDLER_DATA_FUNC)(AMBA_DEV *apb_dev);

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
/*
 * pnp_dev must be allocated by caller
 * returns 0 if ok, non-zero otherwise
 */
int capture_amba_dev(AMBA_DEV *apb_dev, BYTE vendor_id, UINT16 device_id, BOOL is_ahb, BOOL is_master);

/*
 * Print list of all connected plug and play devices on ahb && apb buses
 */
void print_all_pnp_devs();

//Print list of all connected plug and play devices on ahb master bus
int print_ahbm_pnp_devs();


//Print list of all connected plug and play devices on ahb slave bus
int print_ahbsl_pnp_devs();

//Print list of all connected plug and play devices on apb bus
int print_apb_pnp_devs();

#endif /* PLUG_AND_PLAY_H_ */
