/**
 * @file
 * @brief
 *
 * @date 04.28.2017
 * @author  Anton Bondarev
 */


#include <errno.h>
#include <stdlib.h>

#include <kernel/printk.h>

void __atexit_funcs(void) {
}

int atexit(void (*func)(void)) {
	printk("STUB >>> %s %p\n", __func__, func);
	return -ENOSYS;
}
