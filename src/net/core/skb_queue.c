/**
 * @file
 * @details sk_buff_queue management API implementation.
 * @date 26.06.12
 * @author Ilia Vaprol
 */

#include <net/skbuff.h>
#include <hal/ipl.h>
#include <mem/misc/pool.h>
#include <assert.h>
#include <framework/mod/options.h>

#include <lib/list.h>




POOL_DEF(skb_queue_pool, struct sk_buff_head, OPTION_GET(NUMBER,skb_queue_quantity));

struct sk_buff_head * skb_queue_alloc(void) {
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

void skb_queue_free(struct sk_buff_head *queue) {
	ipl_t sp;
	struct sk_buff *skb;

	assert(queue != NULL);

	while ((skb = skb_queue_pop(queue)) != NULL) {
		skb_free(skb);
	}

	sp = ipl_save();
	pool_free(&skb_queue_pool, queue);
	ipl_restore(sp);
}

void skb_queue_push(struct sk_buff_head *queue, struct sk_buff *skb) {
	ipl_t sp;

	assert((queue != NULL) && (skb != NULL));

	sp = ipl_save();
	list_move_tail((struct list_head *)skb, (struct list_head *)queue);
	ipl_restore(sp);
}

struct sk_buff * skb_queue_front(struct sk_buff_head *queue) {
	struct sk_buff *skb;

	assert(queue != NULL);

	skb = queue->next;
	assert(skb != NULL);

	if (skb == (struct sk_buff *)queue) {
		return NULL;
	}

	return skb;
}

struct sk_buff * skb_queue_pop(struct sk_buff_head *queue) {
	ipl_t sp;
	struct sk_buff *skb;

	assert(queue != NULL);

	sp = ipl_save();

	skb = skb_queue_front(queue);
	if (skb != NULL) {
		list_del_init((struct list_head *)skb); // TODO use list_del
	}

	ipl_restore(sp);

	return skb;
}
