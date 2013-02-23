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
#include <compiler.h>
#include <framework/mod/options.h>
#include <linux/list.h>

#define MODOPS_AMOUNT_SKB      OPTION_GET(NUMBER, amount_skb)
#define MODOPS_AMOUNT_SKB_BUFF OPTION_GET(NUMBER, amount_skb_buff)
#define MODOPS_SKB_BUFF_SIZE   OPTION_GET(NUMBER, skb_buff_size)

POOL_DEF(skb_pool, struct sk_buff, MODOPS_AMOUNT_SKB);
POOL_DEF(net_buff_pool, unsigned char[SK_BUF_EXTRA_HEADROOM + MODOPS_SKB_BUFF_SIZE], MODOPS_AMOUNT_SKB_BUFF);

static unsigned char * net_buff_alloc(void) {
	ipl_t sp;
	unsigned char *buff;

	sp = ipl_save();
	buff = pool_alloc(&net_buff_pool);
	ipl_restore(sp);

	return buff;
}

static void net_buff_free(unsigned char *buff) {
	ipl_t sp;

	assert(buff != NULL);

	sp = ipl_save();
	pool_free(&net_buff_pool, buff);
	ipl_restore(sp);
}

struct sk_buff * skb_alloc(unsigned int size) {
	ipl_t sp;
	struct sk_buff *skb;
	unsigned char *head;

	if ((size == 0) || (size > MODOPS_SKB_BUFF_SIZE)) {
		return NULL;
	}

	head = net_buff_alloc();
	if (head == NULL) {
		return NULL;
	}

	sp = ipl_save();
	skb = (struct sk_buff *)pool_alloc(&skb_pool);
	ipl_restore(sp);

	if (skb == NULL) {
		net_buff_free(head);
		return NULL;
	}

	memset(skb, 0, sizeof *skb);
	skb->head = head;
	INIT_LIST_HEAD((struct list_head *)skb);
	skb->mac.raw = skb->head + SK_BUF_EXTRA_HEADROOM;
	/* Really skb->nh.raw (as a pointer) is also defined now,
	 * because everything supports only Ethernet.
	 * Does NULL pointer give us something more reasonable?
	 */
	skb->len = size;

	return skb;
}

void skb_free(struct sk_buff *skb) {
	ipl_t sp;

	assert(skb != NULL);

	net_buff_free(skb->head);

	sp = ipl_save();

	if ((skb->prev != NULL) && (skb->next != NULL)) {
		list_del((struct list_head *)skb);
	}
	pool_free(&skb_pool, skb);

	ipl_restore(sp);
}

static void skb_copy_data(struct sk_buff *to, const struct sk_buff *from) {
	assert((to->mac.raw != NULL) && (from->mac.raw != NULL));
	memcpy(to->mac.raw, from->mac.raw, from->len);

	to->len = from->len;

	/*fix data references during copy net_pack*/
	if (from->nh.raw != NULL) {
		to->nh.raw = to->mac.raw + (from->nh.raw - from->mac.raw);
	}
	if (from->h.raw != NULL) {
		to->h.raw = to->mac.raw + (from->h.raw - from->mac.raw);
	}
	if (from->p_data != NULL) {
		to->p_data = to->mac.raw + (from->p_data - from->mac.raw);
	}
}

struct sk_buff * skb_duplicate(struct sk_buff *skb) {
	struct sk_buff *duplicate;

	assert(skb != NULL);

	duplicate = skb_alloc(skb->len);
	if (duplicate == NULL) {
		return NULL;
	}

	duplicate->sk = skb->sk;
	duplicate->dev = skb->dev;
	duplicate->protocol = skb->protocol;
	skb_copy_data(duplicate, skb);

	return duplicate;
}

struct sk_buff *skb_checkcopy_expand(struct sk_buff *skb, int headroom, int tailroom, gfp_t priority) {
	int free_headroom = skb->mac.raw - skb->head;
	int free_tailroom = SK_BUF_EXTRA_HEADROOM + MODOPS_SKB_BUFF_SIZE - (free_headroom + skb->len);
	int free_space = SK_BUF_EXTRA_HEADROOM + MODOPS_SKB_BUFF_SIZE - (skb->len + headroom + tailroom);

		/* Stupid situations during shrink */
	assert(((long)(-headroom)) < ((long)skb->len + (long)tailroom));

	if (likely((headroom <= free_headroom) && (tailroom <= free_tailroom))) {
		/* Simplest case, do nothing */
		return skb;
	} else if (free_space >= 0) {
		/* We still can fit reserved buffer */
		struct sk_buff skb_fields_save = *skb;
		skb->mac.raw = skb->head + (free_space / 2);	/* Be fair to the tail and head */
		skb_copy_data(skb, &skb_fields_save);
		return skb;
	} else {
		/* There is no way in current implementaion to give more than
		 * we have space in pool. You should use sk_buff_head somehow
		 */
		return NULL;
	}
}

void skb_shifthead(struct sk_buff *skb, int headshift) {
	skb->mac.raw -= headshift;
	if(likely(skb->h.raw))
		skb->h.raw -= headshift;
	if(likely(skb->nh.raw))
		skb->nh.raw -= headshift;
	assert(skb->head <= skb->mac.raw);
	assert((int)skb->len >= (-headshift));
	skb->len += headshift;
}
