/**
 * @file
 *
 * @brief
 *
 * @date 19.03.2012
 * @author Anton Bondarev
 */

#ifndef DLIST_H_
#define DLIST_H_

struct dlist_head {
	struct dlist_head *next;
	struct dlist_head *prev;
	struct dlist_head *list_id;
};

#define DLIST_INIT(head) { &(head), &(head) }

#define DLIST_DEFINE(name) \
		struct dlist_head = DLIST_INIT()

/* only for internal using */
static inline __dlist_add(struct dlist_head *new, struct dlist_head *next, struct dlist_head *prev) {
	new->prev = prev;
	new->next = next;
	next->prev = new;
	prev->next = new;
}
static inline int __is_linked(struct dlist_head *head) {
	return (NULL == head->list_id);
}

static inline void dlist_add_next(struct dlist_head *new, struct dlist_head *list) {
	if(__is_linked(new)) {
		return;// assert
	}
	if(!__is_linked(list)) {
		list->list_id = list;
		new->list_id = list;
	} else {
		new ->list_id = list->list_id;
	}

	__dlist_add(new, list->next, list);
}

static inline void dlist_add_prev(struct dlist_head *new, struct dlist_head *list) {
	if(__is_linked(new)) {
		return;// assert
	}

	if(!__is_linked(list)) {
		list->list_id = list;
		new->list_id = list;
	} else {
		new ->list_id = list->list_id;
	}

	__dlist_add(new, list, list->prev);
}

static inline void dlist_del(struct dlist_head *head) {
	if(!__is_linked(head)) {
		return; //assert
	}
	head->prev->next = head->next;
	head->next->prev = head->prev;
	head->list_id = NULL;
}

static inline int dlist_empty(struct dlist_head *head) {
	return head == head->next;
}

#define dlist_entry(p, type, member) \
    ((type *)((char *)(p) - (unsigned long)(&((type *)0)->member)))

#define dlist_foreach(ptr, nxt, head) \
	for (ptr = (head)->next, nxt = ptr->next; ptr != (head); \
		ptr = nxt, nxt = ptr->next)

#endif /* DLIST_H_ */
