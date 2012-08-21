/**
 * @file
 * @brief Defenitions for priority queue data structure
 *
 * @date 25.10.11
 * @author Malkovsky Nikolay
 */

#ifndef PRIORITY_Q_
#define PRIORITY_Q_

/**
 * @brief Structure representing the queue itself
 */
struct priority_q {
	void **a;
	int capacity;
	int size;
	int(*cmp)(void*, void*);
};

#define CREATE_PRIORITY_QUEUE(name, pool_type, pool_size, compare) \
	static pool_type pool ## name[pool_size]; \
	static struct priority_q __pq ## name = { \
		.a = (void*)pool ## name, \
		.size = 0, \
		.capacity = pool_size, \
		.cmp = compare \
	}; \
	static struct priority_q* name = & __pq ## name;


/**
 * @brief Inserts specific value in queue priority_q
 *
 * @param priority_q queue to insert element into
 * @param value value to insert into queue
 */
extern void priority_q_insert(struct priority_q *pq, void* value);

/**
 * @brief pops the lowest element from the queue
 */
extern void priority_q_pop(struct priority_q *pq);

/**
 * @brief Returns the lowest element in the queue
 *
 * @param pq *priority_q to find the lowest element in
 * @return -1 when queue is empty, value of the lowest element in queue
 */
#define priority_q_front(pq) (pq->size == 0 ? NULL : pq->a[0])

/**
 * @brief Returns whether queue is empty or not
 */
#define priority_q_empty(pq) (pq->size == 0)

#endif /* PRIORITY_Q_ */
