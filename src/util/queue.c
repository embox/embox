/**
 * @file
 * @brief An implementation of 'queue' interface
 *
 * @date 20.02.12
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <errno.h>
#include <framework/mod/options.h>
#include <mem/objalloc.h>
#include <util/list.h>
#include <util/queue.h>

#define MODOPS_QUEUE_QUANTITY         OPTION_GET(NUMBER, queue_quantity)
#define MODOPS_QUEUE_ELEMENT_QUANTITY OPTION_GET(NUMBER, queue_element_quantity)

struct queue_element {
	struct list_link lnk;
	void *val;
};

struct queue {
	struct list head;
	size_t size;
};

OBJALLOC_DEF(queue_pool, struct queue, MODOPS_QUEUE_QUANTITY);
OBJALLOC_DEF(queue_element_pool, struct queue_element, MODOPS_QUEUE_ELEMENT_QUANTITY);

struct queue * queue_create(void) {
	struct queue *q;

	q = objalloc(&queue_pool);
	if (q == NULL) {
		return NULL;
	}

	list_init(&q->head);
	q->size = 0;

	return q;
}

int queue_empty(struct queue *q) {
	assert(q != NULL);
	return list_is_empty(&q->head);
}

size_t queue_size(struct queue *q) {
	assert(q != NULL);
	return q->size;
}

void * queue_front(struct queue *q) {
	struct queue_element *qe;

	assert(q != NULL);
	assert(!queue_empty(q));

	qe = list_first_element(&q->head, struct queue_element, lnk);
	assert(qe != NULL);

	return qe->val;
}

void * queue_back(struct queue *q) {
	struct queue_element *qe;

	assert(q != NULL);
	assert(!queue_empty(q));

	qe = list_last_element(&q->head, struct queue_element, lnk);
	assert(qe != NULL);

	return qe->val;
}

int queue_push(struct queue *q, void *val) {
	struct queue_element *qe;

	assert(q != NULL);
	qe = objalloc(&queue_element_pool);
	if (qe == NULL) {
		return -ENOMEM;
	}

	list_link_init(&qe->lnk);
	qe->val = val;

	list_add_last_element(qe, &q->head, lnk);
	++q->size;

	return ENOERR;
}

void queue_pop(struct queue *q) {
	struct queue_element *qe;

	assert(q != NULL);
	assert(!queue_empty(q));

	qe = list_first_element(&q->head, struct queue_element, lnk);
	assert(qe != NULL);

	list_unlink_element(qe, lnk);

	objfree(&queue_element_pool, qe);

	--q->size;
}

void queue_destroy(struct queue *q) {
	assert(q != NULL);

	while (!queue_empty(q)) {
		queue_pop(q);
	}

	objfree(&queue_pool, q);
}
