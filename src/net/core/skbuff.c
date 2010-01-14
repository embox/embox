/**
 * @file skbuff.c
 *
 * @details this module realize interface with structure sk_buff
 * structures sk_buff are presented queue free packet.
 * @date 20.10.2009
 * @author Anton Bondarev
 */
#include <types.h>
#include <string.h> /*for memcpy*/
#include <common.h>
#include <kernel/module.h>
#include <lib/list.h>
#include <net/skbuff.h>
#include <asm/spin_lock.h>
#include <net/net_pack_manager.h>

//TODO this define must place in autoconf
#define QUANTITY_SKB 0x100

//TODO this define must place in autoconf
#define QUANTITY_SKB_QUEUE 0x100

static struct sk_buff sk_buff_pool[QUANTITY_SKB];
static LIST_HEAD(head_free_skb);

static struct sk_buff_head sk_queue_pool[QUANTITY_SKB_QUEUE];
static LIST_HEAD(head_free_queue);

struct sk_buff_head *alloc_skb_queue(int len) {
	struct sk_buff_head *queue;
	unsigned long sp = spin_lock();
	if(list_empty(&head_free_queue)) {
		spin_unlock(sp);
		return NULL;
	}
	/* in free queue we held structure sk_buff_head but this pointer has sk_buff * type
	 * we must assignment it
	 */
	queue = (struct sk_buff_head *)(&head_free_queue)->next;
	list_del((&head_free_queue)->next);
	INIT_LIST_HEAD((struct list_head *)queue);
	spin_unlock(sp);
	queue->qlen = len;
	return queue;
}

void __init skb_init(void) {
	int i;
	/* Init net pack manager */
	net_buff_init();

	for (i = 1; i < array_len(sk_buff_pool); i ++) {
		list_add((struct list_head *)&sk_buff_pool[i], &head_free_skb);
	}
	for (i = 1; i < array_len(sk_queue_pool); i ++) {
		list_add((struct list_head *)&sk_queue_pool[i], &head_free_queue);
	}
}

struct sk_buff *alloc_skb(unsigned int size, gfp_t priority){
	struct sk_buff *skb;
	//TODO only one packet now

	unsigned long sp = spin_lock();
	if(list_empty(&head_free_skb)) {
		spin_unlock(sp);
		return NULL;
	}
	skb = (struct sk_buff *)(&head_free_skb)->next;
	list_del((&head_free_skb)->next);
	INIT_LIST_HEAD((struct list_head *)skb);
	spin_unlock(sp);

	if (NULL == (skb->data = net_buff_alloc())) {
		kfree_skb(skb);
		return NULL;
	}
	skb ->len = size;
	return skb;
}

void kfree_skb(struct sk_buff *skb) {
	unsigned long sp;
	if (NULL == skb) {
		return;
	}
	net_buff_free(skb->data);
	sp = spin_lock();
	list_move_tail((struct list_head *)skb, (struct list_head *)&head_free_skb);
	spin_unlock(sp);
}

void skb_queue_tail(struct sk_buff_head *list, struct sk_buff *newsk) {
	unsigned long sp;
	if (NULL == list || NULL == newsk) {
		return;
	}
	sp = spin_lock();
	list_move_tail((struct list_head *)newsk, (struct list_head *)list);
	spin_unlock(sp);
}

sk_buff_t *skb_peek(struct sk_buff_head *list_) {
        sk_buff_t *list = ((sk_buff_t *)list_)->next;
        if (list == (sk_buff_t *)list_) {
                list = NULL;
        }
        return list;
}

void skb_unlink(sk_buff_t *skb, struct sk_buff_head *list) {
        struct sk_buff *next, *prev;
        list->qlen--;
        next       = skb->next;
        prev       = skb->prev;
        skb->next  = skb->prev = NULL;
        next->prev = prev;
        prev->next = next;
}

sk_buff_t *skb_dequeue(struct sk_buff_head *list) {
	struct sk_buff *skb;
        unsigned long sp = spin_lock();
        skb = skb_peek(list);
        if (skb) {
        	skb_unlink(skb, list);
        }
        spin_unlock(sp);
        return skb;
}

void skb_queue_purge(struct sk_buff_head *queue) {
	struct list_head *skb;
	unsigned long sp;
	if (NULL == queue) {
    		return;
	}
        sp = spin_lock();
        /*free all sk_buff*/
        list_for_each(skb, (struct list_head *)queue) {
                net_buff_free(((struct sk_buff *)skb)->data);
                list_move_tail(skb, (struct list_head *)queue);
        }
        /*free queue*/
        list_add((struct list_head *)queue, &head_free_queue);
        spin_unlock(sp);
#if 0
	sk_buff_t *skb;
        while ((skb = skb_dequeue(queue)) != NULL) {
    		//FIXME: crash
                kfree_skb(skb);
        }
#endif
}

/**
 * split buffer to skb queue
 */
//TODO buff_to_skb not realize now
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
	memcpy (new_pack->data, skb->data, skb->len);

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
