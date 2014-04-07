#ifndef SQLITE_STDIO_H_
#define SQLITE_STDIO_H_

#include_next <stdio.h>

extern FILE * popen(const char *command, const char *mode);
extern int pclose(FILE *stream);

#endif /* SQLITE_STDIO_H_ */
