#ifndef GCC_STDLIB_H_
#define GCC_STDLIB_H_

#include_next <stdlib.h>

#include <sys/cdefs.h>

__BEGIN_DECLS

extern int atexit(void (*func)(void));

__END_DECLS

#endif /* GCC_STDLIB_H_ */

