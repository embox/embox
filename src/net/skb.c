/**
 * @file
 * @details sk_buff management API implementation.
 * @date 20.10.09
 *
 * @author Anton Bondarev
 * @author Ilia Vaprol
 * @author Vladimir Sokolov
 */

#include <net/skbuff.h>
#include <hal/ipl.h>
#include <mem/misc/pool.h>
#include <assert.h>
#include <string.h>
#include <linux/compiler.h>
#include <framework/mod/options.h>
#include <linux/list.h>
#include <kernel/printk.h>
#include <util/member.h>

#define MODOPS_AMOUNT_SKB         OPTION_GET(NUMBER, amount_skb)
#define MODOPS_AMOUNT_SKB_DATA    OPTION_GET(NUMBER, amount_skb_data)
#define MODOPS_SKB_EXTRA_HDR_SIZE OPTION_GET(NUMBER, skb_extra_hdr_size)
#define MODOPS_SKB_DATA_SIZE      OPTION_GET(NUMBER, skb_data_size)

struct sk_buff_data {
	unsigned char buff[MODOPS_SKB_EXTRA_HDR_SIZE + MODOPS_SKB_DATA_SIZE];
	unsigned int links;
};

POOL_DEF(skb_pool, struct sk_buff, MODOPS_AMOUNT_SKB);
POOL_DEF(skb_data_pool, struct sk_buff_data, MODOPS_AMOUNT_SKB_DATA);

unsigned int skb_max_extra_hdr_size(void) {
	return MODOPS_SKB_EXTRA_HDR_SIZE;
}

unsigned int skb_max_size(void) {
	return MODOPS_SKB_DATA_SIZE;
}

unsigned int skb_avail(struct sk_buff *skb) {
	return skb_max_size() - skb->len;
}

struct sk_buff_data * skb_data_alloc(void) {
	ipl_t sp;
	struct sk_buff_data *skb_data;

	sp = ipl_save();
	{
		skb_data = (struct sk_buff_data *)pool_alloc(&skb_data_pool);
	}
	ipl_restore(sp);

	if (skb_data == NULL) {
		printk("skb_data_alloc: error: no memory\n");
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

	return skb_data; /* cloned */
}

void skb_data_free(struct sk_buff_data *skb_data) {
	ipl_t sp;

	assert(skb_data != NULL);

	sp = ipl_save();
	{
		if (--skb_data->links == 0) {
			pool_free(&skb_data_pool, skb_data);
		}
	}
	ipl_restore(sp);
}

struct sk_buff * skb_wrap(unsigned int size,
		unsigned int offset, struct sk_buff_data *skb_data) {
	ipl_t sp;
	struct sk_buff *skb;

	assert(size != 0);
	assert(skb_data != NULL);

	if ((offset > skb_max_extra_hdr_size())
			|| (size > skb_max_size())) {
		printk("skb_wrap: error: size is too big\n");
		return NULL; /* error: invalid argument */
	}

	sp = ipl_save();
	{
		skb = (struct sk_buff *)pool_alloc(&skb_pool);
	}
	ipl_restore(sp);

	if (skb == NULL) {
		printk("skb_wrap: error: no memory\n");
		return NULL; /* error: no memory */
	}

	INIT_LIST_HEAD((struct list_head *)skb);
	skb->sk = NULL;
	skb->dev = NULL;
	skb->len = size;
	skb->mac.raw = skb->nh.raw = skb->h.raw = NULL;
	skb->data = skb_data;
	skb->p_data = &skb_data->buff[offset];
	skb->p_data_end = &skb_data->buff[offset + size];

	/* TODO remove this */
	skb->mac.raw = skb->p_data;

	return skb;
}

struct sk_buff * skb_alloc(unsigned int size) {
	struct sk_buff *skb;
	struct sk_buff_data *skb_data;

	skb_data = skb_data_alloc();
	if (skb_data == NULL) {
		return NULL; /* error: no memory */
	}

	skb = skb_wrap(size, skb_max_extra_hdr_size(), skb_data);
	if (skb == NULL) {
		skb_data_free(skb_data);
		return NULL; /* error: no memory */
	}

	return skb;
}

void skb_free(struct sk_buff *skb) {
	ipl_t sp;

	assert(skb != NULL);

	skb_data_free(skb->data);

	sp = ipl_save();
	{
		assert((skb->lnk.prev != NULL) && (skb->lnk.next != NULL));
		list_del((struct list_head *)skb);
		pool_free(&skb_pool, skb);
	}
	ipl_restore(sp);
}

static void skb_copy_ref(struct sk_buff *to,
		const struct sk_buff *from) {
	assert((to != NULL) && (to->data != NULL)
			&& (from != NULL) && (from->data != NULL));

	to->sk = from->sk;
	to->dev = from->dev;
	if (from->mac.raw != NULL) {
		to->mac.raw = &to->data->buff[0] + (from->mac.raw
					- &from->data->buff[0]);
	}
	if (from->nh.raw != NULL) {
		to->nh.raw = &to->data->buff[0] + (from->nh.raw
					- &from->data->buff[0]);
	}
	if (from->h.raw != NULL) {
		to->h.raw = &to->data->buff[0] + (from->h.raw
					- &from->data->buff[0]);
	}
	if (from->p_data != NULL) {
		to->p_data = &to->data->buff[0] + (from->p_data
					- &from->data->buff[0]);
	}
	if (from->p_data_end != NULL) {
		to->p_data_end = &to->data->buff[0] + (from->p_data_end
					- &from->data->buff[0]);
	}
}

static void skb_copy_data(struct sk_buff *to, const struct sk_buff *from) {
	assert((to != NULL) && (to->data != NULL)
			&& (from != NULL) && (from->data != NULL)
			&& (to->len == from->len));
	memcpy(&to->data->buff[skb_max_extra_hdr_size()],
			&from->data->buff[skb_max_extra_hdr_size()], from->len);
}

struct sk_buff * skb_copy(struct sk_buff *skb) {
	struct sk_buff *copied;

	assert(skb != NULL);

	copied = skb_alloc(skb->len);
	if (copied == NULL) {
		return NULL; /* error: no memory */
	}

	skb_copy_ref(copied, skb);
	skb_copy_data(copied, skb);

	return copied;
}

struct sk_buff * skb_clone(struct sk_buff *skb) {
	struct sk_buff *cloned;
	struct sk_buff_data *cloned_data;

	assert(skb != NULL);

	cloned_data = skb_data_clone(skb->data);
	if (cloned_data == NULL) {
		return NULL; /* error: no memory */
	}

	cloned = skb_wrap(skb->len, 0, cloned_data);
	if (cloned == NULL) {
		skb_data_free(cloned_data);
		return NULL; /* error: no memory */
	}

	skb_copy_ref(cloned, skb);

	return cloned;
}

void skb_rshift(struct sk_buff *skb, unsigned int count) {
	assert(skb != NULL);
	assert(skb->data != NULL);
	assert(count + skb->len <= skb_max_size());
	memmove(&skb->data->buff[skb_max_extra_hdr_size() + count],
			&skb->data->buff[skb_max_extra_hdr_size()], skb->len);
	skb->len += count;
	skb->p_data_end += count;
}
