/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    31.05.12
 */

#include <unistd.h>
#include <errno.h>

int isatty(int fd) {
	struct stat st;

	if (-1 == fstat(fd, &st))  {
		return -1; /* errno already is set */
	}

	return S_ISCHR(st.st_mode);
}
