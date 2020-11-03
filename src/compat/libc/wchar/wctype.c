/**
 * @file
 * @brief
 *
 * @date 22.10.13
 * @author Alexander Kalmuk
 */

#include <wctype.h>
#include <string.h>

/* Section "LC_CTYPE Category in the POSIX Locale" in
 * http://pubs.opengroup.org/onlinepubs/007904975/basedefs/xbd_chap07.html
 * says that there are 8 default classes for POSIX locale and <space>. In total 9. So
 * we must have 9 bit mask to distinguish those 9 classes, but we have only 8 (sizeof char).
 * Therefore I extracted "blank" class in separate case. */
#define _B    0xFF            // <space> and <tab>

#define _A     (_CTYPE_U | _CTYPE_L)      // "alpha"
#define _AN    (_CTYPE_U | _CTYPE_L | _CTYPE_D) // "alnum"
#define _G     (_CTYPE_P | _AN)     // "graph"
#define _R     (_G | _CTYPE_SP)     // "print"

wctype_t wctype(const char *property) {
	static const struct {
		const char	*name;
		wctype_t	 mask;
	} props[] = {
		{ "alnum",  _AN },
		{ "alpha",  _A },
		{ "blank",  _B },
		{ "cntrl",  _CTYPE_C },
		{ "digit",  _CTYPE_D },
		{ "graph",  _G },
		{ "lower",  _CTYPE_L },
		{ "print",  _R },
		{ "punct",  _CTYPE_P },
		{ "space",  _CTYPE_S },
		{ "upper",  _CTYPE_U },
		{ "xdigit", _CTYPE_X },
		{ NULL,     0UL }, /* Default */
	};
	int i = 0;

	while (props[i].name != NULL && strcmp(props[i].name, property) != 0) {
		i++;
	}

	return props[i].mask;
}

wctrans_t wctrans(const char *charclass) {
	static const struct {
		const char	*name;
		wctrans_t	 type;
	} classes[] = {
		{ "tolower", _CTYPE_L },
		{ "toupper", _CTYPE_U },
		{ NULL,      0UL }, /* Default */
	};
	int i = 0;

	while (classes[i].name != NULL && strcmp(classes[i].name, charclass) != 0) {
		i++;
	}

	return classes[i].type;
}

int iswctype(wint_t wc, wctype_t t) {
	if (wc == WEOF)
		return 0;
	if (t == _B) {
		return iswblank(wc);
	}
	return (wc < (wint_t)0x100 ? __ismask(wc) & t : 0);
}

wint_t towctrans(wint_t w, wctrans_t t) {
	if (w == WEOF)
		return WEOF;
	switch (t) {
	case _CTYPE_L:
		return towlower(w);
	case _CTYPE_U:
		return towupper(w);
	default:
		return w;
	}
}

wint_t towlower(wint_t wc) {
	if (wc == WEOF)
		return WEOF;
	return (wc < (wint_t)0x100 ? tolower(wc) : wc);
}

wint_t towupper(wint_t wc) {
	if (wc == WEOF)
		return WEOF;
	return (wc < (wint_t)0x100 ? toupper(wc) : wc);
}

int iswspace(wint_t wc) {
	return ((wc < (wint_t)0x100 ? isspace(wc) : 0));
}
