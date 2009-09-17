/**
 * \file lspnp.c
 *
 * \date 20.02.2009
 * \author abatyukov
 */
#include "asm/types.h"
#include "common.h"
#include "string.h"
#include "drivers/amba_pnp.h"
#include "drivers/pnp_id.h"
#include "lspnp.h"
#include "shell_command.h"

#define COMMAND_NAME "lspnp"
#define COMMAND_DESC_MSG "show list of plug and play devices"
static const char *help_msg =
	#include "lspnp_help.inc"
	;
#define HELP_MSG help_msg

DECLARE_SHELL_COMMAND_DESCRIPTOR(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG);


static char available_keys[] = {
	#include "lspnp_keys.inc"
};

//static int bus_type;

static void show_help() {
	printf(
	#include "lspnp_help.inc"
	);
}


#define AMBA_BT_AHBM  1
#define AMBA_BT_AHBSL 2
#define AMBA_BT_APB   3
#define AMBA_BT_ALL   0

//-------------Utils---------------------------------------------------


#define REGISTER_DEVICES(dev_table) dev_table, array_len(dev_table)
/**
 * \struct PNP_DEVICE_INFO
 */
typedef struct _PNP_DEVICE_INFO {
	const BYTE vendor_id;
	const UINT16 device_id;
	const char *name;
} PNP_DEVICE_INFO;

PNP_DEVICE_INFO gaisler_pnp_devices_table [] = {
	#include "gaisler_pnp_devices_table.inc"
};
PNP_DEVICE_INFO esa_pnp_devices_table [] = {
	#include "esa_pnp_devices_table.inc"
};

/**
 * \struct PNP_VENDOR_INFO
 */
typedef struct _PNP_VENDOR_INFO {
	const BYTE vendor_id;
	const char *name;
	PNP_DEVICE_INFO *dev_table ;
	int size;
} PNP_VENDOR_INFO;


static PNP_VENDOR_INFO const vendors_table[] = {
	#include "pnp_vendors_table.inc"
};

static PNP_DEVICE_INFO const devs_table[] = {
	#include "gaisler_pnp_devices_table.inc"
	,
	#include "esa_pnp_devices_table.inc"
};

#define VENDORS_TABLE_LEN        array_len(vendors_table)
#define DEVICES_TABLE_LEN        array_len(devs_table)

/**
 * Get vendor name of amba pnp device.
 * @param ven_id vendor ID
 */
inline static const char* amba_get_ven_name(BYTE ven_id) {
	LOG_DEBUG("amba_get_ven_name: ven_id=0x%X\n", ven_id);
	int i;
	for (i = 0; i < VENDORS_TABLE_LEN; i++) {
		if (vendors_table[i].vendor_id == ven_id) {
			return vendors_table[i].name;
		}
	}
	return NULL;
}
/**
 * Get device name of amba pnp device.
 * @param ven_id vendor ID
 * @param dev_id device ID
 */
inline static const char* amba_get_dev_name(BYTE ven_id, UINT16 dev_id) {
	LOG_DEBUG("amba_get_dev_name: ven_id=0x%X, dev_id=0x%X\n", ven_id, dev_id);
	int i;
	for (i = 0; i < DEVICES_TABLE_LEN; i++) {
		if ((devs_table[i].vendor_id == ven_id) &&
		    (devs_table[i].device_id == dev_id)) {
			return devs_table[i].name;
		}
	}
	return NULL;
}

inline static void print_table_row(int n, int ven_id, int dev_id,
                const char *ven_name, const char *dev_name, int ver, int irq) {
        TRACE("%02x.%02x:%03x %20s %28s \t0x%02x %3d\n", n, ven_id, dev_id, ven_name, dev_name, ver, irq);
}

inline static void print_table_head() {
        TRACE("\n  %7s %20s %28s \t%4s %3s\n", "ven:dev", "Vendor Name", "Device Name","ver", "irq");
}

inline static void show_bars_type(AMBA_BAR_INFO *bar) {
        switch (bar->type) {
        case 1:
        	TRACE("\tapb:");
        case 2:
                TRACE("\tahb:");
    		break;
        default:
        	TRACE("\t%X", bar->type);
        }
}

inline static void show_bar_info(AMBA_BAR_INFO *bar) {
        if (bar->used) {
                show_bars_type(bar);
                TRACE("%X\n", bar->start);
        }
}

void show_bars_infos(AMBA_DEV *dev) {
        int i;
        for (i = 0; i < array_len(dev->bar); i++) {
                if (dev->bar[i].used) {
                        show_bars_type(&dev->bar[i]);
                        TRACE("%X\n", dev->bar[i].start);
                }
        }
}

const char UNKNOWN[] = "unknown";

static void show_dev(AMBA_DEV *dev, BOOL show_user) {
	LOG_DEBUG("show_dev\n");
        HANDLER_DATA_FUNC func;
        if (NULL == dev) return;

        char *ven_name;
        char *dev_name;

        if ((!show_user)||(NULL == dev->show_info )) {
                //standard out
                if (NULL != (ven_name = (char *) amba_get_ven_name(dev->dev_info.venID))) {
                        if (NULL == (dev_name = (char *) amba_get_dev_name(dev->dev_info.venID,
                                        dev->dev_info.devID))) {
                                dev_name = (char *) UNKNOWN;
                        }
                } else {
                        ven_name = (char *) UNKNOWN;
                        dev_name = (char *) UNKNOWN;
                }
                print_table_row(dev->slot, dev->dev_info.venID, dev->dev_info.devID, ven_name,
                                dev_name, dev->dev_info.version, dev->dev_info.irq);
                show_bar_info(dev->bar);
                return;
        }
        //all info out using handler
        dev->show_info(dev);
        return;
}

static int print_apb_entries(int amount) {
	LOG_DEBUG("print_apb_entries: %d\n", amount);
        int i, count = 0;
        AMBA_DEV dev;
        //APB_SLOT *pslot = base_addr;
        for (i = 0; i < amount/4; i++) {
                //if (0 != pslot[i].id_reg) {
                if(-1 != fill_amba_dev(&dev, i, FALSE, FALSE)){
                        show_dev(&dev, FALSE);
                        count++;
                }
        }
        return count;
}

//static int print_ahb_entries(AHB_SLOT *base_addr, int amount,BOOL is_master) {
static int print_ahb_entries(int amount, BOOL is_master) {
	LOG_DEBUG("print_ahb_entries: %d\n", amount);
        int i, count = 0;
        AMBA_DEV dev;
        //AHB_SLOT *pslot = base_addr;
        for (i = 0; i < amount; i++) {
                //if (0 != pslot[i].id_reg) {
                if(-1 != fill_amba_dev(&dev,  i, TRUE, is_master)){
                	show_dev(&dev, FALSE);
                	count++;
                }
        }
        return count;
}

/**
 * Print list of all connected plug and play devices on ahb master bus
 */
int print_ahbm_pnp_devs() {
	LOG_DEBUG("print_ahbm_pnp_devs\n");
        int count = 0;
        TRACE("\nAHB masters..\n");
        print_table_head();
        count += print_ahb_entries(AHB_MASTERS_QUANTITY, TRUE);
        return count;
}

/**
 * Print list of all connected plug and play devices on ahb slave bus
 */
int print_ahbsl_pnp_devs() {
	LOG_DEBUG("print_ahbsl_pnp_devs\n");
        int count = 0;
        TRACE("\nAHB slaves..\n");
        print_table_head();
        count += print_ahb_entries(AHB_SLAVES_QUANTITY, FALSE);
	return count;
}

/**
 * Print list of all connected plug and play devices on apb bus
 */
int print_apb_pnp_devs() {
	LOG_DEBUG("print_apb_pnp_devs\n");
        int count = 0;
        TRACE("\nAPB slaves..\n");
        print_table_head();
        count += print_apb_entries(APB_QUANTITY);
        return count;
}

/**
 * Print list of all connected plug and play devices on ahb && apb buses
 */
void print_all_pnp_devs() {
	LOG_DEBUG("print_all_pnp_devs\n");
#ifdef RELEASE
        return;
#else
        int count = 0;
        TRACE("\nListing AMBA PNP devices..\n");
        count +=print_ahbm_pnp_devs();
        count +=print_ahbsl_pnp_devs();
        TRACE("\n..Total: %d\n\n", count);
        count = 0;
	count += print_apb_pnp_devs();
        TRACE("\n..Total: %d\n\n", count);
#endif
}

void print_ahbm_pnp_dev(UINT32 slot) {
	LOG_DEBUG("print_ahbm_pnp_dev: %d\n", slot);
        if (slot >AHB_MASTERS_QUANTITY) {
    		LOG_ERROR("print_ahbm_pnp_dev: Too big arg. The quantity of AHB masters is %d\n",AHB_MASTERS_QUANTITY);
    		return;
        }

        if (NULL != ahbm_devices[slot]) {
                show_dev(ahbm_devices[slot], TRUE);
                return;
        }
        AMBA_DEV dev;
        //dev.handler_data = NULL;

        //AHB_SLOT *pslot = (AHB_SLOT *)AHB_MASTER_BASE;
        //if (0 != pslot[slot].id_reg) {
        if(-1 != fill_amba_dev(&dev, slot, TRUE, TRUE)){
                show_dev(&dev, FALSE);
        } else {
                LOG_ERROR("No such device.\n");
        }
        //print_ahb_entries((AHB_SLOT *)AHB_MASTER_BASE, 1, TRUE);
}

void print_ahbsl_pnp_dev(UINT32 slot) {
	LOG_DEBUG("print_ahbsl_pnp_dev: %d\n", slot);
        if (slot >AHB_SLAVES_QUANTITY) {
                LOG_ERROR("print_ahbsl_pnp_dev: Too big arg. The quantity of AHB slaves is %d\n",AHB_SLAVES_QUANTITY);
                return;
        }
        if (NULL != ahbsl_devices[slot]) {
                show_dev(ahbsl_devices[slot], TRUE);
                return;
        }
        AMBA_DEV dev;
        //AHB_SLOT *pslot = (AHB_SLOT *)AHB_SLAVE_BASE;
        //if (0 != pslot[slot].id_reg) {
        if (-1 != fill_amba_dev(&dev, slot, TRUE, FALSE)){
                show_dev(&dev, FALSE);
        } else {
                LOG_ERROR("No such device.\n");
        }
}

void print_apb_pnp_dev(UINT32 slot) {
	LOG_DEBUG("print_apb_pnp_dev: %d\n", slot);
        if (slot >APB_QUANTITY) {
                LOG_ERROR("print_apb_pnp_dev: Too big arg. The quantity of APB devices is %d\n",APB_QUANTITY);
                return;
        }
        if (NULL != apb_devices[slot]) {
                show_dev(apb_devices[slot], TRUE);
                return;
        }
        AMBA_DEV dev;
        //APB_SLOT *pslot = (APB_SLOT *)APB_BASE;
        //if (0 != pslot[slot].id_reg) {

        if (-1 != fill_amba_dev(&dev, slot, FALSE, FALSE)){
                show_dev(&dev, FALSE);
        } else {
                LOG_ERROR("No such device.\n");
        }
}
//-------------------------------------------------------------------------

typedef int (*FUNC_SHOW_BUS)(int dev_num);

static int show_ahbm(int dev_num){
	if(dev_num < 0) {
		print_ahbm_pnp_devs();
		return 0;
	}
	print_ahbm_pnp_dev(dev_num);
	return 0;
}

static int show_ahbsl(int dev_num){
	if(dev_num < 0) {
		print_ahbsl_pnp_devs();
		return 0;
	}
	print_ahbsl_pnp_dev(dev_num);
	return 0;
}

static int show_apb(int dev_num){
	if (dev_num < 0) {
		print_apb_pnp_devs();
		return 0;
	}
	print_apb_pnp_dev(dev_num);
	return 0;
}

static int show_all(int dev_num){
	show_ahbm(-1);
	show_ahbsl(-1);
	show_apb(-1);
}

static FUNC_SHOW_BUS set_bus_type(const char *key_value) {
	if (0==strcmp(key_value, "ahbm")) {
		return show_ahbm;
	} else if (0==strcmp(key_value, "ahbsl")) {
		return  show_ahbsl;
	} else if (0==strcmp(key_value, "apb")) {
		return  show_apb;
	} else if (0==strcmp(key_value, "all")) {
		return  show_all;
	}
	return NULL;
}

static int exec(int argsc, char **argsv) {
	SHELL_KEY keys[MAX_SHELL_KEYS];
	char *key_value;
	int keys_amount;
	int dev_number = -1;

	FUNC_SHOW_BUS show_func;

	keys_amount = parse_arg(COMMAND_NAME, argsc, argsv, available_keys,
				sizeof(available_keys), keys);

	if (keys_amount < 0) {
		show_help();
		return -1;
	}

	if (get_key('h', keys, keys_amount, &key_value)) {
		show_help();
		return 0;
	}

	if (!get_key('b', keys, keys_amount, &key_value)) {
		key_value = "all";
	}
	if (NULL == (show_func = set_bus_type(key_value))){
		LOG_ERROR("Parsing: chose right bus type '-b'\n");
		show_help();
		return -1;
	}

	if (get_key('n', keys, keys_amount, &key_value)) {
		if (show_all == show_func) {
			LOG_ERROR("Parsing: chose bus type '-b'\n");
			show_help();
			return -1;
		}
		if (1!=sscanf(key_value,"%d", &dev_number)) {
			LOG_ERROR("parsing: enter validly dev_number '-b'\n");
			show_help();
			return -1;
		}
	}

	show_func(dev_number);

	return 0;
}
