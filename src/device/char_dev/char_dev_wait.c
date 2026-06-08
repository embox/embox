/**
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 05.06.26
 */

#include <errno.h>
#include <stddef.h>
#include <sys/poll.h>

#include <drivers/char_dev.h>
#include <kernel/sched/waitq.h>
#include <kernel/thread/thread_sched_wait.h>

int char_dev_wait(struct char_dev *cdev, int mask) {
	struct waitq_link wql;
	int err;

	waitq_link_init(&wql);
	waitq_wait_prepare(&cdev->waitq, &wql);

	err = SCHED_WAIT(cdev->ops->status(cdev, mask));

	waitq_wait_cleanup(&cdev->waitq, &wql);

	return err;
}

void char_dev_notify(struct char_dev *cdev) {
	waitq_wakeup(&cdev->waitq, 0);
}
