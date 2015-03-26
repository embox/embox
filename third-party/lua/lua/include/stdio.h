#ifndef LUA_STDIO_H_
#define LUA_STDIO_H_

#include_next <stdio.h>

#include <errno.h>
#include <stddef.h>

#include <sys/cdefs.h>

__BEGIN_DECLS

static inline FILE * tmpfile(void) {
	return SET_ERRNO(ENOSYS), NULL;
}

__END_DECLS

#endif /* LUA_STDIO_H_ */
