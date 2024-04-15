/**
 * @brief
 *
 * @date 12.02.24
 * @author Aleksey Zhmulin
 */

#include <sys/stat.h>
#include <sys/types.h>

#include <kernel/task/resource/umask.h>

mode_t umask(mode_t mode) {
	mode_t *umask_p = task_self_resource_umask();
	mode_t oldmode;

	oldmode = *umask_p;
	*umask_p = mode & S_IRWXA;
	return oldmode;
}
