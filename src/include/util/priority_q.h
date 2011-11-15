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
	 /* TODO replace static array for a vector or something */
	int a[32];
	int size;
};

/**
 * @brief Inserts specific value in queue priority_q
 *
 * @param priority_q queue to insert element into
 * @param value value to insert into queue
 */
extern void insert(struct priority_q *pq, int value);

/**
 * @brief pops the lowest element from the queue
 */
extern void pop(struct priority_q *pq);

/**
 * @brief Returns the lowest element in the queue
 *
 * @param pq *priority_q to find the lowest element in
 * @return -1 when queue is empty, value of the lowest element in queue
 */
/* TODO better implementatoin in case when queue is empty */
#define front(pq) (pq->size == 0 ? -1 : pq->a[0])

/**
 * @brief Returns whether queue is empty or not
 */
#define empty(pq) (pq->size == 0)

#endif /* PRIORITY_Q_ */
