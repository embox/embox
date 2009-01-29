/*
 * test_availability_irq.c
 *
 *  Created on: 28.01.2009
 *      Authors: Alexandr Batyukov, Alexey Fomin, Eldar Abusalimov
 */

#ifndef PLUG_AND_PLAY_H_
#define PLUG_AND_PLAY_H_

#define PNP_MASTER_BASE	0xFFFFF000
#define PNP_SLAVE_BASE	0xFFFFF800

typedef BYTE PNP_VENDOR_ID;
typedef WORD PNP_DEVICE_ID;
typedef BYTE PNP_VERSION;
typedef BYTE PNP_IRQ;
typedef WORD PNP_USER_DEFINED;

typedef WORD BAR_ADDR;
typedef WORD BAR_MASK;
typedef BYTE BAR_TYPE;

typedef struct {
	BAR_ADDR addr;
	BOOL prefetchable;
	BOOL cacheable;
	BAR_MASK mask;
	BAR_TYPE type;
} BANK_ADDRESS_REG;

typedef struct {
	PNP_VENDOR_ID vendor_id;
	PNP_DEVICE_ID device_id;
	PNP_VERSION version;
	PNP_IRQ irq;
} ID_REG;

typedef struct {
	ID_REG id_reg;
	PNP_USER_DEFINED user_defined[3];
	BANK_ADDRESS_REG bar[4];
} PNP_DEV;

BOOL pnp_dev_not_available[128];

/***
 * Determine connected device
 *
 * pnp_dev must be allocated by caller
 * returns 0 if ok, non-zero otherwise
 */
int capture_ahb_dev(PNP_DEV *pnp_dev, BYTE vendor_id, WORD device_id);

/***
 * Print list of all connected plug and play devices
 */
void print_ahb_dev();

#endif /* PLUG_AND_PLAY_H_ */
