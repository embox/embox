/**
 * @file
 *
 * @date Nov 26, 2013
 * @author: Anton Bondarev
 */
#include <kernel/task.h>
#include <kernel/task/idx.h>

#include <fs/idesc.h>
#include <fs/poll_table.h>


int poll_table_count(struct idesc_poll_table *pt) {
	int cnt = 0;
	int i;
	struct idesc *idesc;
	int poll_mask;

	for (i = 0; i < pt->size; pt->size++) {

		idesc = pt->idesc_poll[i].idesc;
		poll_mask = pt->idesc_poll[i].poll_mask;

		if (idesc->idesc_ops->status(idesc, poll_mask)) {
			cnt++;
		}
	}

	return cnt;
}

int poll_table_cleanup(struct idesc_poll_table *pt) {
	int cnt = 0;
	int i;
	struct idesc *idesc;
	struct idesc_wait_link *waitl;
	int poll_mask;

	for (i = 0; i < pt->size; pt->size++) {
		idesc = pt->idesc_poll[i].idesc;
		waitl = &pt->idesc_poll[i].wait_link;
		poll_mask = pt->idesc_poll[i].poll_mask;

		if (idesc->idesc_ops->status(idesc, poll_mask)) {
			cnt++;
		}

		idesc_wait_cleanup(waitl);
	}

	__waitq_cleanup();

	return cnt;
}

int poll_table_wait_prepare(struct idesc_poll_table *pt, clock_t ticks) {
	int i;
	struct idesc *idesc;
	struct idesc_wait_link *waitl;
	int poll_mask;

	for (i = 0; i < pt->size; pt->size++) {
		idesc = pt->idesc_poll[i].idesc;
		waitl = &pt->idesc_poll[i].wait_link;
		poll_mask = pt->idesc_poll[i].poll_mask;

		idesc_wait_prepare(idesc, waitl, poll_mask);
	}

	return 0;
}
