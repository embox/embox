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

static struct sk_buff sk_buff_pool[CONFIG_QUANTITY_SKB];
static LIST_HEAD(head_free_skb);

static struct sk_buff_head sk_queue_pool[CONFIG_QUANTITY_SKB_QUEUE];
static LIST_HEAD(head_free_queue);

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
        if (list_empty(&head_free_queue)) {
                ipl_restore(sp);
                return NULL;
        }
        /* in free queue we held structure sk_buff_head but this pointer has sk_buff * type
         * we must assignment it
         */
        queue = (struct sk_buff_head *) (&head_free_queue)->next;
        list_del((&head_free_queue)->next);
        INIT_LIST_HEAD((struct list_head *) queue);
        queue->qlen = 0;
        ipl_restore(sp);
        queue->qlen = len;
        return queue;
}


void __init skb_init(void) {
        size_t i;

        for (i = 1; i < ARRAY_SIZE(sk_buff_pool); i++) {
                list_add((struct list_head *) &sk_buff_pool[i], &head_free_skb);
        }
        for (i = 1; i < ARRAY_SIZE(sk_queue_pool); i++) {
                list_add((struct list_head *) &sk_queue_pool[i], &head_free_queue);
        }
}


struct sk_buff *alloc_skb(unsigned int size, gfp_t priority) {
        struct sk_buff *skb;
        /*TODO only one packet now*/


        ipl_t sp;
        sp = ipl_save();
        if (list_empty(&head_free_skb)) {
                ipl_restore(sp);
                return NULL;
        }
        skb = (struct sk_buff *) (&head_free_skb)->next;
        list_del((&head_free_skb)->next);
        INIT_LIST_HEAD((struct list_head *) skb);
        ipl_restore(sp);


        if (NULL == (skb->data = net_buff_alloc())) {
                kfree_skb(skb);
                return NULL;
        }
        skb ->len = size;
        skb->mac.raw = (unsigned char *) skb->data;
        return skb;
}


void kfree_skb(struct sk_buff *skb) {
        ipl_t sp;
        if (NULL == skb) {
                return;
        }
        net_buff_free(skb->data);
        sp = ipl_save();
        if ((NULL == skb->prev) || (NULL == skb->next)) {
                list_add((struct list_head *) skb, (struct list_head *)&head_free_skb);
        } else {
                list_move_tail((struct list_head *) skb,
                                (struct list_head *) &head_free_skb);
        }
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
