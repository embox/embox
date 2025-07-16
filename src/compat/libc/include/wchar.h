/**
 * @file
 *
 * @date 21.10.13
 * @author Alexander Kalmuk
 */

#ifndef COMPAT_LIBC_WCHAR_H_
#define COMPAT_LIBC_WCHAR_H_

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h> /* WCHAR_MAX and WCHAR_MIN */
#include <stdio.h>
#include <sys/cdefs.h>
#include <time.h>

#define WEOF ((wint_t)(-1))

typedef __WINT_TYPE__ wint_t;
typedef int wctype_t;
typedef int mbstate_t;

__BEGIN_DECLS

extern size_t wcslen(const wchar_t *s);
extern int wcscmp(const wchar_t *s1, const wchar_t *s2);
extern wchar_t *wcsncpy(wchar_t *dst, const wchar_t *src, size_t n);
extern int wmemcmp(const wchar_t *s1, const wchar_t *s2, size_t n);
extern size_t wcslen(const wchar_t *s);
extern size_t wcsnlen(const wchar_t *s, size_t maxlen);
extern wchar_t *wmemchr(const wchar_t *s, wchar_t c, size_t n);
extern wchar_t *wmemmove(wchar_t *dest, const wchar_t *src, size_t n);
extern wchar_t *wmemcpy(wchar_t *dest, const wchar_t *src, size_t n);
extern wchar_t *wmemset(wchar_t *wcs, wchar_t wc, size_t n);

extern int swprintf(wchar_t *s, size_t n, const wchar_t *format, ...);
extern int vswprintf(wchar_t *wcs, size_t maxlen, const wchar_t *format,
    va_list args);

extern long wcstol(const wchar_t *nptr, wchar_t **endptr, int base);
extern long long wcstoll(const wchar_t *nptr, wchar_t **endptr, int base);
extern unsigned long wcstoul(const wchar_t *nptr, wchar_t **endptr, int base);
extern unsigned long long wcstoull(const wchar_t *nptr, wchar_t **endptr,
    int base);

extern float wcstof(const wchar_t *str, wchar_t **str_end);
extern double wcstod(const wchar_t *str, wchar_t **str_end);
extern long double wcstold(const wchar_t *str, wchar_t **endptr);

extern int wctob(wint_t);
extern wint_t btowc(int);
extern wint_t getwc(FILE *);
extern wint_t ungetwc(wint_t, FILE *);
extern wint_t putwc(wchar_t, FILE *);
extern size_t wcrtomb(char *, wchar_t, mbstate_t *);
extern size_t mbrtowc(wchar_t *, const char *, size_t, mbstate_t *);
extern int wcscoll(const wchar_t *, const wchar_t *);
extern size_t wcsxfrm(wchar_t *, const wchar_t *, size_t);
extern size_t wcsftime(wchar_t *, size_t, const wchar_t *, const struct tm *);

extern int wcsncasecmp(const wchar_t *ws1, const wchar_t *ws2, size_t n);

#if defined(__EMBOX__)
#include <framework/mod/options.h>

#include <module/embox/compat/libc/wchar_extended.h>
#endif

__END_DECLS

#endif /* COMPAT_LIBC_WCHAR_H_ */
