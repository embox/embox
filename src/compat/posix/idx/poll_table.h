/**
 * @file
 *
 * @date Nov 26, 2013
 * @author: Anton Bondarev
 */

#ifndef POLL_TABLE_H_
#define POLL_TABLE_H_

#include <framework/mod/options.h>
#include <fs/idesc_event.h>
#include <fs/poll_table.h>
#include <kernel/task/resource/idesc_table.h>

struct idesc;

struct idesc_poll {
	int fd;
	int i_poll_mask;
	int o_poll_mask;

	struct idesc_wait_link wait_link;
};

struct idesc_poll_table {
	struct idesc_poll idesc_poll[MODOPS_IDESC_TABLE_SIZE];
	int size;
};

extern int poll_table_count(struct idesc_poll_table *pt);
extern int poll_table_wait(struct idesc_poll_table *pt, clock_t ticks);

#endif /* POLL_TABLE_H_ */
