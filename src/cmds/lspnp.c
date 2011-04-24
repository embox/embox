/**
 * @file
 * @brief show list of plug and play devices
 *
 * @date 20.02.09
 * @author Alexandr Batyukov
 */

#include <embox/cmd.h>

#include <getopt.h>
#include <string.h>
#include <stdio.h>

#include <drivers/amba_pnp.h>
#include <drivers/amba_registry.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: lspnp [-b bus_type] [-n dev_id] [-h]\n");
}

//static int bus_type;

#define AMBA_BT_AHBM  1
#define AMBA_BT_AHBSL 2
#define AMBA_BT_APB   3
#define AMBA_BT_ALL   0

//-------------Utils---------------------------------------------------


static inline void print_table_row(int n, int ven_id, int dev_id,
		const char *ven_name, const char *dev_name, int ver, int irq) {
	printf("%02x.%02x:%03x %20s %28s \t0x%02x %3d\n",
			n, ven_id, dev_id, ven_name, dev_name, ver, irq);
}

static inline void print_table_head(void) {
	printf("\n  %7s %20s %28s \t%4s %3s\n", "ven:dev",
			"Vendor Name", "Device Name","ver", "irq");
}

static inline void show_bars_type(amba_bar_info_t *bar) {
	switch (bar->type) {
	case 1:
		printf("\tapb:");
		break;
	case 2:
		printf("\tahb:");
		break;
	default:
		printf("\t%X", bar->type);
	}
}

static inline void show_bar_info(amba_bar_info_t *bar) {
	if (bar->used) {
		show_bars_type(bar);
		printf("%X\n", bar->start);
	}
}
#if 0
static void show_bars_infos(amba_dev_t *dev) {
	int i;
	for (i = 0; i < ARRAY_SIZE(dev->bar); i++) {
		if (dev->bar[i].used) {
			show_bars_type(&dev->bar[i]);
			printf("%X\n", dev->bar[i].start);
		}
	}
}
#endif
const char UNKNOWN[] = "<unknown>";

static void show_dev(amba_dev_t *dev, bool show_user) {

	char *ven_name, *dev_name;
	if (NULL == dev) return;

	if ((!show_user) || (NULL == dev->show_info)) {
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
	amba_dev_t dev;

	for (i = 0; i < amount / 4; i++) {
		if (-1 != fill_amba_dev(&dev, i, false, false)) {
			show_dev(&dev, false);
			count++;
		}
	}
	return count;
}

static int print_ahb_entries(int amount, bool is_master) {
	int i, count = 0;
	amba_dev_t dev;

	for (i = 0; i < amount; i++) {

		if (-1 != fill_amba_dev(&dev,  i, true, is_master)) {
			show_dev(&dev, false);
			count++;
		}
	}
	return count;
}

/**
 * Print list of all connected plug and play devices on ahb master bus
 */
static int print_ahbm_pnp_devs(void) {
	int count = 0;
	printf("\nAHB masters..\n");
	print_table_head();
	count += print_ahb_entries(AHB_MASTERS_QUANTITY, true);
	return count;
}

/**
 * Print list of all connected plug and play devices on ahb slave bus
 */
static int print_ahbsl_pnp_devs(void) {
	int count = 0;
	printf("\nAHB slaves..\n");
	print_table_head();
	count += print_ahb_entries(AHB_SLAVES_QUANTITY, false);
	return count;
}

/**
 * Print list of all connected plug and play devices on apb bus
 */
static int print_apb_pnp_devs(void) {
	int count = 0;
	printf("\nAPB slaves..\n");
	print_table_head();
	count += print_apb_entries(APB_QUANTITY);
	return count;
}
#if 0
/**
 * Print list of all connected plug and play devices on ahb && apb buses
 */
static void print_all_pnp_devs(void) {
	int count = 0;
	printf("\nListing AMBA PNP devices..\n");
	count +=print_ahbm_pnp_devs();
	count +=print_ahbsl_pnp_devs();
	printf("\n..Total: %d\n\n", count);
	count = 0;
	count += print_apb_pnp_devs();
	printf("\n..Total: %d\n\n", count);
}
#endif
static void print_ahbm_pnp_dev(uint32_t slot) {
	amba_dev_t dev;
	if (slot >AHB_MASTERS_QUANTITY) {
    		LOG_ERROR("print_ahbm_pnp_dev: Too big arg. The quantity of AHB masters is %d\n",AHB_MASTERS_QUANTITY);
    		return;
	}

	if (NULL != ahbm_devices[slot]) {
		show_dev(ahbm_devices[slot], true);
		return;
	}
	if (-1 != fill_amba_dev(&dev, slot, true, true)) {
		show_dev(&dev, false);
	} else {
		LOG_ERROR("No such device.\n");
	}
}

static void print_ahbsl_pnp_dev(uint32_t slot) {
	amba_dev_t dev;
	if (slot >AHB_SLAVES_QUANTITY) {
		LOG_ERROR("print_ahbsl_pnp_dev: Too big arg. The quantity of AHB slaves is %d\n",AHB_SLAVES_QUANTITY);
		return;
	}
	if (NULL != ahbsl_devices[slot]) {
		show_dev(ahbsl_devices[slot], true);
		return;
	}

	if (-1 != fill_amba_dev(&dev, slot, true, false)) {
		show_dev(&dev, false);
	} else {
		LOG_ERROR("No such device.\n");
	}
}

static void print_apb_pnp_dev(uint32_t slot) {
	amba_dev_t dev;
	if (slot > APB_QUANTITY) {
		printf("print_apb_pnp_dev: Too big arg. The quantity of APB devices is %d\n",APB_QUANTITY);
		return;
	}
	if (NULL != apb_devices[slot]) {
		show_dev(apb_devices[slot], true);
		return;
	}

	if (-1 != fill_amba_dev(&dev, slot, false, false)) {
		show_dev(&dev, false);
	} else {
		printf("No such device.\n");
	}
}
/*-------------------------------------------------------------------------*/

typedef void (*func_show_bus_t)(int dev_num);

static void show_ahbm(int dev_num) {
	if (dev_num < 0) {
		print_ahbm_pnp_devs();
	}
	return; /* WTF? */
	print_ahbm_pnp_dev(dev_num);
}

static void show_ahbsl(int dev_num) {
	if (dev_num < 0) {
		print_ahbsl_pnp_devs();
		return;
	}
	print_ahbsl_pnp_dev(dev_num);
}

static void show_apb(int dev_num) {
	if (dev_num < 0) {
		print_apb_pnp_devs();
		return;
	}
	print_apb_pnp_dev(dev_num);
}

static void show_all(int dev_num) {
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

static int exec(int argc, char **argv) {
	int dev_number = -1;
	func_show_bus_t show_func = show_all;
	int opt;
	getopt_init();
	do {
		opt = getopt(argc, argv, "n:b:h");
		switch(opt) {
		case 'h':
			print_usage();
			return 0;
		case 'b':
			if (NULL == (show_func = set_bus_type(optarg))) {
				LOG_ERROR("Parsing: chose right bus type '-b'\n");
				print_usage();
				return -1;
			}
			break;
		case 'n':
			if (show_all == show_func) {
				LOG_ERROR("Parsing: chose bus type '-b'\n");
				print_usage();
				return -1;
			}
			if (1 != sscanf(optarg,"%d", &dev_number)) {
				LOG_ERROR("parsing: enter validly dev_number '-n'\n");
				print_usage();
				return -1;
			}
			break;
		case -1:
			break;
		default:
			return 0;
		}
	} while (-1 != opt);

	show_func(dev_number);
	return 0;
}
