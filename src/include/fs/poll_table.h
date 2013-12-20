/**
 * @file
 *
 * @date Nov 26, 2013
 * @author: Anton Bondarev
 */

#ifndef POLL_TABLE_H_
#define POLL_TABLE_H_

struct idesc_poll_table;

extern int poll_table_count(struct idesc_poll_table *pt);

extern int poll_table_wait(struct idesc_poll_table *pt, clock_t ticks);


#include <fs/idesc_event.h>
#include <kernel/task/idesc_table.h>

struct idesc;
struct idesc_poll {
	int fd;
	/* struct idesc *idesc; */
	int i_poll_mask;
	int o_poll_mask;

	struct idesc_wait_link wait_link;
};

struct idesc_poll_table {
	struct idesc_poll idesc_poll[IDESC_QUANTITY];
	int size;
};


#endif /* POLL_TABLE_H_ */
