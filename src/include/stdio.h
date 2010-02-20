/**
 * @file
 *
 * @date 23.11.09
 * @author Nikolay Korotky
 * @author Eldar Abusalimov
 */

#ifndef STDIO_H_
#define STDIO_H_

#define EOF (-1)

extern int puts(const char *s);
extern int putchar(int c);

extern char *gets(char *s);
extern int getchar(void);

/**
 * Write formatted output to stdout from the format string FORMAT.
 */
extern int printf(const char *format, ...);

/**
 * Write formatted output into S, according to the format string FORMAT.
 */
extern int sprintf(char *s, const char *format, ...);

/**
 * Read formatted input from stdin according to the format string FORMAT.
 */
extern int scanf(const char *format, ...);

/**
 * Read formatted input from S, according to the format string FORMAT.
 */
extern int sscanf(char *out, const char *format, ...);

#endif /* STDIO_H_ */
