/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    31.05.12
 */

#include <unistd.h>
#include <errno.h>
#include <kernel/task.h>

int isatty(int fd) {
	struct stat st;
#if 0
	struct idx_desc *desc = task_self_idx_get(fd);


	SET_ERRNO(ENOERR);

	if (!desc) {

		SET_ERRNO(EBADF);

		return 0;
	}
#endif

	if (-1 == fstat(fd, &st))  {
		return -1; /* errno already is set */
	}

	return S_ISCHR(st.st_mode);
}
