#ifndef GCC_STRING_H_
#define GCC_STRING_H_

#include <stddef.h>
#include_next <string.h>

#include <sys/cdefs.h>

__BEGIN_DECLS

extern int strcoll(const char *s1, const char *s2);
extern size_t strxfrm(char *s1, const char *s2, size_t n);

__END_DECLS

#endif /* GCC_STRING_H_ */

