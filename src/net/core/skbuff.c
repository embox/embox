/**
 * @file
 * @details sk_buff management API implementation.
 *
 * @date 20.10.09
 * @author Anton Bondarev
 * @author Ilia Vaprol
 * @author Vladimir Sokolov
 */

#include <stdio.h>
#include <hal/ipl.h>
#include <linux/init.h>
#include <linux/list.h>
#include <mem/misc/pool.h>
#include <net/ip.h>
#include <net/skbuff.h>
#include <string.h>
#include <assert.h>


POOL_DEF(skb_pool, struct sk_buff, CONFIG_QUANTITY_SKB);
POOL_DEF(skb_queue_pool, struct sk_buff_head, CONFIG_QUANTITY_SKB_QUEUE);
POOL_DEF(net_buff_pool, unsigned char[SK_BUF_EXTRA_HEADROOM + CONFIG_ETHERNET_V2_FRAME_SIZE], CONFIG_PACK_POOL_SIZE);

/**
 * Allocate net_packet_buff for one ethernet packet on the cache.
 * @return net_packet
 */
static unsigned char * net_buff_alloc(void) {
	ipl_t sp;
	unsigned char *buff;

	sp = ipl_save();
	buff = pool_alloc(&net_buff_pool);
	ipl_restore(sp);

	return buff;
}

/**
 * Free buff of net packet.
 * @param buff
 */
static void net_buff_free(unsigned char *buff) {
	ipl_t sp;

	/* printk("deallocating 0x%X\n", (unsigned int)buff); */

	sp = ipl_save();
	pool_free(&net_buff_pool, buff);
	ipl_restore(sp);
}

struct sk_buff_head * alloc_skb_queue(void) {
	ipl_t sp;
	struct sk_buff_head *queue;

	sp = ipl_save();
	queue = (struct sk_buff_head *)pool_alloc(&skb_queue_pool);
	ipl_restore(sp);
	if (queue == NULL) {
		return NULL;
	}
	/* in free queue we held structure sk_buff_head but this pointer has sk_buff * type
	 * we must assignment it
	 */
	INIT_LIST_HEAD((struct list_head *)queue);

	return queue;
}

void kfree_skb(struct sk_buff *skb) {
	ipl_t sp;

	assert(skb);

	net_buff_free(skb->head);

	sp = ipl_save();

	if ((skb->prev != NULL) && (skb->next != NULL)) {
		list_del((struct list_head *)skb);
	}
	pool_free(&skb_pool, skb);

	ipl_restore(sp);
}

struct sk_buff * alloc_skb(unsigned int size, gfp_t priority) {
	ipl_t sp;
	struct sk_buff *skb;
	unsigned char *head;

	assert((size > 0) && (size <= CONFIG_ETHERNET_V2_FRAME_SIZE));

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

void skb_queue_tail(struct sk_buff_head *list, struct sk_buff *newsk) {
	ipl_t sp;

	assert(list != NULL);
	assert(newsk != NULL);

	sp = ipl_save();
	list_move_tail((struct list_head *)newsk, (struct list_head *)list);
	ipl_restore(sp);
}

struct sk_buff * skb_peek(struct sk_buff_head *list) {
	struct sk_buff *next;

	assert(list != NULL);

	next = ((struct sk_buff *)list)->next;

	assert(list->next != NULL);

	if (next == (struct sk_buff *)list) {
		return NULL;
	}

	return next;
}

static void skb_unlink(struct sk_buff *skb, struct sk_buff_head *list) {
	struct sk_buff *next, *prev;

	assert(skb != NULL);
	assert(list != NULL);

	next = skb->next;
	prev = skb->prev;
	skb->next = skb->prev = skb;
	next->prev = prev;
	prev->next = next;
}

struct sk_buff * skb_dequeue(struct sk_buff_head *list) {
	ipl_t sp;
	struct sk_buff *skb;

	assert(list != NULL);

	sp = ipl_save();

	skb = skb_peek(list);
	if (skb != NULL) {
		skb_unlink(skb, list);
	}

	ipl_restore(sp);

	return skb;
}

void skb_queue_purge(struct sk_buff_head *queue) {
	ipl_t sp;
	struct sk_buff *skb;

	assert(queue != NULL);

	while ((skb = skb_dequeue(queue)) != NULL) {
		kfree_skb(skb);
	}

	sp = ipl_save();
	pool_free(&skb_queue_pool, queue);
	ipl_restore(sp);
}

#if 0
/**
 * Split buffer to skb queue
 */
/* FIXME size must be less then CONFIG_ETHERNET_V2_FRAME_SIZE */
struct sk_buff * buff_to_skb(unsigned char *buff, unsigned int size) {
	struct sk_buff *skb;


	if ((buff == NULL) || (size == 0)) {
		return NULL;
	}

	skb = alloc_skb(size, 0);
	if (skb == NULL) {
		return NULL;
	}

	memcpy(skb->mac.raw, buff, size * sizeof(unsigned char));
	skb->protocol = ntohs(skb->mac.ethh->h_proto);
	skb->nh.raw = skb->mac.raw + ETH_HEADER_SIZE;
	if (skb->protocol == ETH_P_IP) {
		skb->h.raw = skb->nh.raw + IP_HEADER_SIZE(skb->nh.iph);
	}

	return skb;
}
#endif

static void skb_copydata(struct sk_buff *new_pack, const struct sk_buff *skb) {
	memmove(new_pack->mac.raw, skb->mac.raw, skb->len);
	new_pack->len = skb->len;

	/*fix data references during copy net_pack*/
	if (skb->h.raw) {
		new_pack->h.raw = new_pack->mac.raw + (skb->h.raw - skb->mac.raw);
	}
	if (skb->nh.raw) {
		new_pack->nh.raw = new_pack->mac.raw + (skb->nh.raw - skb->mac.raw);
	}
	if (skb->p_data) {
		new_pack->p_data = new_pack->mac.raw + (skb->p_data - skb->mac.raw);
	}
}

static inline struct sk_buff *skb_allocatecopydata(const struct sk_buff *skb, gfp_t priority) {
	struct sk_buff *new_pack;

	assert(skb != NULL);

	new_pack = alloc_skb(skb->len, priority);
	if (new_pack == NULL) {
//		LOG_ERROR("new_pack is null. len=%u\n", skb->len);
		return NULL;
	}

	skb_copydata(new_pack, skb);

	return new_pack;
}


struct sk_buff * skb_clone(struct sk_buff *skb, gfp_t priority) {
	struct sk_buff *clone = skb_allocatecopydata(skb, priority);

	if (clone == NULL) {
		return NULL;
	}

	clone->dev = skb->dev;
	clone->protocol = skb->protocol;
	clone->sk = skb->sk;

	return clone;
}

struct sk_buff * skb_peek_datagram(struct sock *sk, unsigned flags, int noblock,
		int *err) {

	if ((sk == NULL) || (sk->sk_receive_queue == NULL)) {
		return NULL;
	}

	return skb_peek(sk->sk_receive_queue);
}

struct sk_buff *skb_checkcopy_expand(struct sk_buff *skb, int headroom, int tailroom, gfp_t priority) {
	int free_headroom = skb->mac.raw - skb->head;
	int free_tailroom = SK_BUF_EXTRA_HEADROOM + CONFIG_ETHERNET_V2_FRAME_SIZE - (free_headroom + skb->len);
	int free_space = SK_BUF_EXTRA_HEADROOM + CONFIG_ETHERNET_V2_FRAME_SIZE - (skb->len + headroom + tailroom);

		/* Stupid situations during shrink */
	assert((-headroom) < (skb->len + tailroom));

	if (likely((headroom <= free_headroom) && (tailroom <= free_tailroom))) {
		/* Simplest case, do nothing */
		return skb;
	} else if (free_space >= 0) {
		/* We still can fit reserved buffer */
		struct sk_buff skb_fields_save = *skb;
		skb->mac.raw = skb->head + (free_space / 2);	/* Be fair to the tail and head */
		skb_copydata(skb, &skb_fields_save);
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

struct sk_buff * skb_recv_datagram(struct sock *sk, unsigned flags, int noblock,
		int *err) {
	ipl_t sp;

	struct sk_buff *skb = skb_peek_datagram(sk, flags, noblock, err);

	if (skb != NULL) {
		sp = ipl_save();

		skb_unlink(skb, sk->sk_receive_queue);

		ipl_restore(sp);
	}

	return skb;
}
