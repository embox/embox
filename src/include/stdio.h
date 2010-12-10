/**
 * @file
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
#include <kernel/driver.h>
#include <fs/file.h>

#define EOF (-1)

extern FILE stdin, stdout;

extern int fputc(FILE f, int c);
extern int fgetc(FILE f);

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

int vprintf(const char *format, va_list args);

int vsprintf(char *s, const char *format, va_list args);

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
 * Opens the file whose file descriptor is the fd
 * and associates a stream with it.
 */
extern FILE *fdopen(int fd, const char *mode);

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

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
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
 * Get file status (size, mode, mtime and so on)
 */
extern int stat(const char *path, stat_t *buf);

#if defined(CONFIG_TRACE)
  #if defined(CONFIG_PROM_PRINTF)
     #include <kernel/prom_printf.h>
     # define TRACE(...) prom_printf(__VA_ARGS__)
  #else
     # define TRACE(...)  do ; while(0)
 #endif
//# define TRACE(...)  printk(__VA_ARGS__)
#else
# define TRACE(...)  do ; while (0)
#endif

#define PRINTREG32_BIN(reg) { \
	int i=0;                         \
	for (;i<32;i++)                   \
		TRACE("%d", (reg>>i)&1); \
		TRACE(" (0x%x)\n", reg); \
	}

#endif /* STDIO_H_ */
