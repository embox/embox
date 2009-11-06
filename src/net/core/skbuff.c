/**
 * \file skbuff.c
 *
 * \date 20.10.2009
 * \author Anton Bondarev
 * \details this module realize interface with structure sk_buff
 * structures sk_buff are presented queue free packet.
 */
#include "asm/types.h"
#include "common.h"
#include "kernel/module.h"
#include "lib/list.h"
#include "net/skbuff.h"
#include "asm/spin_lock.h"
#include "net/net_packet.h"

//TODO this define must place in autoconf
#define QUANTITY_SKB 0x100



//TODO this define must place in autoconf
#define QUANTITY_SKB_QUEUE 0x100

static struct sk_buff sk_buff_pool[QUANTITY_SKB];
static LIST_HEAD(head_free_skb);

static struct sk_buff_head sk_queue_pool[QUANTITY_SKB_QUEUE];
static LIST_HEAD(head_free_queue);

struct sk_buff_head *alloc_skb_queue(){
	struct sk_buff_head *queue;
	unsigned long sp = spin_lock();
	if(list_empty(&head_free_queue)){
		spin_unlock(sp);
		return NULL;
	}
	/* in free queue we held structure sk_buff_head but this pointer has sk_buff * type
	 * we must assignment it
	 */
	queue = (struct sk_buff_head *)(&head_free_queue)->next;
	list_del((&head_free_queue)->next);
	spin_unlock(sp);
	return queue;
}

/**
 * function must called if we want use this functionality.
 * It init queue free packet
 */
static void __init init_skb(){
	int i;
	for (i = 1; i < array_len(sk_buff_pool); i ++){
		kfree_skb(&sk_buff_pool[i]);
	}
	for (i = 1; i < array_len(sk_queue_pool); i ++){
		list_add((struct list_head *)&sk_queue_pool[i], &head_free_queue);
	}
}

/**
 * allocate one instance of structure sk_buff. With pointed size and flags.
 * @return If size is more then mtu (now it is defined by macros ETHERNET_V2_FRAME_SIZE)
 * function will return queue of sk_buff. Else function return single sk_buff.
 * Function return NULL if function can't allocate demanded buffer
 */
struct sk_buff *alloc_skb(unsigned int size, gfp_t priority){
	struct sk_buff *skb;
//TODO only one packet now

	unsigned long sp = spin_lock();
	if(list_empty(&head_free_skb)){
		spin_unlock(sp);
		return NULL;
	}
	skb = (struct sk_buff *)(&head_free_skb)->next;
	list_del((&head_free_skb)->next);
	spin_unlock(sp);

	if (NULL == (skb->data = net_buff_alloc())) {
		kfree_skb(skb);
		return NULL;
	}
	skb ->len = size;
	return skb;
}

void kfree_skb(struct sk_buff *skb){
	if (NULL == skb){
		return;
	}
	net_buff_free(skb->data);
	unsigned long sp = spin_lock();
	list_move_tail((struct list_head *)skb, (struct list_head *)&head_free_skb);
	spin_unlock(sp);
}

void skb_queue_tail(struct sk_buff_head *list, struct sk_buff *newsk){
	if (NULL == list || NULL == newsk)
		return;

	unsigned long sp = spin_lock();
	list_move_tail((struct list_head *)newsk, (struct list_head *)list);
	spin_unlock(sp);

}
/**
 * return head of queue allocated in alloc_skb_queue to the free queue pool
 * and give back all contained sk_buff in this list
 */
void skb_queue_purge(struct sk_buff_head *queue){
	if (NULL == queue){
		return;
	}
	struct list_head *skb;
	unsigned long sp = spin_lock();
	/*free all sk_buff*/
	list_for_each(skb, (struct list_head *)queue){
		net_buff_free(((struct sk_buff *)skb)->data);
		list_move_tail(skb, (struct list_head *)queue);
	}
	/*free queue*/
	list_add((struct list_head *)queue, &head_free_queue);
	spin_unlock(sp);
}

/**
 * split buffer to skb queue
 *
 */
//TODO buff_to_skb not realize now
struct sk_buff *buff_to_skb(unsigned char *buff){
	return NULL;
}

struct sk_buff *alloc_skb_fclone(struct sk_buff *skb, gfp_t priority) {

}

struct sk_buff *skb_copy(const struct sk_buff *skb, gfp_t priority){
	struct sk_buff *new_pack;
	if ((NULL == skb) || (NULL == (new_pack = alloc_skb(skb->len, priority)))){
		return NULL;
	}
	memcpy (skb->data, new_pack->data, skb->len);

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
