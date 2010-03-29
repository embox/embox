/**
 * @file
 *
 * @date 23.11.09
 * @author Nikolay Korotky
 * @author Eldar Abusalimov
 */
#ifndef STDIO_H_
#define STDIO_H_

#include <types.h>

#define EOF (-1)

typedef void FILE;

/**
 * Writes the string s and a trailing newline to stdout.
 */
extern int puts(const char *s);

extern int putchar(int c);

/**
 * Read a line from stdin into the buffer pointed to by s until
 * either a terminating newline or EOF
 */
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

/**
 * Open the file whose name is the string pointed to by path
 * and associates a stream with it.
 */
extern FILE *fopen(const char *path, const char *mode);

/**
 * Read nmemb elements of data, each size bytes long, from the stream
 * pointed to by stream, storing them at the location given by ptr.
 */
extern size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);

/**
 * Read nmemb elements of data, each size bytes long, from the stream
 * pointed to by stream, storing them at the location given by ptr.
 */
extern size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);

/**
 * Function will flushes the stream pointed to by fp (writing any buffered
 * output data using fflush(3)) and closes the underlying file descriptor.
 */
extern int fclose(FILE *fp);

/**
 * Deletes a name from the file system.
 */
extern int remove(const char *pathname);

#if defined(CONFIG_TRACE)
# ifdef __EMBOX__
#  define TRACE(...)  printf(__VA_ARGS__)
# else
#  define TRACE(...)  printk(__VA_ARGS__)
# endif
#else
# define TRACE(...)  do ; while(0)
#endif

#define PRINTREG32_BIN(reg) {int i=0; for(;i<32;i++) TRACE("%d", (reg>>i)&1); TRACE(" (0x%x)\n", reg);}

#endif /* STDIO_H_ */
