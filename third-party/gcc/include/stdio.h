#ifndef GCC_STDIO_H_
#define GCC_STDIO_H_

#include_next <stdio.h>

#include <sys/cdefs.h>

__BEGIN_DECLS

extern FILE * tmpfile(void);
extern char * tmpnam(char *s);

__END_DECLS

#endif /* GCC_STDIO_H_ */

