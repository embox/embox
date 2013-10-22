/**
 * @file
 * @brief
 *
 * @date 22.10.13
 * @author Alexander Kalmuk
 */

#include <wctype.h>
#include <string.h>

#define _A    (_U | _D)
#define _B    0xFF /* FIXME: I am not sure that it is good solution */
#define _G    (_P | _U | _L | _D)
#define _R    (_P | _U | _L | _D | _SP)

wctype_t wctype(const char *property) {
	static const struct {
		const char	*name;
		wctype_t	 mask;
	} props[] = {
		{ "alnum",  _A |_D },
		{ "alpha",  _A },
		{ "blank",  _B },
		{ "cntrl",  _C },
		{ "digit",  _D },
		{ "graph",  _G },
		{ "lower",  _L },
		{ "print",  _R },
		{ "punct",  _P },
		{ "space",  _S },
		{ "upper",  _U },
		{ "xdigit", _X },
		{ NULL,     0UL }, /* Default */
	};
	int i;

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
		{ "tolower", _L },
		{ "toupper", _U },
		{ NULL,      0UL }, /* Default */
	};
	int i;

	while (classes[i].name != NULL && strcmp(classes[i].name, charclass) != 0) {
		i++;
	}

	return classes[i].type;
}

int iswctype(wint_t wc, wctype_t t) {
	/* FIXME it is not best way to extract _B into separate case*/
	if (t == _B) {
		return iswblank(wc);
	}
	return (__ismask(wc) & t);
}

wint_t towctrans(wint_t w, wctrans_t t) {
	switch (t) {
	case _L:
		return towlower(w);
	case _U:
		return towupper(w);
	default:
		return w;
	}
}
