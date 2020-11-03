/**
 * @file
 * @brief
 *
 * @date   27.10.2020
 * @author Alexander Kalmuk
 */

#include <wchar.h>
#include <inttypes.h>

int wmemcmp(const wchar_t *s1, const wchar_t *s2, size_t n) {
	return -1;
}

size_t wcslen(const wchar_t *s) {
	return 0;
}

wchar_t *wmemchr(const wchar_t *s, wchar_t c, size_t n) {
	return NULL;
}

wchar_t *wmemmove(wchar_t *dest, const wchar_t *src, size_t n) {
	return NULL;
}

wchar_t *wmemcpy(wchar_t *dest, const wchar_t *src, size_t n) {
	return NULL;
}

wchar_t *wmemset(wchar_t *wcs, wchar_t wc, size_t n) {
	return NULL;
}

int vswprintf(wchar_t *wcs, size_t maxlen,
		const wchar_t *format, va_list args) {
	return -1;
}

long wcstol(const wchar_t *restrict nptr, wchar_t **restrict endptr,
       int base) {
	return -1;
}

long long wcstoll(const wchar_t *restrict nptr,
       wchar_t **restrict endptr, int base) {
	return -1;
}

unsigned long wcstoul(const wchar_t *restrict nptr,
       wchar_t **restrict endptr, int base) {
	return -1;
}

unsigned long long wcstoull(const wchar_t *restrict nptr,
       wchar_t **restrict endptr, int base) {
	return -1;
}

float wcstof(const wchar_t* str, wchar_t** str_end) {
	return 0;
}

double wcstod(const wchar_t* str, wchar_t** str_end) {
	return 0;
}

long double wcstold(const wchar_t* str, wchar_t** endptr) {
	return 0;
}

int wctob(wint_t wc) {
	return 0;
}

wint_t btowc(int c) {
	return 0;
}

wint_t getwc(FILE *f) {
	return 0;
}

wint_t ungetwc(wint_t wc, FILE *f) {
	return 0;
}

wint_t putwc(wchar_t wc, FILE *f) {
	return 0;
}

size_t wcrtomb(char *c, wchar_t wc, mbstate_t *st) {
	return 0;
}

size_t mbrtowc(wchar_t *wc, const char *c, size_t s, mbstate_t *st) {
	return 0;
}

int wcscoll(const wchar_t *wc, const wchar_t *wci) {
	return 0;
}

size_t wcsxfrm(wchar_t *wc, const wchar_t *wci, size_t s) {
	return 0;
}

size_t wcsftime(wchar_t *wc, size_t s, const wchar_t *wci, const struct tm *tm) {
	return 0;
}

int wcsncasecmp(const wchar_t *ws1, const wchar_t *ws2, size_t n) {
	return 0;
}
