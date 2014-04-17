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

int execl(const char *path, const char *arg0,
		... /*, (char *)0 */) {
	printk(">>> %s %s %s\n", __func__, path, arg0);
	return SET_ERRNO(ENOSYS);
}

