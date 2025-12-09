/**
 * @file
 *
 * @date Jul 18, 2022
 * @author Anton Bondarev
 */

#include <errno.h>
#include <pthread.h>

int pthread_attr_setstackaddr(pthread_attr_t *attr, void *stackaddr) {
	return -ENOSYS;
}

int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize) {
	return -ENOSYS;
}

int pthread_attr_setstack(pthread_attr_t *attr, void *stackaddr, size_t stacksize) {
	return -ENOSYS;
}

int pthread_attr_getstackaddr(const pthread_attr_t *attr, void **stackaddr) {
	*stackaddr = attr->stack;

	return 0;
}

int pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize) {
	*stacksize = attr->stack_size;
	return 0;
}

int pthread_attr_getstack(pthread_attr_t *attr,
                          void **stackaddr, size_t *stacksize) {
	*stackaddr = attr->stack;
	*stacksize = attr->stack_size;

	return 0;
}
