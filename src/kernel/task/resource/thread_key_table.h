/**
 * @file
 *
 * @date Oct 3, 2013
 * @author: Anton Bondarev
 */

#ifndef THREAD_KEY_TABLE_H_
#define THREAD_KEY_TABLE_H_

#include <kernel/task/resource/thread_key_table.h>
#include <kernel/thread/sync/mutex.h>
#include <kernel/thread/thread_local.h>
#include <lib/libds/indexator.h>

struct thread_key_table {
	struct indexator indexator;
	index_data_t index_buffer[INDEX_DATA_LEN(MODOPS_THREAD_KEY_QUANTITY)];
	struct mutex mutex;
};

#endif /* THREAD_KEY_TABLE_H_ */
