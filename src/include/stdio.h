/**
 * @file
 * @brief ISO C99 Standard: 7.19 Input/output
 * @details Contains related to "standard" input/output functions.
 *
 * @date 23.11.09
 * @author Nikolay Korotky
 * @author Eldar Abusalimov
 */

#ifndef STDIO_H_
#define STDIO_H_

#include <stdarg.h>
#include <types.h>
#include <kernel/printk.h>

#include <module/embox/lib/FileApi.h>

#define EOF (-1)

typedef struct file_struct {
	struct file_struct_int file_int;
	char has_ungetc;
	int ungetc;
} FILE;


struct stat;

__BEGIN_DECLS

extern int fputc(int c, FILE *f);
extern int fgetc(FILE *f);

#define getc(file) fgetc(file)

/**
 * Writes the string s and a trailing newline to stdout.
 */
extern int puts(const char *s);

#define fputs(s, stream) puts(s)

extern int putchar(int c);

/**
 * Read a line from stdin into the buffer pointed to by s until
 * either a terminating newline or EOF
 */
extern char *gets(char *s);

extern char *fgets(char *s, int n, FILE *stream);

extern int getchar(void);

extern int ungetc(int c, FILE *stream);

/* ungetc() for stdin */
int ungetchar(int ch);

/**
 * Write formatted output to stdout from the format string FORMAT.
 */
extern int printf(const char *format, ...);
extern void perror(const char *s);

#define fprintf(stream, ...) printf(__VA_ARGS__)

/**
 * Write formatted output into S, according to the format string FORMAT.
 */
extern int sprintf(char *s, const char *format, ...);

extern int vprintf(const char *format, va_list args);

extern int vsprintf(char *s, const char *format, va_list args);

/**
 * Read formatted input from stdin according to the format string FORMAT.
 */
extern int scanf(const char *format, ...);

extern int fscanf(FILE *stream, const char *format, ...);

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
 * Opens the file whose file descriptor is the fd
 * and associates a stream with it.
 */
extern FILE *fdopen(int fd, const char *mode);

/**
 * Opens the file whose name is the string pointed to by
 * path and associates the stream pointed to by stream with it
 */
extern FILE *freopen(const char *path, const char *mode, FILE *stream);

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

/**
 * Sets the file position indicator for the stream pointed to by stream.
 * The new position, measured in bytes, is obtained by adding offset bytes to
 * the position specified by whence.  If whence is set to SEEK_SET, SEEK_CUR,
 *  or SEEK_END, the offset is relative to the start of the file, the current
 *  position indicator, or end-of-file, respectively
 */
extern int fseek(FILE *stream, long int offset, int origin);

/**
 * Manipulate the underlying device parameters of special files.
 */
extern int fioctl(FILE *fp, int request, ...);

/**
 * Get statistic of file.
 */
extern int fstat(FILE *fp, void *buff);

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

//TODO: stub

extern void clearerr(FILE *stream);

extern int feof(FILE *stream);

extern int ferror(FILE *stream);

extern int fileno(FILE *stream);

__END_DECLS

#endif /* STDIO_H_ */
