#ifndef PJSIP_PTHREAD_H_
#define PJSIP_PTHREAD_H_

#include_next <pthread.h>

#include <sys/cdefs.h>

__BEGIN_DECLS

#undef PTHREAD_SCOPE_SYSTEM
#define PTHREAD_SCOPE_SYSTEM 0
#define PTHREAD_CANCELED 0

static inline int pthread_attr_setscope(pthread_attr_t *attr,
		int contentionscope) {
	(void)attr; (void)contentionscope;
	return 0;
}

__END_DECLS

#endif /* PJSIP_PTHREAD_H_ */

