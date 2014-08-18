#ifndef LUA_TIME_H_
#define LUA_TIME_H_

#include_next <time.h>

#include <sys/cdefs.h>

__BEGIN_DECLS

static inline double difftime(time_t time1, time_t time0) {
	return (double)(time1 - time0);
}

__END_DECLS

#endif /* LUA_TIME_H_ */
