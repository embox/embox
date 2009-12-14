/**
 * @file stdio.h
 *
 * @date 23.11.09
 * @author Nikolay Korotky
 */
#ifndef _STDIO_H_
#define _STDIO_H_

#include "types.h"

/**
 * Write formatted output to stdout from the format string FORMAT.
 */
extern int printf (const char *format, ...);

/**
 * Write formatted output into S, according to the format string FORMAT.
 */
extern int sprintf(char *s, const char *format, ...);

/**
 * Read formatted input from stdin according to the format string FORMAT.
 */
extern int scanf (const char *format, ...);

/**
 * Read formatted input from S, according to the format string FORMAT.
 */
extern int sscanf (char *out, const char *format, ...);

#endif /* _STDIO_H_ */
