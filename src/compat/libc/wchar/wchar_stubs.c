/**
 * @file
 * @brief
 *
 * @date   27.10.2020
 * @author Alexander Kalmuk
 */

#include <wchar.h>
#include <inttypes.h>
#include <string.h>

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

wint_t getwchar(void) {
	return 0;
}

wint_t fgetwc(FILE *stream) {
	return 0;
}

wchar_t *fgetws(wchar_t *ws, int n, FILE *stream) {
	return NULL;
}

wint_t fputwc(wchar_t wc, FILE *stream) {
	return 0;
}

int fputws(const wchar_t *wc, FILE *stream) {
	return 0;
}

int fwide(FILE *stream, int wc) {
	return 0;
}

int fwprintf(FILE *stream, const wchar_t *wc, ...) {
	return 0;
}

int fwscanf(FILE *stream, const wchar_t *wc, ...){
	return 0;
}

int mbsinit(const mbstate_t *st) {
	return 0;
}
size_t mbsrtowcs(wchar_t *dst, const char **src, size_t len, mbstate_t *ps) {
	return 0;
}

wint_t putwchar(wchar_t wc) {
	return 0;
}

int swprintf(wchar_t *s, size_t n, const wchar_t *format, ...) {
	return 0;
}

int swscanf(const wchar_t *s, const wchar_t *format, ... ) {
	return 0;
}

int vwprintf(const wchar_t *format, va_list arg) {
	return 0;
}

int vfwprintf(FILE *stream, const wchar_t *format, va_list arg) {
	return 0;
}

wchar_t *wcscat(wchar_t *ws1, const wchar_t *ws2) {
	return NULL;
}

wchar_t *wcschr(const wchar_t *ws, wchar_t wc) {
	return NULL;
}

wchar_t *wcscpy(wchar_t *ws1, const wchar_t *ws2) {
	return NULL;
}

size_t wcscspn(const wchar_t *ws1, const wchar_t *ws2) {
	return 0;
}

wchar_t *wcsncat(wchar_t *ws1, const wchar_t *ws2, size_t n) {
	return NULL;
}

int wcsncmp(const wchar_t *ws1, const wchar_t *ws2, size_t n) {
	return 0;
}

wchar_t *wcspbrk(const wchar_t *ws1, const wchar_t *ws2) {
	return NULL;
}

wchar_t *wcsrchr(const wchar_t *ws, wchar_t wc) {
	return NULL;
}

size_t wcsrtombs(char *dst, const wchar_t **src, size_t len, mbstate_t *ps) {
	return 0;
}

size_t wcsspn(const wchar_t *ws1, const wchar_t *ws2) {
	return 0;
}

wchar_t *wcsstr(const wchar_t *ws1, const wchar_t *ws2) {
	return NULL;
}

wchar_t *wcstok(wchar_t *ws1, const wchar_t *ws2, wchar_t **ptr) {
	return NULL;
}

int wprintf(const wchar_t *format, ...) {
	return 0;
}

int wscanf(const wchar_t *format, ... ) {
	return 0;
}

size_t mbrlen(const char * s, size_t n, mbstate_t * ps) {
	return 0;
}
