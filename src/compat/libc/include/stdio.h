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

/*va_list As described in <stdarg.h>. */
#include <stdarg.h>
#include <sys/types.h>

#include <defines/size_t.h>
#include <defines/null.h>


#define EOF (-1)

#define _IOFBF        0x20
#define _IOLBF        0x20
#define _IONBF        0x20
#define BUFSIZ        0x100
#define L_tmpnam      0x20
#define FILENAME_MAX  0x20

#define P_tmpdir   "/tmp"


/* Values for the WHENCE argument to lseek. */
#define SEEK_SET        0   /* Seek from beginning of file. */
#define SEEK_CUR        1   /* Seek from current position. */
#define SEEK_END        2   /* Seek from end of file. */


typedef struct file_struct {
	int fd;
	char has_ungetc;
	int ungetc;
} FILE;


struct stat;

#include <sys/cdefs.h>

__BEGIN_DECLS

extern int putc(int c, FILE *f);
extern int fputc(int c, FILE *f);

extern int getc(FILE *f);
extern int fgetc(FILE *f);

/**
 * Writes the string s and a trailing newline to stdout.
 */
extern int puts(const char *s);
extern int fputs(const char *s, FILE *f);

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

extern void perror(const char *s);

/**
 * Write formatted output to stdout from the format string FORMAT.
 */
extern int printf(const char *format, ...);

extern int vprintf(const char *format, va_list args);

/**
 * Write formatted output to file stream from the format string FORMAT.
 */
extern int fprintf(FILE *f, const char *format, ...);

extern int vfprintf(FILE *f, const char *format, va_list args);

/**
 * Write formatted output to string, according to the format string FORMAT.
 */
extern int sprintf(char *s, const char *format, ...);

extern int vsprintf(char *s, const char *format, va_list args);

/**
 * Write formatted output to string with specific size, according to the format string FORMAT.
 */
extern int snprintf(char *s, size_t n, const char *format, ...);

extern int vsnprintf(char *s, size_t n, const char *format, va_list args);

/**
 * Read formatted input from stdin according to the format string FORMAT.
 */
extern int scanf(const char *format, ...);

extern int fscanf(FILE *stream, const char *format, ...);

/**
 * Read formatted input from S, according to the format string FORMAT.
 */
extern int sscanf(const char *out, const char *format, ...);

/**
 * Open the file whose name is the string pointed to by path
 * and associates a stream with it.
 */
extern FILE *fopen(const char *path, const char *mode);

/**
 * Opens the file whose file descriptor is the fd
 * and associates a stream with it.
 * FIXME mode ignored
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
extern int fseeko(FILE *stream, off_t offset, int origin);

extern long int ftell(FILE *stream);
extern off_t ftello(FILE *stream);

typedef long int fpos_t;

extern int fgetpos(FILE *stream, fpos_t *pos);

extern int fsetpos(FILE *stream, const fpos_t *pos);

/**
 * Sets the file position indicator for the stream pointed to by stream to the beginning of the file.
 */
extern void rewind(FILE *stream);

/**
 * Change the name or location of a file.
 */
extern int rename(const char *oldpath, const char *newpath);

/**
 * Manipulate the underlying device parameters of special files.
 */
extern int fioctl(FILE *fp, int request, ...);

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

extern int fileno(FILE *stream);

//TODO: stub
extern void clearerr(FILE *stream);
extern int feof(FILE *stream);
extern int ferror(FILE *stream);
extern int fflush(FILE *fp);


__END_DECLS

#endif /* STDIO_H_ */
