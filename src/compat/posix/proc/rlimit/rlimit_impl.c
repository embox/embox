/**
 * @file
 *
 * @date Sep 26, 2020
 * @author Anton Bondarev
 */
#include <errno.h>
#include <sys/resource.h>
#include <kernel/task.h>

int setrlimit(int resource, const struct rlimit *rlp) {
	int ret = -1;

	switch (resource) {
	case RLIMIT_STACK:
		task_setrlim_stack_size(task_self(), rlp->rlim_cur);
		ret = 0;
		break;
	default:
		errno = -ENOSYS;
		break;
	}

	return ret;
}

int getrlimit(int resource, struct rlimit *rlp) {
	int ret = -1;

	switch (resource) {
	case RLIMIT_STACK:
		ret = task_getrlim_stack_size(task_self());
		break;
	default:
		errno = -ENOSYS;
		break;
	}

	return ret;
}
