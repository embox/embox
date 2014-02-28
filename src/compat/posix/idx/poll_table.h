/**
 * @file
 *
 * @date Nov 26, 2013
 * @author: Anton Bondarev
 */

#ifndef POLL_TABLE_H_
#define POLL_TABLE_H_

#include <framework/mod/options.h>
#include <fs/poll_table.h>
#include <kernel/task/resource/idesc_table.h>

struct idesc_poll_table {
	struct idesc_poll idesc_poll[MODOPS_IDESC_TABLE_SIZE];
	int size;
};

#endif /* POLL_TABLE_H_ */
