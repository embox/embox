#include <errno.h>
#include <kernel/printk.h>
#include <stdlib.h>

int atexit(void (*func)(void)) {
	printk(">>> %s %p\n", __func__, func);
	return 1;
}
