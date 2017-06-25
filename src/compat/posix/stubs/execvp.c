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

int execvp(const char *file, char *const argv[]) {
	printk(">>> %s %s %s\n", __func__, file, argv[0]);
	return SET_ERRNO(ENOSYS);
}

