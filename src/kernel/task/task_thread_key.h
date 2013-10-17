/**
 * @file
 *
 * @date Oct 3, 2013
 * @author: Anton Bondarev
 */

#ifndef TASK_THREAD_KEY_H_
#define TASK_THREAD_KEY_H_

#include <limits.h>

#include <framework/mod/options.h>
#include <util/indexator.h>
#include <kernel/thread/sync/mutex.h>

#define THREAD_KEYS_QUANTITY \
	OPTION_MODULE_GET(embox__kernel__task__task_thread_key, NUMBER, \
			keys_quantity)


struct thread_key_table {
	struct indexator indexator;
	char ind_buff[THREAD_KEYS_QUANTITY / CHAR_BIT + 1];
	void *destructor_table[THREAD_KEYS_QUANTITY];
	struct mutex mutex;
};
typedef struct thread_key_table __thread_key_table_t;

#endif /* TASK_THREAD_KEY_H_ */
