/**
 * @file
 *
 * @date 07.03.2009
 * @author Anton Bondarev
 */

#include <net/netdevice.h>
#include <net/net_pack_manager.h>
#include <lib/list.h>
#include <linux/init.h>
#include <linux/spinlock.h>

typedef struct net_buff_info {
	/*it must be first member! We use casting in net_buff_free function*/
	unsigned char buff[CONFIG_ETHERNET_V2_FRAME_SIZE];
	struct list_head list;
} net_buff_info_t;

static LIST_HEAD(head_free_pack);

static net_buff_info_t pack_pool[CONFIG_PACK_POOL_SIZE];

int __init net_buff_init(void) {
	size_t i;
	for (i = 0; i < array_len(pack_pool); i ++) {
		list_add(&(&pack_pool[i])->list, &head_free_pack);
	}
	return 0;
}

unsigned char *net_buff_alloc(void) {
	net_buff_info_t *entry;
	unsigned char * buff;
	ipl_t sp;
	sp = ipl_save();
	if (list_empty (&head_free_pack)) {
		ipl_restore(sp);
		return NULL;
	}
	entry = (net_buff_info_t *)((&head_free_pack)->next);
	list_del_init((struct list_head *)entry);
	buff = (unsigned char *)list_entry((struct list_head *)entry,
										net_buff_info_t, list);
	ipl_restore(sp);
	return buff;
}

void net_buff_free(unsigned char *buff) {
	net_buff_info_t *pack;
	ipl_t sp;
	sp = ipl_save();
	/*return buff into pull*/
	/* we can cast like this because buff is first element of
	 * struct socket_info
	 */
	pack = (net_buff_info_t *)buff;
	list_add(&pack->list, &head_free_pack);
	buff = NULL;
	ipl_restore(sp);
}
