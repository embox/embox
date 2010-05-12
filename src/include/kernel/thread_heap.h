/**
 * @file
 * @brief Keeps threads in heap
 *
 * @date 11.05.2010
 * @author Avdyukhin Dmitry
 */

#ifndef THREAD_HEAP_H_
#define THREAD_HEAP_H_

#include <kernel/thread.h>

/**
 * Save thread in heap of threads.
 * @param thread saved thread
 */
void heap_insert(struct thread *thread);

/**
 * Delete thread from heap of threads.
 * @param thread deleted thread
 */
void heap_delete(struct thread *thread);

/**
 * Extract thread from heap of threads.
 */
struct thread *heap_extract(void);

/**
 * Check whether thread is in heap.
 * @param thread found thread.
 * @return true iff thread is in heap.
 */
bool heap_contains (struct thread *thread);

void heap_print(void);


#endif /* THREAD_HEAP_H_ */
