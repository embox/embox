/**
 * @file
 * @brief show list of plug and play devices
 *
 * @date 20.02.2009
 * @author Alexandr Batyukov
 */
#include <shell_command.h>
#include <string.h>
#include <drivers/amba_pnp.h>
#include <drivers/amba_registry.h>

#define COMMAND_NAME     "lspnp"
#define COMMAND_DESC_MSG "show list of plug and play devices"
#define HELP_MSG         "Usage: lspnp [-b bus_type] [-n dev_id] [-h]"
static const char *man_page =
	#include "lspnp_help.inc"
	;

DECLARE_SHELL_COMMAND(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG, man_page);

//static int bus_type;

#define AMBA_BT_AHBM  1
#define AMBA_BT_AHBSL 2
#define AMBA_BT_APB   3
#define AMBA_BT_ALL   0

//-------------Utils---------------------------------------------------


inline static void print_table_row(int n, int ven_id, int dev_id,
		const char *ven_name, const char *dev_name, int ver, int irq) {
	TRACE("%02x.%02x:%03x %20s %28s \t0x%02x %3d\n",
			n, ven_id, dev_id, ven_name, dev_name, ver, irq);
}

inline static void print_table_head(void) {
	TRACE("\n  %7s %20s %28s \t%4s %3s\n", "ven:dev",
			"Vendor Name", "Device Name","ver", "irq");
}

inline static void show_bars_type(AMBA_BAR_INFO *bar) {
	switch (bar->type) {
	case 1:
		TRACE("\tapb:");
		break;
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

const char UNKNOWN[] = "<unknown>";

static void show_dev(AMBA_DEV *dev, bool show_user) {

	char *ven_name, *dev_name;
	if (NULL == dev) return;

	if ((!show_user)||(NULL == dev->show_info )) {
		ven_name = amba_registry_get_ven_name(dev->dev_info.venID);
		dev_name = amba_registry_get_dev_name(dev->dev_info.venID, dev->dev_info.devID);
		print_table_row(dev->slot, dev->dev_info.venID, dev->dev_info.devID, ven_name,
				dev_name, dev->dev_info.version, dev->dev_info.irq);
		show_bar_info(dev->bar);
		return;
	}
	/* all info out using handler */
	dev->show_info(dev);
	return;
}

static int print_apb_entries(int amount) {
	int i, count = 0;
	AMBA_DEV dev;

	for (i = 0; i < amount/4; i++) {
		if(-1 != fill_amba_dev(&dev, i, false, false)){
			show_dev(&dev, false);
			count++;
		}
	}
	return count;
}

static int print_ahb_entries(int amount, bool is_master) {
	int i, count = 0;
	AMBA_DEV dev;

	for (i = 0; i < amount; i++) {

		if(-1 != fill_amba_dev(&dev,  i, true, is_master)){
			show_dev(&dev, false);
			count++;
		}
	}
	return count;
}

/**
 * Print list of all connected plug and play devices on ahb master bus
 */
int print_ahbm_pnp_devs(void) {
	int count = 0;
	TRACE("\nAHB masters..\n");
	print_table_head();
	count += print_ahb_entries(AHB_MASTERS_QUANTITY, true);
	return count;
}

/**
 * Print list of all connected plug and play devices on ahb slave bus
 */
int print_ahbsl_pnp_devs(void) {
	int count = 0;
	TRACE("\nAHB slaves..\n");
	print_table_head();
	count += print_ahb_entries(AHB_SLAVES_QUANTITY, false);
	return count;
}

/**
 * Print list of all connected plug and play devices on apb bus
 */
int print_apb_pnp_devs(void) {
	int count = 0;
	TRACE("\nAPB slaves..\n");
	print_table_head();
	count += print_apb_entries(APB_QUANTITY);
	return count;
}

/**
 * Print list of all connected plug and play devices on ahb && apb buses
 */
void print_all_pnp_devs(void) {
	int count = 0;
	TRACE("\nListing AMBA PNP devices..\n");
	count +=print_ahbm_pnp_devs();
	count +=print_ahbsl_pnp_devs();
	TRACE("\n..Total: %d\n\n", count);
	count = 0;
	count += print_apb_pnp_devs();
	TRACE("\n..Total: %d\n\n", count);
}

void print_ahbm_pnp_dev(uint32_t slot) {
	AMBA_DEV dev;
	if (slot >AHB_MASTERS_QUANTITY) {
    		LOG_ERROR("print_ahbm_pnp_dev: Too big arg. The quantity of AHB masters is %d\n",AHB_MASTERS_QUANTITY);
    		return;
	}

	if (NULL != ahbm_devices[slot]) {
		show_dev(ahbm_devices[slot], true);
		return;
	}
	if(-1 != fill_amba_dev(&dev, slot, true, true)){
		show_dev(&dev, false);
	} else {
		LOG_ERROR("No such device.\n");
	}
}

void print_ahbsl_pnp_dev(uint32_t slot) {
	AMBA_DEV dev;
	if (slot >AHB_SLAVES_QUANTITY) {
		LOG_ERROR("print_ahbsl_pnp_dev: Too big arg. The quantity of AHB slaves is %d\n",AHB_SLAVES_QUANTITY);
		return;
	}
	if (NULL != ahbsl_devices[slot]) {
		show_dev(ahbsl_devices[slot], true);
		return;
	}

	if (-1 != fill_amba_dev(&dev, slot, true, false)){
		show_dev(&dev, false);
	} else {
		LOG_ERROR("No such device.\n");
	}
}

void print_apb_pnp_dev(uint32_t slot) {
	AMBA_DEV dev;
	if (slot > APB_QUANTITY) {
		TRACE("print_apb_pnp_dev: Too big arg. The quantity of APB devices is %d\n",APB_QUANTITY);
		return;
	}
	if (NULL != apb_devices[slot]) {
		show_dev(apb_devices[slot], true);
		return;
	}

	if (-1 != fill_amba_dev(&dev, slot, false, false)){
		show_dev(&dev, false);
	} else {
		TRACE("No such device.\n");
	}
}
/*-------------------------------------------------------------------------*/

typedef void (*func_show_bus_t)(int dev_num);

static void show_ahbm(int dev_num){
	if(dev_num < 0) {
		print_ahbm_pnp_devs();
	}
	return;
	print_ahbm_pnp_dev(dev_num);
}

static void show_ahbsl(int dev_num){
	if(dev_num < 0) {
		print_ahbsl_pnp_devs();
		return;
	}
	print_ahbsl_pnp_dev(dev_num);
}

static void show_apb(int dev_num){
	if (dev_num < 0) {
		print_apb_pnp_devs();
		return;
	}
	print_apb_pnp_dev(dev_num);
}

static void show_all(int dev_num){
	show_ahbm(-1);
	show_ahbsl(-1);
	show_apb(-1);
}

static func_show_bus_t set_bus_type(const char *key_value) {
	if (0 == strncmp(key_value, "ahbm", 5)) {
		return show_ahbm;
	} else if (0 == strncmp(key_value, "ahbsl", 5)) {
		return  show_ahbsl;
	} else if (0 == strncmp(key_value, "apb", 5)) {
		return  show_apb;
	} else if (0 == strncmp(key_value, "all", 5)) {
		return  show_all;
	}
	return NULL;
}


static int exec(int argsc, char **argsv) {
	int dev_number = -1;
	func_show_bus_t show_func = show_all;
	int nextOption;
	getopt_init();
	do {
		nextOption = getopt(argsc, argsv, "n:b:h");
		switch(nextOption) {
		case 'h':
			show_help();
			return 0;
		case 'b':
			if (NULL == (show_func = set_bus_type(optarg))) {
				LOG_ERROR("Parsing: chose right bus type '-b'\n");
				show_help();
				return -1;
			}
			break;
		case 'n':
			if (show_all == show_func) {
				LOG_ERROR("Parsing: chose bus type '-b'\n");
				show_help();
				return -1;
			}
			if (1 != sscanf(optarg,"%d", &dev_number)) {
				LOG_ERROR("parsing: enter validly dev_number '-n'\n");
				show_help();
				return -1;
			}
			break;
		case -1:
			break;
		default:
			return 0;
		}
	} while(-1 != nextOption);

	show_func(dev_number);
	return 0;
}
