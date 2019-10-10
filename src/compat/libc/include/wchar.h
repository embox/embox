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

int wcscmp(const wchar_t *s1, const wchar_t *s2);
wchar_t *wcsncpy(wchar_t *dst, const wchar_t *src, size_t n);

#endif /* WCHAR_H_ */
