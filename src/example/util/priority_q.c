/**
 * @file
 * @brief Exapmle for usage of priority queue
 *
 * @author Nikoaly Malkovsky
 * @date 12.04.12
 */

#include <framework/example/self.h>
#include <util/priority_q.h>
#include <stdio.h>

EMBOX_EXAMPLE(priority_q_example);

/*
 * For proper working of priority queue we need compare function.
 * note that comparation have to be transitive: that is if a < b and b < c then a < c
 * compare function should return 1 if first parameter compares less then second one
 * 0 otherwise
 *
 * priority_q contains values of type void*,
 * in this case we will just store integer numbers, that can be directly cast to void*
 */

int _cmp(void* a, void* b) {
	return (int)a < (int)b;
}

static int priority_q_example(int argc, char **argv) {

	/*
	 * Creating queue pq
	 * that can store a maximum of 4 int's
	 * comparing them with cmp
	 */
	CREATE_PRIORITY_QUEUE(pq, int, 4, _cmp);

	/*
	 * Puttin some values into queue
	 * note that values should have type of void*
	 */

	priority_q_insert(pq, (void*)6);
	priority_q_insert(pq, (void*)5);
	priority_q_insert(pq, (void*)10);

	/*
	 * retrieving values in acsending order
	 * note that priority_q_front has type of void*
	 */
	printf("%d\n", (int)priority_q_front(pq));
	priority_q_pop(pq);
	printf("%d\n", (int)priority_q_front(pq));
	priority_q_pop(pq);
	printf("%d\n", (int)priority_q_front(pq));
	priority_q_pop(pq);

	return 0;
}
