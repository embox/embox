/**
 * @file
 * @details sk_buff management API implementation.
 * @date 20.10.09
 *
 * @author Anton Bondarev
 * @author Ilia Vaprol
 * @author Vladimir Sokolov
 */

#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <sys/uio.h>

#include <util/math.h>
#include <util/log.h>
#include <util/member.h>
#include <util/binalign.h>

#include <hal/ipl.h>

#include <mem/misc/pool.h>
#include <mem/sysmalloc.h>

#include <linux/list.h>

#include <net/skbuff.h>

#include <framework/mod/options.h>

#define MODOPS_AMOUNT_SKB_DATA  OPTION_GET(NUMBER, amount_skb_data)
#define MODOPS_DATA_SIZE        OPTION_GET(NUMBER, data_size)
#define MODOPS_DATA_ALIGN       OPTION_GET(NUMBER, data_align)
#define MODOPS_DATA_PADTO       OPTION_GET(NUMBER, data_padto)

#define IP_ALIGN_SIZE \
	(OPTION_GET(BOOLEAN, ip_align) ? 2 : 0)

#define DATA_PAD_SIZE \
	PAD_SIZE(IP_ALIGN_SIZE + MODOPS_DATA_SIZE, MODOPS_DATA_PADTO)
#define DATA_ATTR \
	__attribute__((aligned(MODOPS_DATA_ALIGN)))

#define SKB_DATA_SIZE(size) \
	IP_ALIGN_SIZE + MODOPS_DATA_SIZE + (size) + sizeof(size_t)

struct sk_buff_data_fixed {
	size_t links;

	char __ip_align[IP_ALIGN_SIZE];
	unsigned char data[MODOPS_DATA_SIZE];
	char __data_pad[DATA_PAD_SIZE];
};

struct sk_buff_data {
	size_t links;

	char __data[];
} DATA_ATTR;

POOL_DEF(skb_data_pool, struct sk_buff_data_fixed, MODOPS_AMOUNT_SKB_DATA);

void *skb_get_data_pointner(struct sk_buff_data *skb_data) {
	return skb_data->__data + IP_ALIGN_SIZE;
}

struct sk_buff_data * skb_data_alloc_dynamic(size_t size) {
	ipl_t sp;
	struct sk_buff_data *skb_data;

	sp = ipl_save();
	{
		skb_data = (struct sk_buff_data *) sysmalloc(SKB_DATA_SIZE(size));
	}
	ipl_restore(sp);

	if (skb_data == NULL) {
		log_error("skb_data_alloc: error: no memory\n");
		return NULL; /* error: no memory */
	}

	skb_data->links = 1;

	return skb_data;
}

size_t skb_max_size(void) {
	return MODOPS_DATA_SIZE;
}

void * skb_data_cast_in(struct sk_buff_data *skb_data) {
	assert(skb_data != NULL);
	return skb_get_data_pointner(skb_data);
}

struct sk_buff_data * skb_data_cast_out(void *data) {
	assert(data != NULL);
	return member_cast_out(data - IP_ALIGN_SIZE, struct sk_buff_data, __data);
}

struct sk_buff_data * skb_data_alloc(void) {
	ipl_t sp;
	struct sk_buff_data *skb_data;

	sp = ipl_save();
	{
		skb_data = pool_alloc(&skb_data_pool);
	}
	ipl_restore(sp);

	if (skb_data == NULL) {
		log_error("skb_data_alloc: error: no memory\n");
		return NULL; /* error: no memory */
	}

	skb_data->links = 1;

	return skb_data;
}

struct sk_buff_data * skb_data_clone(struct sk_buff_data *skb_data) {
	ipl_t sp;

	assert(skb_data != NULL);

	sp = ipl_save();
	{
		++skb_data->links;
	}
	ipl_restore(sp);

	return skb_data;
}

int skb_data_cloned(const struct sk_buff_data *skb_data) {
	return skb_data->links != 1;
}

void skb_data_free(struct sk_buff_data *skb_data) {
	ipl_t sp;

	assert(skb_data != NULL);

	sp = ipl_save();
	{
		if (--skb_data->links == 0) {
			if (pool_belong(&skb_data_pool, skb_data)) {
				pool_free(&skb_data_pool, skb_data);
			}
			else {
				sysfree(skb_data);
			}
		}
	}
	ipl_restore(sp);
}
