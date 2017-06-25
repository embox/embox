/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    16.04.2014
 */

#include <dirent.h>
#include <errno.h>
#include <kernel/printk.h>
#include <stdio.h>
#include <sys/types.h>
#include <utime.h>

int utime(const char *path,
		const struct utimbuf *times) {
	printk(">>> %s %s %p\n", __func__, path, times);
	return SET_ERRNO(ENOSYS);
}

