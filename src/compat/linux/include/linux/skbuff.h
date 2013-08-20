/**
 * @file
 * @brief
 * @date 26.06.12
 * @author Ilia Vaprol
 */

#ifndef LINUX_SKBUFF_H_
#define LINUX_SKBUFF_H_

#include <net/skbuff.h>
#include <sys/types.h>

static inline struct sk_buff * alloc_skb(unsigned int size, gfp_t priority) {
	return skb_alloc(size);
}

static inline void kfree_skb(struct sk_buff *skb) {
	skb_free(skb);
}

#define skb_clone(skb, ...) skb_clone(skb)

#define skb_copy(skb, ...) skb_copy(skb)

static inline struct sk_buff * skb_peek(struct sk_buff_head *list) {
	return skb_queue_front(list);
}

static inline void skb_queue_tail(struct sk_buff_head *list, struct sk_buff *newsk) {
	skb_queue_push(list, newsk);
}

static inline struct sk_buff * skb_dequeue(struct sk_buff_head *list) {
	return skb_queue_pop(list);
}

#endif /* LINUX_SKBUFF_H_ */
