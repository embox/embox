#ifndef LUA_STRING_H_
#define LUA_STRING_H_

#include_next <string.h>

#include <sys/cdefs.h>

__BEGIN_DECLS

static inline int strcoll(const char *s1, const char *s2) {
	return strcmp(s1, s2);
}

__END_DECLS

#endif /* LUA_STRING_H_ */
