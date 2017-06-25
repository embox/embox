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

int execlp(const char *file, const char *arg0,
		... /*, (char *)0 */) {
	printk(">>> %s %s %s\n", __func__, file, arg0);
	return SET_ERRNO(ENOSYS);
}

