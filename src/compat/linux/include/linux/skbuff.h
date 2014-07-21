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

/**
 * Get the amount of bytes of free space at the head of skb
 */
static inline unsigned int skb_headroom(const struct sk_buff *skb) {
	assert(skb != NULL);
	return (void *)skb->mac.raw - skb_data_cast_in(skb->data);
}

/**
 * Get the amount of bytes of free space at the tail of skb
 */
static inline int skb_tailroom(const struct sk_buff *skb) {
	assert(skb != NULL);
	return skb_max_size() - skb->len;
}

/**
 * Reserve the @a len bytes at the head of skb
 */
static inline void skb_reserve(struct sk_buff *skb, int len) {
	assert(skb != NULL);
	assert(len <= skb_tailroom(skb));
	skb->mac.raw += len;
}

/**
 * Allocate @a len bytes at the tail of skb and return the pointer to it
 */
static inline unsigned char * skb_put(struct sk_buff *skb, unsigned int len) {
	unsigned char *tail;
	assert(skb != NULL);
	assert(len <= skb_tailroom(skb));
	tail = skb->mac.raw + skb->len;
	skb->len += len;
	return tail;
}

/**
 * Allocate @a len bytes at the head of skb and return the pointer to it.
 * Before allocating a few bytes you have to reserve them first by skb_reserve()
 */
static inline unsigned char * skb_push(struct sk_buff *skb, unsigned int len) {
	assert(skb != NULL);
	assert(len <= skb_headroom(skb));
	skb->len += len;
	return skb->mac.raw -= len;
}

/**
 * Remove data from the start of a buffer, returning the bytes to headroom.
 * A pointr to the next data in the buffer is returned.
 */
static inline unsigned char * skb_pull(struct sk_buff *skb, unsigned int len) {
	assert(skb != NULL);
	assert(len <= skb->len);
	skb->len -= len;
	return skb->mac.raw += len;
}

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
