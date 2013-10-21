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

#define MODOPS_AMOUNT_SKB      OPTION_GET(NUMBER, amount_skb)
#define MODOPS_AMOUNT_SKB_DATA OPTION_GET(NUMBER, amount_skb_data)
#define MODOPS_EXTRA_HDR_SIZE  OPTION_GET(NUMBER, extra_hdr_size)
#define MODOPS_DATA_SIZE       OPTION_GET(NUMBER, data_size)
#define MODOPS_DATA_ALIGN      OPTION_GET(NUMBER, data_align)

#if MODOPS_DATA_ALIGN == 0
#define ATTR_ALIGNED
#define PAD_SIZE 0
#else
#define SKB_ALIGNMENT 4
#define ATTR_ALIGNED __attribute__((aligned(SKB_ALIGNMENT)))
#define PAD_SIZE MODOPS_DATA_ALIGN
#endif

struct sk_buff_data {
	unsigned char extra_hdr[MODOPS_EXTRA_HDR_SIZE] ATTR_ALIGNED;
	struct {
		char __unused[PAD_SIZE];
		unsigned char data[MODOPS_DATA_SIZE];
	} ATTR_ALIGNED;
	size_t links;
};

POOL_DEF(skb_pool, struct sk_buff, MODOPS_AMOUNT_SKB);
POOL_DEF(skb_data_pool, struct sk_buff_data, MODOPS_AMOUNT_SKB_DATA);

unsigned char * skb_data_get_extra_hdr(
		struct sk_buff_data *skb_data) {
	return &skb_data->extra_hdr[0];
}

unsigned char * skb_data_get_data(
		struct sk_buff_data *skb_data) {
	return &skb_data->data[0];
}

size_t skb_max_extra_hdr_size(void) {
	return member_sizeof(struct sk_buff_data, extra_hdr);
}

size_t skb_max_size(void) {
	return member_sizeof(struct sk_buff_data, data);
}

size_t skb_avail(const struct sk_buff *skb) {
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

struct sk_buff * skb_wrap(size_t size, size_t offset,
		struct sk_buff_data *skb_data) {
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
	skb->p_data = &skb_data->data[0];
	skb->p_data_end = &skb_data->data[size];

	/* TODO remove this */
	skb->mac.raw = skb->p_data;

	return skb;
}

struct sk_buff * skb_alloc(size_t size) {
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
	ptrdiff_t offset;

	assert((to != NULL) && (to->data != NULL)
			&& (from != NULL) && (from->data != NULL));

	to->sk = from->sk;
	to->dev = from->dev;
	offset = &to->data->data[0] - &from->data->data[0];
	if (from->mac.raw != NULL) {
		to->mac.raw = from->mac.raw + offset;
	}
	if (from->nh.raw != NULL) {
		to->nh.raw = from->nh.raw + offset;
	}
	if (from->h.raw != NULL) {
		to->h.raw = from->h.raw + offset;
	}
	if (from->p_data != NULL) {
		to->p_data = from->p_data + offset;
	}
	if (from->p_data_end != NULL) {
		to->p_data_end = from->p_data_end + offset;
	}
}

static void skb_copy_data(struct sk_buff *to, const struct sk_buff *from) {
	assert((to != NULL) && (to->data != NULL)
			&& (from != NULL) && (from->data != NULL)
			&& (to->len == from->len));
	memcpy(&to->data->data[0], &from->data->data[0], from->len);
}

struct sk_buff * skb_copy(const struct sk_buff *skb) {
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

struct sk_buff * skb_clone(const struct sk_buff *skb) {
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

void skb_rshift(struct sk_buff *skb, size_t count) {
	assert(skb != NULL);
	assert(skb->data != NULL);
	assert(count + skb->len <= skb_max_size());
	memmove(&skb->data->data[count], &skb->data->data[0], skb->len);
	skb->len += count;
	skb->p_data_end += count;
}
