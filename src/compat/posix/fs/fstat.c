/**
 * @file
 *
 * @date Nov 15, 2013
 * @author: Anton Bondarev
 */
#include <assert.h>
#include <stddef.h>
#include <sys/stat.h>
#include <errno.h>


#include <kernel/task.h>
#include <kernel/task/idx.h>

#include <fs/index_descriptor.h>

#include <fs/idesc.h>

int fstat(int fd, struct stat *buff) {
	const struct idesc_ops *ops;

#ifndef IDESC_TABLE_USE
	int rc;
	struct idx_desc *desc;


	assert(task_self_idx_table());

	desc = task_self_idx_get(fd);
	if (!desc) {
		SET_ERRNO(EBADF);
		rc = -1;
		goto end;
	}

	ops = task_idx_desc_ops(desc);
	assert(ops);
	if(NULL != ops->fstat) {
		rc = ops->fstat(desc, buff);
	}
	else {
		rc = -1;
	}

	return rc;
#else
	struct idesc *desc;
	desc = index_descriptor_get(fd);
	assert(desc);

	ops = desc->idesc_ops;
	return ops->fstat(desc, buff);
#endif

}
