/**
 * @file
 * @brief Implements net stack process loading.
 *
 * @date 04.02.10
 * @author Anton Bondarev
 */

#include <net/netdevice.h>
#include <embox/unit.h>
#include <linux/init.h>
#include <net/icmp.h>
#include <embox/net.h>

EMBOX_UNIT_INIT(unit_init);

static int __init unit_init(void) {
	/* Initializaton of icmp socket */
	icmp_init();
	return 0;
}
