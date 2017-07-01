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

void rewinddir(DIR *dirp) {
	printk("STUB >>> %s %p\n", __func__, dirp);
}

