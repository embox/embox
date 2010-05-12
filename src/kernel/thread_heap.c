/**
 * @file
 * @brief Implementation of methods in thread_heap.h
 *
 * @date 11.05.2010
 * @author Avdyukhin Dmitry
 */

#include <kernel/thread_heap.h>

/** Maximal count of threads. */
#define MAX_THREAD_COUNT 1000

/** Heap of executed threads. */
static struct thread *threads[MAX_THREAD_COUNT];

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
 * @param i index of the first thread
 * @param j index of the second thread
 * @return true if first thread's priority is greater then second one's.
 * @return false otherwise.
 */
static inline bool greater(int i, int j) {
	return (threads[i]->priority > threads[j]->priority);
}

/**
 * Restore heap's property by going up to the tree.
 * @param s the lowest unbalanced vertex.
 */
static void restore_up(int s) {
	for (; s > 1; s /= 2) {
		if (greater(s, s / 2)) {
			swap(s, s / 2);
		} else {
			break;
		}
	}
}

/**
 * Restore heap's property by going down to the tree.
 * @param s the highest unbalanced vertex.
 */
static void restore_down(int s) {
	int maxi;
	for (; left(s) <= threads_count; s = maxi) {
		maxi = s;
		if (greater(left(s), maxi)) {
			maxi = left(maxi);
		}
		if (right(s) <= threads_count && greater(right(s), maxi)) {
			maxi = right(s);
		}
		if (maxi != s) {
			swap(s, maxi);
		} else {
			break;
		}
	}
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
	if (index > 1 && greater(index, index / 2)) {
		restore_up(index);
	} else {
		restore_down(index);
	}
}

struct thread *heap_extract(void) {
	swap(1, threads_count);
	threads_count--;
	restore_down(1);
	return threads[threads_count + 1];
}

bool heap_contains (struct thread *thread) {
	return (thread->heap_index <= threads_count &&
			thread->heap_index > 0 && threads[thread->heap_index] == thread);
}

void heap_print(void) {
	for (int i = 1; i <= threads_count; i++) {
		TRACE("%d %d | ", threads[i]->id, threads[i]->priority);
	}
	TRACE("\n");
}
