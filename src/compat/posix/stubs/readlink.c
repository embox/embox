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

ssize_t readlink(const char *path, char *buf, size_t bufsize) {
	printk(">>> %s %s %s %zu\n", __func__, path, buf, bufsize);
	return SET_ERRNO(ENOSYS);
}

