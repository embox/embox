/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    16.04.2014
 */

#include <dirent.h>

#include <kernel/printk.h>

/* Weak: a VFS that can rewind (DVFS, dirent_dvfs.c) provides the real one;
 * the others keep this. */
__attribute__((weak)) void rewinddir(DIR *dirp) {
	printk("STUB >>> %s %p\n", __func__, dirp);
}
