#include <errno.h>
#include <kernel/printk.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

int atexit(void (*func)(void)) {
	printk(">>> %s %p\n", __func__, func);
	return 1;
}

int setitimer(int which, const struct itimerval *value,
		struct itimerval *ovalue) {
	printk(">>> %s %d %p %p\n", __func__, which, value, ovalue);
	return SET_ERRNO(ENOSYS);
}

pid_t setsid(void) {
	printk(">>> %s\n", __func__);
	return SET_ERRNO(ENOSYS);
}
