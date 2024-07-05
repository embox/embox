/**
 * @file
 *
 * @date Oct 6, 2022
 * @author Anton Bondarev
 */

#ifndef SRC_COMPAT_LIBC_WCHAR_WCHAR_EXTENDED_STUBS_H_
#define SRC_COMPAT_LIBC_WCHAR_WCHAR_EXTENDED_STUBS_H_

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


#endif /* SRC_COMPAT_LIBC_WCHAR_WCHAR_EXTENDED_STUBS_H_ */
