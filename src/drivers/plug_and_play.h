#ifndef PLUG_AND_PLAY_H_
#define PLUG_AND_PLAY_H_

#define PNP_MASTER_BASE	0xFFFFF000
#define PNP_SLAVE_BASE	0xFFFFF800

#define COMPUTE_PNP_VENDOR_ID(id_reg)	(0xFF & ((id_reg) >> 24))
#define COMPUTE_PNP_DEVICE_ID(id_reg)	(0xFFF & ((id_reg) >> 12))
#define COMPUTE_PNP_VERSION(id_reg)	(0x1F & ((id_reg)) >> 5)
#define COMPUTE_PNP_IRQ(id_reg)		(0x1F & (id_reg))

#define COMPUTE_PNP_BAR_ADDR(bar)		(0xFFF & ((bar) >> 20))
#define COMPUTE_PNP_BAR_P(bar)			(0x1 & ((bar) >> 17))
#define COMPUTE_PNP_BAR_C(bar)			(0x1 & ((bar) >> 16))
#define COMPUTE_PNP_BAR_MASK(bar)		(0xFFF & ((bar) >> 4))
#define COMPUTE_PNP_BAR_TYPE(bar)		(0xF & (bar))

typedef BYTE PNP_DEVICE_ID;
typedef BYTE PNP_VENDOR_ID;
typedef BYTE PNP_VERSION;
typedef BYTE PNP_IRQ;
typedef WORD PNP_USER_DEFINED;
typedef BYTE BAR_ADDR;
typedef BYTE BAR_MASK;
typedef BYTE BAR_TYPE;

typedef struct {
	BAR_ADDR addr;
	BOOL prefetchable;
	BOOL cacheable;
	BAR_MASK mask;
	BAR_TYPE type;
}BANK_ADDRESS_REG;

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
 * returns NULL if not connected or already determined
 */
PNP_DEV *alloc_ahb_dev(BYTE vendor_id, WORD device_id);

/***
 * Print list of all connected plug and play devices
 */
void print_ahb_dev();

#endif /* PLUG_AND_PLAY_H_ */
