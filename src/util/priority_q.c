/**
 * @brief Implementation of priority queue data structure
 *
 * @date October 25, 2011
 * @author Malkovsky Nikolay
 */

#include <util/priority_q.h>

inline void insert(struct priority_q *pq, int value) {
	int temp;
	pq->a[pq->size++] = value;
	for(temp = pq->size - 1; (temp && pq->a[temp] < pq->a[(temp - 1) >> 1]); temp = (temp - 1) >> 1) {
		int c = pq->a[temp];
		pq->a[temp] = pq->a[(temp - 1) >> 1];
		pq->a[(temp - 1) >> 1] = c;
	}
}

inline void pop(struct priority_q *pq) {
	int temp;
	if(empty(pq)) {
		return;
	}

	pq->a[0] = pq->a[--pq->size];

	for(temp = 0; (((temp << 1) + 1 < pq->size && pq->a[temp] > pq->a[(temp << 1) + 1]) ||
		((temp << 1) + 2 < pq->size && pq->a[temp] > pq->a[(temp << 1) + 2])) ; ) {
		if((temp << 1) + 2 < pq->size) {
			if(pq->a[(temp << 1) + 1] < pq->a[(temp << 1) + 2]) {
				if(pq->a[temp] > pq->a[(temp << 1) + 1]) {
					int c = pq->a[temp];
					pq->a[temp] = pq->a[(temp << 1) + 1];
					pq->a[(temp << 1) + 1] = c;
					temp = (temp << 1) + 1;
				}
            }
            else {
            	if(pq->a[temp] > pq->a[(temp << 1) + 2]) {
            		int c = pq->a[temp];
            		pq->a[temp] = pq->a[(temp << 1) + 2];
            		pq->a[(temp << 1) + 2] =c;
            		temp = (temp << 1) + 2;
            	}
            }
		}
		else if((temp << 1) < pq->size) {
			if(pq->a[temp] > pq->a[(temp << 1) + 1]) {
				int c = pq->a[temp];
				pq->a[temp] = pq->a[(temp << 1) + 1];
				pq->a[(temp << 1) + 1] = c;
				temp <<= 1;
			}
       	}
    }
}
