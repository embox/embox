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
#include <stdbool.h>

#include <util/math.h>
#include <util/log.h>
#include <util/member.h>
#include <util/binalign.h>
#include <mem/sysmalloc.h>

#include <hal/ipl.h>

#include <mem/misc/pool.h>

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
#if MODOPS_DATA_ALIGN > 1
#define DATA_ATTR \
	__attribute__((aligned(MODOPS_DATA_ALIGN)))
#else
#define DATA_ATTR
#endif
#define SKB_DATA_SIZE(size) \
	IP_ALIGN_SIZE + MODOPS_DATA_SIZE + (size) + sizeof(size_t)

#define ALLOCATED_POOL 0
#define ALLOCATED_MALLOC 1

struct sk_buff_data_fixed {
	int links;
	int alloc_type;

	char __ip_align[IP_ALIGN_SIZE];
	unsigned char data[MODOPS_DATA_SIZE] DATA_ATTR;
	char __data_pad[DATA_PAD_SIZE];
};

POOL_DEF(skb_data_pool, struct sk_buff_data_fixed, MODOPS_AMOUNT_SKB_DATA);

void *skb_get_data_pointner(struct sk_buff_data *data) {
	struct sk_buff_data_fixed *skb_data = (void *)data;

	return skb_data->data;
}

size_t skb_max_size(void) {
	return MODOPS_DATA_SIZE;
}

static inline bool skb_data_is_huge(size_t size) {
	return size > skb_max_size();
}

void * skb_data_cast_in(struct sk_buff_data *skb_data) {
	assert(skb_data != NULL);
	return skb_get_data_pointner(skb_data);
}

struct sk_buff_data * skb_data_cast_out(void *data) {
	assert(data != NULL);
	return (void *)member_cast_out(data - IP_ALIGN_SIZE, struct sk_buff_data_fixed, __ip_align);
}

struct sk_buff_data * skb_data_alloc(size_t size) {
	ipl_t sp;
	struct sk_buff_data_fixed *skb_data;
	int alloc_type = -1;

	sp = ipl_save();
	{
		if (!skb_data_is_huge(size)) {
			skb_data = pool_alloc(&skb_data_pool);
			alloc_type = ALLOCATED_POOL;
		} else {
			skb_data = sysmalloc(SKB_DATA_SIZE(size));
			alloc_type = ALLOCATED_MALLOC;
		}
	}
	ipl_restore(sp);

	if (skb_data == NULL) {
		log_error("no memory skb_size = %d", size);
		return NULL; /* error: no memory */
	}

	skb_data->alloc_type = alloc_type;
	skb_data->links = 1;

	return (void *)skb_data;
}

struct sk_buff_data * skb_data_clone(struct sk_buff_data *data) {
	struct sk_buff_data_fixed *skb_data = (void *)data;
	ipl_t sp;

	assert(skb_data != NULL);

	sp = ipl_save();
	{
		++skb_data->links;
	}
	ipl_restore(sp);

	return (void *)skb_data;
}

int skb_data_cloned(const struct sk_buff_data *data) {
	struct sk_buff_data_fixed *skb_data = (void *)data;
	return skb_data->links != 1;
}

void skb_data_free(struct sk_buff_data *data) {
	struct sk_buff_data_fixed *skb_data = (void *)data;
	ipl_t sp;

	assert(skb_data != NULL);

	sp = ipl_save();
	{
		skb_data->links--;
		assert(skb_data->links >= 0);

		if (skb_data->links == 0) {
			switch (skb_data->alloc_type) {
			case ALLOCATED_POOL:
				pool_free(&skb_data_pool, skb_data);
				break;
			case ALLOCATED_MALLOC:
				sysfree(skb_data);
				break;
			default:
				log_error("Wrong skb->alloc_type = %d", skb_data->alloc_type);
				break;
			}
		}
	}
	ipl_restore(sp);
}
