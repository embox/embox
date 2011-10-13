/**
 *  @file
 *  @brief show info about NE2000 PCI
 *  @date 01.07.11
 *  @author Gleb Efimov
 *  @author Ilia Vaprol
 */

#include <types.h>
#include <stdio.h>
#include <net/netdevice.h>
#include <embox/cmd.h>
#include <asm/io.h>
#include <net/checksum.h>
#include <getopt.h>
#include <net/ne2k_pci.h>

EMBOX_CMD(exec);

static void show_mac(struct net_device *dev) {
	uint8_t i;

	printf("The current stations MAC address is ");
	for (i = 0; i < dev->addr_len - 1; i++) {
		printf("%2X:", dev->dev_addr[i]);
	}
	printf("%2X.\n", dev->dev_addr[i]);
}

static void show_page(unsigned long base_addr) {
	uint8_t i, page, val;
	/* Page Dump*/
	printf("\n       ");
	for (i = 0; i < 16; i++) {
    		printf(" 0%X", i);
	}
	for (page = 0; page < 4; page++) {
		printf("\npage %d:", page);
		out8(E8390_NODMA | (page << 6), base_addr + E8390_CMD);
		for(i = 0; i < 16; i++) {
			val = in8(base_addr + i);
			if (!val) {
				printf(" ..");
			} else if (val < 0x10) {
				printf(" 0%X", val);
			} else {
				printf(" %X", val);
			}
		}
	}
	printf("\n");
}

static int exec(int argc, char **argv) {
	int opt;
	struct net_device *dev;
	uint8_t page, full;

	dev = netdev_get_by_name("eth0");
	if (dev == NULL) {
		printf("Couldn't find NE2K_PCI device");
		return -1;
	}

	page = full = 0;
	getopt_init();
	do {
		opt = getopt(argc, argv, "pf");
		switch(opt) {
		case 'p':
			page = 1;
			break;
		case 'f':
			full = 1;
			break;
		case -1:
			break;
		default:
			printf("Usage: %s [-fp]\n", *argv);
			return 0;
		}
	} while (opt != -1);

	if (full) {
		show_mac(dev);
	 	show_page(dev->base_addr);
	} else if (page) {
	 	show_page(dev->base_addr);
	} else {
		show_mac(dev);
	}

	return 0;
}
