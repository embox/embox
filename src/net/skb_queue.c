/**
 * @file
 * @details sk_buff_queue management API implementation.
 *
 * @date 26.06.12
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <framework/mod/options.h>
#include <hal/ipl.h>
#include <linux/list.h>
#include <mem/misc/pool.h>
#include <net/skbuff.h>

#define MODOPS_AMOUNT_SKB_QUEUE OPTION_GET(NUMBER, amount_skb_queue)

POOL_DEF(skb_queue_pool, struct sk_buff_head, MODOPS_AMOUNT_SKB_QUEUE);

struct sk_buff_head * skb_queue_alloc(void) {
	ipl_t sp;
	struct sk_buff_head *queue;

	sp = ipl_save();
	{
		queue = (struct sk_buff_head *)pool_alloc(&skb_queue_pool);
	}
	ipl_restore(sp);

	if (queue == NULL) {
		return NULL; /* error: no memory */
	}

	skb_queue_init(queue);

	return queue;
}

void skb_queue_init(struct sk_buff_head *queue) {
	if (queue == NULL) {
		return; /* error: invalid argument */
	}

	INIT_LIST_HEAD((struct sk_buff *)queue);
}

void skb_queue_free(struct sk_buff_head *queue) {
	ipl_t sp;
	struct sk_buff *skb;

	while ((skb = skb_queue_pop(queue)) != NULL) {
		skb_free(skb);
	}

	sp = ipl_save();
	{
		pool_free(&skb_queue_pool, queue);
	}
	ipl_restore(sp);
}

void skb_queue_push(struct sk_buff_head *queue, struct sk_buff *skb) {
	ipl_t sp;

	if ((queue == NULL) || (skb == NULL)) {
		return; /* error: invalid arguments */
	}

	sp = ipl_save();
	{
		list_move_tail((struct list_head *)skb, (struct list_head *)queue);
	}
	ipl_restore(sp);
}

struct sk_buff * skb_queue_front(struct sk_buff_head *queue) {
	struct sk_buff *skb;

	if (queue == NULL) {
		return NULL; /* error: invalid argument */
	}

	skb = queue->next;
	assert(skb != NULL);

	if (skb == (struct sk_buff *)queue) {
		return NULL; /* error: there is no more skb */
	}

	return skb;
}

struct sk_buff * skb_queue_pop(struct sk_buff_head *queue) {
	ipl_t sp;
	struct sk_buff *skb;

	if (queue == NULL) {
		return NULL; /* error: invalid argument */
	}

	sp = ipl_save();
	{
		skb = skb_queue_front(queue);
		if (skb != NULL) {
			list_del_init((struct list_head *)skb);
		}
	}
	ipl_restore(sp);

	return skb;
}
