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

EMBOX_UNIT_INIT(unit_init);

static int __init unit_init(void) {
	extern packet_type_t *__ipstack_packets_start, *__ipstack_packets_end;
	packet_type_t ** p_netpack = &__ipstack_packets_start;

	for (; p_netpack < &__ipstack_packets_end; p_netpack++) {
		if ((NULL == (*p_netpack)) || (NULL == (*p_netpack)->init)) {
			TRACE ("Wrong packet descriptor\n");
			continue;
		}
		TRACE ("Adding packet type 0x%03X..", (*p_netpack)->type);

		if (-1 == (*p_netpack)->init()) {
			TRACE ("NO\n");
		} else {
			dev_add_pack((*p_netpack));
			TRACE ("YES\n");
		}
	}
	return 0;
}
