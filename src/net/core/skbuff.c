/**
 * @file
 * @details this module realize interface with structure sk_buff
 * structures sk_buff are presented queue free packet.
 *
 * @date 20.10.2009
 * @author Anton Bondarev
 */

#include <string.h> /*for memcpy*/
#include <linux/list.h>
#include <net/skbuff.h>
#include <net/sock.h>
#include <kernel/mm/slab_static.h>
#include <linux/init.h>
#include <linux/spinlock.h>
#include <hal/ipl.h>

/* static cache for sk_buff allocate */
STATIC_CACHE_CREATE(skb_cache, struct sk_buff, CONFIG_QUANTITY_SKB);
/* static cache for sk_buff_head allocate */
STATIC_CACHE_CREATE(skb_queue_cache,
		struct sk_buff_head,
		CONFIG_QUANTITY_SKB_QUEUE);
/* static cache for net_packet allocate */
STATIC_CACHE_CREATE(net_buff_cache,
		unsigned char[CONFIG_ETHERNET_V2_FRAME_SIZE],
		CONFIG_PACK_POOL_SIZE);


/**
 * Allocate net_packet_buff for one ethernet packet on the cache.
 * @return net_packet
 */
static unsigned char *net_buff_alloc(void) {
	unsigned char * buff;
	ipl_t sp;
	sp = ipl_save();
	buff = static_cache_alloc(&net_buff_cache);
	ipl_restore(sp);
	return buff;
}


/**
 * Free buff of net packet.
 * @param buff
 */
static void net_buff_free(unsigned char *buff) {
	ipl_t sp;
	sp = ipl_save();
	static_cache_free(&net_buff_cache, buff);
	buff = NULL;
	ipl_restore(sp);
}


struct sk_buff_head *alloc_skb_queue(int len) {
	struct sk_buff_head *queue;
	ipl_t sp;
	sp = ipl_save();
	if(NULL == (queue = static_cache_alloc(&skb_queue_cache))) {
		ipl_restore(sp);
		return NULL;
	}
	/* in free queue we held structure sk_buff_head but this pointer has sk_buff * type
	 * we must assignment it
	 */
	INIT_LIST_HEAD((struct list_head *) queue);
	queue->qlen = 0;
	ipl_restore(sp);
	queue->qlen = len;
	return queue;
}


struct sk_buff *alloc_skb(unsigned int size, gfp_t priority) {
	struct sk_buff *skb;
	/*TODO only one packet now*/

	ipl_t sp;
	sp = ipl_save();
	if(NULL == (skb = static_cache_alloc(&skb_cache)))	{
		ipl_restore(sp);
		return NULL;
	}

	INIT_LIST_HEAD((struct list_head *) skb);
	ipl_restore(sp);

	if (NULL == (skb->data = net_buff_alloc())) {
		kfree_skb(skb);
		return NULL;
	}
	skb ->len = size;
	skb->mac.raw = skb->data;
	return skb;
}


void kfree_skb(struct sk_buff *skb) {
        ipl_t sp;
        if (NULL == skb) {
                return;
        }
        net_buff_free(skb->data);
        sp = ipl_save();
        if ((NULL != skb->prev) && (NULL != skb->next)) {
                list_del((struct list_head *) skb);
        }
        static_cache_free(&skb_cache, skb);
        skb = NULL;
        ipl_restore(sp);
}


void skb_queue_tail(struct sk_buff_head *list, struct sk_buff *newsk) {
        ipl_t sp;
        if (NULL == list || NULL == newsk) {
                return;
        }
        sp = ipl_save();
        list_move_tail((struct list_head *) newsk, (struct list_head *) list);
        list->qlen++;
        ipl_restore(sp);
}


sk_buff_t *skb_peek(struct sk_buff_head *list_) {
        sk_buff_t *list = ((sk_buff_t *) list_)->next;
        if (list == (sk_buff_t *) list_) {
                list = NULL;
        }
        return list;
}


void skb_unlink(sk_buff_t *skb, struct sk_buff_head *list) {
        struct sk_buff *next, *prev;
        list->qlen--;
        next = skb->next;
        prev = skb->prev;
        skb->next = skb->prev = skb;
        next->prev = prev;
        prev->next = next;
}


sk_buff_t *skb_dequeue(struct sk_buff_head *list) {
        struct sk_buff *skb;
        ipl_t sp;
        sp = ipl_save();
        skb = skb_peek(list);
        if (skb) {
                skb_unlink(skb, list);
        }
        ipl_restore(sp);
        return skb;
}


void skb_queue_purge(struct sk_buff_head *queue) {
        sk_buff_t *skb;
        while ((skb = skb_dequeue(queue)) != NULL) {
                kfree_skb(skb);
        }
        static_cache_free(&skb_queue_cache, queue);
        queue = NULL;
}


/**
 * split buffer to skb queue
 */
/*TODO buff_to_skb not realize now*/
struct sk_buff *buff_to_skb(unsigned char *buff) {
        return NULL;
}


struct sk_buff *alloc_skb_fclone(struct sk_buff *skb, gfp_t priority) {
        return NULL;
}


struct sk_buff *skb_copy(const struct sk_buff *skb, gfp_t priority) {
        struct sk_buff *new_pack;
        if ((NULL == skb) || (NULL == (new_pack = alloc_skb(skb->len, priority)))) {
                return NULL;
        }
        memcpy(new_pack->data, skb->data, skb->len);


        /*fix references during copy net_pack*/
        if (NULL != skb->h.raw) {
                new_pack->h.raw = new_pack->data + (skb->h.raw - skb->data);
        }
        if (NULL != skb->nh.raw) {
                new_pack->nh.raw = new_pack->data + (skb->nh.raw - skb->data);
        }
        if (NULL != skb->mac.raw) {
                new_pack->mac.raw = new_pack->data + (skb->mac.raw - skb->data);
        }
        return new_pack;
}


struct sk_buff *skb_recv_datagram(struct sock *sk, unsigned flags,
                                int noblock, int *err) {
        struct sk_buff *skb;
        ipl_t sp;
        sp = ipl_save();
        skb = skb_peek(sk->sk_receive_queue);
        if (skb) {
                skb_unlink(skb, sk->sk_receive_queue);
        }
        ipl_restore(sp);
        return skb;
}
