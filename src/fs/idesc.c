/**
 * @file
 *
 * @date Oct 21, 2013
 * @author: Anton Bondarev
 */

#include <errno.h>
#include <string.h>

#include <util/dlist.h>

#include <kernel/task.h>
#include <kernel/task/idesc_table.h>
#include <fs/idesc.h>

int idesc_init(struct idesc *idesc, const struct idesc_ops *ops, idesc_access_mode_t amode) {

	memset(idesc, 0, sizeof(struct idesc));

	idesc->idesc_amode = amode;

	idesc->idesc_ops = ops;

	waitq_init(&idesc->idesc_waitq);

	return 0;
}

#if 0

int idesc_read(struct idesc *idesc, char *buff, int size) {
	return -ENOSUPP;
}

int idesc_write(struct idesc *idesc,char *buff, int size) {
	return -ENOSUPP;
}

int idesc_ctrl(struct idesc *idesc, int cmd, int type, void *buff, int size) {
	return -ENOSUPP;
}
#endif

int idesc_close(struct idesc *idesc, int fd) {
	struct idesc_table *it;

	it = task_get_idesc_table(task_self());
	assert(it);
	idesc_table_del(it, fd);

	return 0;
}

