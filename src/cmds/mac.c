/**
 *  @file mac.c
 *  @brief show physical address net device
 *  @date 01.07.11
 *  @author: Gleb Efimov
 *  @author: Ilia Vaprol
 */

#include <types.h>
#include <stdio.h>
#include <net/netdevice.h>
#include <embox/cmd.h>
#include <asm/io.h>
#include <net/checksum.h>

EMBOX_CMD(exec);

static int exec(int argv, char **argc) {
	uint8_t i;
	struct net_device *dev;

	dev = netdev_get_by_name("eth0");
	if (NULL == dev) {
		printf("Couldn't find NE2K_PCI device");
		return 1;
	}
	/* Get MAC-Address */
	printf("The current MAC stations address is ");
	for (i = 0; i < dev->addr_len - 1; i++) {
		printf("%2X:", dev->dev_addr[i]);
	}
	printf("%2X.\n", dev->dev_addr[i]);
	return 0;
}
