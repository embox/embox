/**
 * @file
 * @details sk_buff_queue management API implementation.
 *
 * @date 26.06.12
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <hal/ipl.h>
#include <linux/list.h>
#include <net/skbuff.h>

void skb_queue_init(struct sk_buff_head *queue) {
	if (queue == NULL) {
		return; /* error: invalid argument */
	}

	queue->next = (struct sk_buff *)queue;
	queue->prev = (struct sk_buff *)queue;
	//INIT_LIST_HEAD(((struct sk_buff *)queue->lnk));
}

void skb_queue_purge(struct sk_buff_head *queue) {
	struct sk_buff *skb;

	while ((skb = skb_queue_pop(queue)) != NULL) {
		skb_free(skb);
	}
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
