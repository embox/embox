/**
 * \file skbuff.c
 *
 * \date 20.10.2009
 * \author Anton Bondarev
 * \details this module releaze interface with struct sk_buff
 * structs sk_buff are presented queue free packet.
 */
#include "types.h"
#include "common.h"
#include "skbuff.h"
//TODO this define must place in autoconf
#define QUANTITY_SKB 0x100


static struct sk_buff sk_buff_pool[QUANTITY_SKB];
#if 0
static struct sk_buff *head_free_skb = sk_buff_pool;
static struct sk_buff *tail_free_skb = sk_buff_pool;
#endif

static sk_buff_queue free_skb_pool = {sk_buff_pool, sk_buff_pool};
//TODO define _init section in link script
#define _init
/**
 * function must called if we want use this functionality.
 * It init queue free packet
 */
void _init init_skb(){
	int i;
	for (i = 1; i < array_len(sk_buff_pool);i ++){
		kfree_skb(&sk_buff_pool[i]);
	}
}

/**
 * alloc one instance of struct sk_buff. With pointed size and flags.
 * @return If size is more then mtu (now it is defined by macros ETHERNET_V2_FRAME_SIZE)
 * function will return queue of sk_buff. Else function return single sk_buff.
 * Function return NULL if function can't alloc demanded buffer
 */
struct sk_buff *alloc_skb(unsigned int size, gfp_t priority){
	struct sk_buff *skb;
//TODO only one packet now
#if 0
	unsigned long sp = spin_lock();
	/*if head_free_skb == tail_free_skb queue is empty */
	if (NULL == head_free_skb->prev){
		spin_unlock(sp);
		return NULL;
	}
	skb = head_free_skb;
	head_free_skb = head_free_skb->prev;

	spin_unlock(sp);
#endif
	if (NULL == (skb = get)
	if (NULL == (new_skb->data = net_buff_alloc())) {
		kfree_skb(skb);
		return NULL;
	}

	skb -> next = NULL;
	skb -> prev = NULL;
	skb ->len = size;

	return skb;
}

void kfree_skb(struct sk_buff *skb){
	struct sk_buff *cur_skb;
	if (NULL == skb){
		return;
	}
	unsigned long sp = spin_lock();
	tail_free_skb -> prev = skb;
	skb -> next = tail_free_skb;
	tail_free_skb = skb;
	tail_free_skb -> prev = NULL;
	spin_unlock(sp);
}

struct sk_buff_queue *add_skb_to_tail(struct sk_buff_queue *queue, struct sk_buff *skb){
	if (NULL == queue || NULL = skb)
		return NULL;

	unsigned long sp = spin_lock();
	if (NULL == queue->tail){
		queue->tail = queue->tail = skb;
		spin_unlock(sp);
		return queue;
	}

	queue->tail->prev = skb;
	skb->next = tail;
	queue->tail = skb;

	spin_unlock(sp);

	return queue;
}

struct sk_buff_queue *add_skb_to_head(struct sk_buff_queue *queue, struct sk_buff *skb){
	if (NULL == queue || NULL = skb)
		return NULL;

	unsigned long sp = spin_lock();
	if (NULL == queue->tail){
		queue->tail = queue->tail = skb;
		spin_unlock(sp);
		return queue;
	}

	queue->head->next = skb;
	skb->prev = tail;
	queue->head = skb;

	spin_unlock(sp);

	return queue;
}

#if 0
void remove_skb(struct sk_buff_queue *queue, struct sk_buff *skb){
	if (NULL == queue || NULL = skb)
		return ;

	unsigned long sp = spin_lock(sp);
	if (NULL == skb->prev && NULL != skb->next){
		queue->tail = queue->head = NULL;
		spin_unlock(sp);
		return;
	}
	if (NULL != skb->prev){
		skb->prev->next = skb->next;
	}
	else {
		queue->tail = NULL;
	}
	if (NULL != skb->next){
		skb->next->prev = skb->prev;
	}
	else {
		queue->tail = NULL;
	}
	spin_unlock(sp);
}
#endif

struct sk_buff *get_skb_from_head(struct sk_buff_queue *queue){
	struct sk_buff *skb;

	if (NULL == queue){
		return NULL;
	}
	unsigned long sp = spin_lock(sp);

	if (NULL == (skb = queue->head)){
		return NULL;
	}

	if (NULL == skb->prev){
		queue->tail = queue->head = NULL;
	} else {
		skb->prev->next = NULL;
		queue->head = skb->prev;
	}
	/*return skb to free_pool*/
	add_skb_to_head(free_skb_pool, skb);

	spin_unlock(sp);
	return skb;
}
/**
 * for iteration on sk_buff_queue
 * first call with queue->head
 * @param poiner to curent sk_buff
 * @return poiner to prev sk_buff in queue
 */
struct sk_buff *get_skb_prev(struct sk_buff *skb){
	return skb->prev;
}

void drop_skb_queue(struct sk_buff_queue *queue){
	if (NULL == queue){
		return;
	}
	unsigned long sp = spin_lock(sp);
	struct sk_buff *skb = queue->head;
	if (NULL == skb)
		return;
	while(NULL == (skb = get_skb_prev (skb))){
		add_skb_to_head(free_skb_pool, skb);
	}
	spin_unlock(sp);
}
/**
 * split buffer to skb queue
 *
 */
sk_buff_queue *buff_to_skb(unsigned char *buff){
	return NULL;
}

struct sk_buff *alloc_skb_fclone(struct sk_buff *skb, gfp_t priority) {
	struct sk_buff *new_skb;
	if ((NULL == skb) || (NULL == (new_skb = alloc_skb(skb->len, priority)))){
		return NULL;
	}
	memcpy (skb->data, new_skb->data, skb->len);

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

