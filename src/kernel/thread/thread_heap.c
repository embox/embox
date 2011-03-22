/**
 * @file
 * @brief Implementation of methods in thread_heap.h
 *
 * @date 11.05.2010
 * @author Avdyukhin Dmitry
 */

#include <kernel/thread/thread_heap.h>
#include <stdbool.h>

/** Maximal count of threads. */
#define MAX_THREAD_COUNT 0x400
/** Maximal value of priority. */
#define MAX_PRIORITY 256

/** Heap of executed threads. */
static struct thread *threads[MAX_THREAD_COUNT];

/**
 * For each priority there is a state.
 * We can start a thread iff his run_count equals to
 * priority_state of his priority or there is no threads
 * with the same priority and "right" run_count.
 * When there is no such threads we change priority_state of this priority
 */
static bool priority_state[MAX_PRIORITY];

/** Count of currently executed threads.  */
static int threads_count;

/**
 * return left descendant of the node;
 */
static inline int left(int x) {
	return (x << 1);
}

/**
 * return right descendant of the node;
 */
static inline int right(int x) {
	return (x << 1) + 1;
}

/**
 * Changes places of threads in heap
 *
 * @param i index of the first thread
 * @param j index of the second thread
 */
static void swap(int i, int j) {
	struct thread *another_thread = threads[i];
	threads[i] = threads[j];
	threads[j] = another_thread;
	threads[i]->heap_index = i;
	threads[j]->heap_index = j;
}

/**
 * Compares priorities of two threads.
 *
 * @param first first thread
 * @param second second thread
 * @return true if first thread's priority is greater then second one's
 * taking into consideration their run_states.
 * @return false otherwise.
 */
static inline bool greater(struct thread *first, struct thread *second) {
	return (first->priority > second->priority ||
		(first->priority == second->priority &&
		(first->run_count ^ second->run_count) &&
		first->run_count == priority_state[first->priority]));
}

/**
 * Restore heap's property by going up to the tree.
 * @param s the lowest unbalanced vertex.
 */
static void restore_up(int s) {
	threads[0] = threads[s];
	for (; s > 1; s /= 2) {
		if (greater(threads[0], threads[s / 2])) {
			threads[s] = threads[s / 2];
			threads[s]->heap_index = s;
		} else {
			break;
		}
	}
	threads[s] = threads[0];
	threads[s]->heap_index = s;
}

/**
 * Restore heap's property by going down to the tree.
 * @param s the highest unbalanced vertex.
 */
static void restore_down(int s) {
	int maxi;
	threads[0] = threads[s];
	for (; left(s) <= threads_count; s = maxi) {
		maxi = 0;
		if (greater(threads[left(s)], threads[0])) {
			maxi = left(s);
		}
		if (right(s) <= threads_count &&
		    greater(threads[right(s)], threads[maxi])) {
			maxi = right(s);
		}
		if (maxi != 0) {
			threads[s] = threads[maxi];
			threads[s]->heap_index = s;
		} else {
			break;
		}
	}
	threads[s] = threads[0];
	threads[s]->heap_index = s;
}

void heap_insert(struct thread *thread) {
	threads_count++;
	threads[threads_count] = thread;
	thread->heap_index = threads_count;
	restore_up(threads_count);
}

void heap_delete(struct thread *thread) {
	int index = thread->heap_index;
	swap(index, threads_count);
	threads_count--;
	if (index == threads_count + 1) {
		return;
	}
	if (index > 1 && greater(threads[index], threads[index / 2])) {
		restore_up(index);
	} else {
		restore_down(index);
	}
}

struct thread *heap_extract(void) {
	struct thread *result = threads[1];
	swap(1, threads_count);
	threads_count--;
	restore_down(1);
	result->run_count ^= 1;
	if (result->run_count == priority_state[result->priority]) {
		priority_state[result->priority] ^= 1;
	}
	return threads[threads_count + 1];
}

bool heap_contains (struct thread *thread) {
	return (thread->heap_index <= threads_count &&
		thread->heap_index > 0 && threads[thread->heap_index] == thread);
}

void heap_print(void) {
	size_t i;
	for (i = 1; i <= threads_count; i++) {
		TRACE("%d %d | ", threads[i]->id, threads[i]->priority);
	}
	TRACE("\n");
}
