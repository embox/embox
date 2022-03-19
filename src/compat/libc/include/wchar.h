/**
 * @file
 *
 * @date 21.10.13
 * @author Alexander Kalmuk
 */

#ifndef WCHAR_H_
#define WCHAR_H_

#include <defines/wchar_t.h>
#include <defines/size_t.h>
#include <sys/cdefs.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>

#define WEOF      ((wint_t)(-1))

#if defined (__WCHAR_MIN__)
#define WCHAR_MIN __WCHAR_MIN__
#elif defined (__WCHAR_UNSIGNED__)
#define WCHAR_MIN 0x00000000UL
#else
#define WCHAR_MIN (-0x7FFFFFFF-1)
#endif

#if defined (__WCHAR_MAX__)
#define WCHAR_MAX __WCHAR_MAX__
#elif defined (__WCHAR_UNSIGNED__)
#define WCHAR_MAX 0xFFFFFFFFUL
#else
#define WCHAR_MAX 0x7FFFFFFFUL
#endif

typedef int mbstate_t;

__BEGIN_DECLS

extern int wcscmp(const wchar_t *s1, const wchar_t *s2);
extern wchar_t *wcsncpy(wchar_t *dst, const wchar_t *src, size_t n);
extern int wmemcmp(const wchar_t *s1, const wchar_t *s2, size_t n);
extern size_t wcslen(const wchar_t *s);
extern size_t wcsnlen(const wchar_t *s, size_t maxlen);
extern wchar_t *wmemchr(const wchar_t *s, wchar_t c, size_t n);
extern wchar_t *wmemmove(wchar_t *dest, const wchar_t *src, size_t n);
extern wchar_t *wmemcpy(wchar_t *dest, const wchar_t *src, size_t n);
extern wchar_t *wmemset(wchar_t *wcs, wchar_t wc, size_t n);

extern int vswprintf(wchar_t *wcs, size_t maxlen,
		const wchar_t *format, va_list args);

extern long wcstol(const wchar_t *nptr, wchar_t **endptr,
       int base);
extern long long wcstoll(const wchar_t *nptr,
       wchar_t **endptr, int base);
extern unsigned long wcstoul(const wchar_t *nptr,
       wchar_t **endptr, int base);
extern unsigned long long wcstoull(const wchar_t *nptr,
       wchar_t **endptr, int base);

extern float       wcstof(const wchar_t* str, wchar_t** str_end);
extern double      wcstod(const wchar_t* str, wchar_t** str_end);
extern long double wcstold(const wchar_t* str, wchar_t** endptr);

extern int wctob(wint_t);
extern wint_t btowc(int);
extern wint_t getwc(FILE *);
extern wint_t ungetwc(wint_t, FILE *);
extern wint_t putwc(wchar_t, FILE *);
extern size_t wcrtomb(char *, wchar_t, mbstate_t *);
extern size_t mbrtowc(wchar_t *, const char *, size_t, mbstate_t *);
extern int wcscoll(const wchar_t *, const wchar_t *);
extern size_t wcsxfrm(wchar_t *, const wchar_t *, size_t);
extern size_t wcsftime(wchar_t *, size_t,
                  const wchar_t *, const struct tm *);

extern int wcsncasecmp(const wchar_t *ws1, const wchar_t *ws2, size_t n);

extern wint_t getwchar(void);
extern wint_t fgetwc(FILE *stream);
extern wchar_t *fgetws(wchar_t *ws, int n, FILE *stream);
extern wint_t fputwc(wchar_t wc, FILE *stream);
extern int fputws(const wchar_t *wc, FILE *stream);
extern int fwide(FILE *stream, int wc);
extern int fwprintf(FILE *stream, const wchar_t *wc, ...);
extern int fwscanf(FILE *stream, const wchar_t *wc, ...);
extern int mbsinit(const mbstate_t *st);
extern size_t mbsrtowcs(wchar_t *dst, const char **src, size_t len, mbstate_t *ps);
extern wint_t putwchar(wchar_t wc);
extern int swprintf(wchar_t *s, size_t n, const wchar_t *format, ...);
extern int swscanf(const wchar_t *s, const wchar_t *format, ... );
extern int vwprintf(const wchar_t *format, va_list arg);
extern int vfwprintf(FILE *stream, const wchar_t *format, va_list arg);
extern wchar_t *wcscat(wchar_t *ws1, const wchar_t *ws2);
extern wchar_t *wcschr(const wchar_t *ws, wchar_t wc);
extern wchar_t *wcscpy(wchar_t *ws1, const wchar_t *ws2);
extern size_t wcscspn(const wchar_t *ws1, const wchar_t *ws2);
extern wchar_t *wcsncat(wchar_t *ws1, const wchar_t *ws2, size_t n);
extern int wcsncmp(const wchar_t *ws1, const wchar_t *ws2, size_t n);
extern wchar_t *wcspbrk(const wchar_t *ws1, const wchar_t *ws2);
extern wchar_t *wcsrchr(const wchar_t *ws, wchar_t wc);
extern size_t wcsrtombs(char *dst, const wchar_t **src, size_t len, mbstate_t *ps);
extern size_t wcsspn(const wchar_t *ws1, const wchar_t *ws2);
extern wchar_t *wcsstr(const wchar_t *ws1, const wchar_t *ws2);
extern wchar_t *wcstok(wchar_t *ws1, const wchar_t *ws2, wchar_t **ptr);
extern int wprintf(const wchar_t *format, ...);
extern int wscanf(const wchar_t *format, ... );
extern size_t mbrlen(const char * s, size_t n, mbstate_t * ps);

__END_DECLS

#endif /* WCHAR_H_ */
