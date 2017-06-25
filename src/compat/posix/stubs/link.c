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

int link(const char *oldpath, const char *newpath) {
	printk(">>> %s %s %s\n", __func__, oldpath, newpath);
	return SET_ERRNO(ENOSYS);
}

