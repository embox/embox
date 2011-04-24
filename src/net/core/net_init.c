/**
 * @file
 * @brief Implements net stack process loading.
 *
 * @date 04.02.2010
 * @author Anton Bondarev
 */

#include <net/netdevice.h>
#include <embox/unit.h>
#include <linux/init.h>
#include <net/icmp.h>

EMBOX_UNIT_INIT(unit_init);

static int __init unit_init(void) {
	extern packet_type_t *__ipstack_packets_start, *__ipstack_packets_end;
	packet_type_t ** p_netpack = &__ipstack_packets_start;

	TRACE("\n");
	for (; p_netpack < &__ipstack_packets_end; p_netpack++) {
		if ((NULL == (*p_netpack)) || (NULL == (*p_netpack)->init)) {
			TRACE ("\nWrong packet descriptor\n");
			continue;
		}
		TRACE ("\nAdding packet type 0x%03X - %s:\n", (*p_netpack)->type,
				trace_proto_pack_info((*p_netpack)->type));

		if (-1 == (*p_netpack)->init()) {
			TRACE ("NO\n");
		} else {
			dev_add_pack((*p_netpack));
			TRACE ("YES\n");
		}
	}
	/* Initializaton of icmp socket */
	icmp_init();
	TRACE("\n");
	return 0;
}
