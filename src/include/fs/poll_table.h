/**
 * @file
 *
 * @date Nov 26, 2013
 * @author: Anton Bondarev
 */

#ifndef FS_POLL_TABLE_H_
#define FS_POLL_TABLE_H_

#include <fs/idesc_event.h>

struct idesc;
struct idesc_poll_table;

struct idesc_poll {
	int fd;
	int i_poll_mask;
	int o_poll_mask;

	struct idesc_wait_link wait_link;
};

extern int poll_table_count(struct idesc_poll_table *pt);
extern int poll_table_wait(struct idesc_poll_table *pt, clock_t ticks);

#endif /* FS_POLL_TABLE_H_ */
