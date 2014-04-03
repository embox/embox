/**
 * @file
 *
 * @date Aug 1, 2013
 * @author: Anton Bondarev
 */

#ifndef STRATEGY_THREAD_PRIORITY_H_
#define STRATEGY_THREAD_PRIORITY_H_



struct thread_priority {
	int base_priority;      /**< Priority was set for the thread. */
	int current_priority;   /**< Current thread scheduling priority. */
};

typedef struct thread_priority __thread_priority_t;

#endif /* STRATEGY_THREAD_PRIORITY_H_ */
