/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    09.06.2014
 */

#include <kernel/task/resource/umask.h>

#include <sys/stat.h>

mode_t umask(mode_t mode) {
	mode_t *umask_p = task_self_resource_umask();
	mode_t oldmode;

	oldmode = *umask_p;
	*umask_p = mode & 0777;
	return oldmode;
}

mode_t umask_modify(mode_t newmode) {
	return newmode & ~(*task_self_resource_umask());
}
