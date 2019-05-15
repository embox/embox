#ifndef GCC_STRING_H_
#define GCC_STRING_H_

#include <stddef.h>
#include <assert.h>
#include_next <string.h>

#include <sys/cdefs.h>

__BEGIN_DECLS

static inline int strcoll(const char *s1, const char *s2) {
	assert(0);
	return 0;
}

static inline size_t strxfrm(char *s1, const char *s2, size_t n) {
	assert(0);
	return 0;
}

__END_DECLS

#endif /* GCC_STRING_H_ */

