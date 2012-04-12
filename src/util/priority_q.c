/**
 * @file
 * @brief Implementation of priority queue data structure
 *
 * @date 25.10.11
 * @author Malkovsky Nikolay
 *
 * @abstract
 * Priority queue implementaion is based on binary tree structure
 * with the variant of values of childs of any node being lower then
 * value of node itself.
 * So with that knowledge we can always guarantie, that the root node
 * contains element with the lowest value.
 *
 * =======================================================================
 *
 * About the structure.
 *
 * all the elements of the queue will be stored in the array. Moreover,
 * if the queue contains size elements, then they will have indexes from
 * 0 up to size - 1 inclusive. Root node will be indexed 0, for every node,
 * stored in cell indexed i, left child is indexed 2 * i + 1, right chiled is
 * indexed 2 * i + 2. From that, we can deduce that parent for node i is
 * indexed [(i - 1) / 2] ( as [a] i mean integer part of a ).
 *
 *                  0
 *               /     \
 *             1         2
 *           /   \     /   \
 *          3     4   5     6
 *
 * Insertion.
 *
 * When we insert the element in the queue, at first, we just place it into
 * cell with index size (current size of the queue). After that, we should
 * perform several swaps, so that the invariant will be valid. These swaps
 * are as follows: while value in the current node if lesser then the value
 * in it's parent, swap these values and perform the same with the parent node,
 * starting from the newly inserted node.
 *
 * Removing.
 *
 * So, we know, that the lowest value is stored in the root, and so, we are
 * erasing that one, and placing the value indexed size - 1 into the root
 * node. Again, we might have broken the invariant, now we will perform
 * swaps in the following way:
 * while value in current node is greater then any of value in chilren,
 * swap the value with the lower one, starting from the root node.
 */

#include <util/priority_q.h>

/**
 * Compares the values stored in cells c and b in queue pq
 */
#define LSS(pq, c, b) (pq->cmp((pq)->a[c], (pq)->a[b]))

#define SWAP(a, b) \
	do {                          \
		typeof(a) temp = (a); \
		a = b;                \
		b = temp;             \
	} while (0)

void priority_q_insert(struct priority_q *pq, void *value) {
	int idx;
	if(pq->size == pq->capacity) {
		return;
	}
	idx = pq->size++;
	pq->a[idx] = value;
	for ( ; idx && LSS(pq, idx, (idx - 1) >> 1); idx = (idx - 1) >> 1) {
		SWAP(pq->a[idx], pq->a[(idx - 1) >> 1]);
	}
}

void priority_q_pop(struct priority_q *pq) {
	int idx = 0;
	if (priority_q_empty(pq)) {
		return;
	}

	pq->a[0] = pq->a[--pq->size];

	for ( ; ((idx << 1) + 1 < pq->size && LSS(pq, (idx << 1) + 1, idx)) ||
	     ((idx << 1) + 2 < pq->size && LSS(pq, (idx << 1) + 2, idx)); ) {
		if ((idx << 1) + 2 < pq->size) {
			if (LSS(pq, (idx << 1) + 1, (idx << 1) + 2) &&
			    LSS(pq, (idx << 1) + 1, idx)) {
				SWAP(pq->a[idx], pq->a[(idx << 1) + 1]);
				idx = (idx << 1) + 1;
			} else if (LSS(pq, (idx << 1) + 2, idx)) {
				SWAP(pq->a[idx], pq->a[(idx << 1) + 2]);
				idx = (idx << 1) + 2;
			}
		} else if (idx << 1 < pq->size && LSS(pq, (idx << 1) + 1, idx)) {
			SWAP(pq->a[idx], pq->a[(idx << 1) + 1]);
			idx = (idx << 1) + 1;
		}
	}
}
