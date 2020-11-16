/**
 * @file
 * @brief
 *
 * @date 19.11.10
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>
#include <util/math.h>

struct printchar_handler_data;
/**
 * Format specifiers
 */
#define OPS_FLAG_LEFT_ALIGN   0x00000001 /* left alignment */
#define OPS_FLAG_WITH_SIGN    0x00000002 /* print sign */
#define OPS_FLAG_EXTRA_SPACE  0x00000004 /* add extra space before digit */
#define OPS_FLAG_WITH_SPEC    0x00000008 /* print a prefix for non-decimal systems */
#define OPS_FLAG_ZERO_PAD     0x00000010 /* padding with zeroes */
#define OPS_PREC_IS_GIVEN     0x00000020 /* precision is specified */
#define OPS_LEN_MIN           0x00000040 /* s_char (d, i); u_char (u, o, x, X); s_char* (n) */
#define OPS_LEN_SHORT         0x00000080 /* short (d, i); u_short (u, o, x, X); short* (n) */
#define OPS_LEN_LONG          0x00000100 /* long (d, i); u_long (u, o, x, X); wint_t (c); wchar_t(s); long* (n) */
#define OPS_LEN_LONGLONG      0x00000200 /* llong (d, i); u_llong (u, o, x, X); llong* (n) */
#define OPS_LEN_MAX           0x00000400 /* intmax_t (d, i); uintmax_t (u, o, x, X); intmax_t* (n) */
#define OPS_LEN_SIZE          0x00000800 /* size_t (d, i, u, o, x, X); size_t* (n) */
#define OPS_LEN_PTRDIFF       0x00001000 /* ptrdiff_t (d, i, u, o, x, X); ptrdiff_t* (n) */
#define OPS_LEN_LONGFP        0x00002000 /* long double (f, F, e, E, g, G, a, A) */
#define OPS_SPEC_UPPER_CASE   0x00004000 /* specifier is tall */

/**
 * Options for print_s
 */
#define PRINT_S_NULL_STR L"(null)" /* default value of NULL */

/**
 * Options for print_i
 */
#define PRINT_I_BUFF_SZ 23 /* size of buffer for long long int (64bit) -- that's enough for oct, dec and hex base systems */

/**
 * Options for print_f
 */
#define PRINT_F_BUFF_SZ        65 /* size of buffer for long double -- FIXME this may not be enough */
//#define PRINT_F_PREC_SHORTENED 4 /* shortened precision for real numbers */
#define PRINT_F_PREC_DEFAULT   6 /* default precision for real numbers */

static int print_s(wchar_t (*printchar_handler)(struct printchar_handler_data *d, wchar_t c),
		struct printchar_handler_data *printchar_data,
		const wchar_t *str, int width, int max_len, unsigned int ops) {
	int pc, len, space_count, ret;

	assert(printchar_handler != NULL);
	assert(str != NULL);
	assert(width >= 0);
	assert(max_len >= 0);

	pc = 0;
	len = wcslen(str);
	if (ops & OPS_PREC_IS_GIVEN) {
		len = min(max_len, len);
	}
	space_count = width > len ? width - len : 0;

	if (!(ops & OPS_FLAG_LEFT_ALIGN)) {
		pc += space_count;
		for (; space_count; --space_count) {
			ret = printchar_handler(printchar_data, L' ');
			if (ret < 0) {
				return ret;
			}
		}
	}

	pc += len;
	while (len--){
		ret = printchar_handler(printchar_data, *str++);
		if (ret < 0) {
			return ret;
		}
	}

	pc += space_count;
	while (space_count--){
		ret = printchar_handler(printchar_data, L' ');
		if (ret < 0) {
			return ret;
		}
	}

	return pc;
}

static wchar_t print_i(wchar_t (*printchar_handler)(struct printchar_handler_data *d, wchar_t c),
		struct printchar_handler_data *printchar_data, unsigned long long int u,
		int is_signed, int width, int min_len, unsigned int ops, int base) {
	wchar_t buff[PRINT_I_BUFF_SZ], *str, *end, *prefix;
	int pc, ch, len, prefix_len, zero_count, space_count, letter_base, ret;

	assert(printchar_handler != NULL);
	assert(width >= 0);
	assert(min_len >= 0);

	str = end = &buff[0] + sizeof buff / sizeof buff[0] - 1;
	*end = L'\0';
	prefix = is_signed && ((long long int)u < 0) ? (u = -u, L"-")
			: is_signed && (ops & OPS_FLAG_WITH_SIGN) ? L"+"
			: is_signed && (ops & OPS_FLAG_EXTRA_SPACE) ? L" "
			: (base == 8) && (ops & OPS_FLAG_WITH_SPEC) ? L"0"
			: (base == 16) && (ops & OPS_FLAG_WITH_SPEC)
				? ops & OPS_SPEC_UPPER_CASE ? L"0X" : L"0x"
			: L"";
	pc = 0;
	prefix_len = wcslen(prefix);
	letter_base = ops & OPS_SPEC_UPPER_CASE ? L'A' : L'a';

	do {
		ch = u % base;
		if (ch >= 10) ch += letter_base - 10 - L'0';
		*--str = ch + L'0';
		u /= base;
	} while (u);

	len = end - str;
	zero_count = (len < min_len ? min_len : (ops & OPS_FLAG_ZERO_PAD)
			&& !(ops & OPS_FLAG_LEFT_ALIGN) ? width : 0) - len - prefix_len;
	zero_count = max(zero_count, 0);
	space_count = width - len - prefix_len - zero_count;
	space_count = max(space_count, 0);

	if (!(ops & OPS_FLAG_LEFT_ALIGN)) {
		pc += space_count;
		for (; space_count; --space_count) {
			ret = printchar_handler(printchar_data, L' ');
			if (ret < 0) {
				return ret;
			}
		}
	}

	pc += prefix_len;
	while (prefix_len--) {
		ret = printchar_handler(printchar_data, *prefix++);
		if (ret < 0) {
			return ret;
		}
	}

	pc += zero_count;
	while (zero_count--) {
		ret = printchar_handler(printchar_data, L'0');
		if (ret < 0) {
			return ret;
		}
	}

	pc += len;
	while (len--) {
		ret = printchar_handler(printchar_data, *str++);
		if (ret < 0) {
			return ret;
		}
	}

	pc += space_count;
	while (space_count--) {
		ret = printchar_handler(printchar_data, L' ');
		if (ret < 0) {
			return ret;
		}
	}

	return pc;
}


int __wprint(wchar_t (*printchar_handler)(struct printchar_handler_data *d, wchar_t c),
		struct printchar_handler_data *printchar_data,
		const wchar_t *format, va_list args) {
	int pc,precision, ret;
	long width;
	unsigned int ops;
	const wchar_t *begin;
	union {
		void *vp;
		wchar_t ca[2];
		wchar_t *cp;
		unsigned long long int ulli;
		long double ld;
	} tmp;

	assert(printchar_handler != NULL);
	assert(format != NULL);

	pc = 0;

	for (begin = format; *format; begin = ++format) {
		/**
		 * %[flags][width][.precision][length]specifier
		 */

		/* check first symbol */
		if (*format != L'%') {
single_print:
			++pc;
			ret = printchar_handler(printchar_data, *format);
			if (ret < 0) {
				return ret;
			}
			continue;
		}

		ops = 0;

		/* get flags */
		while (*++format) {
			switch (*format) {
			default: goto after_flags;
			case L'-': ops |= OPS_FLAG_LEFT_ALIGN; break;
			case L'+': ops |= OPS_FLAG_WITH_SIGN; break;
			case L' ': ops |= OPS_FLAG_EXTRA_SPACE; break;
			case L'#': ops |= OPS_FLAG_WITH_SPEC; break;
			case L'0': ops |= OPS_FLAG_ZERO_PAD; break;
			}
		}
after_flags:

		/* get width */
		if (*format == L'*') {
			width = va_arg(args, int);
			++format;
		} else {
			width = wcstol(format, NULL, 10);
			while (iswdigit(*format)) {
				++format;
			}
		}
		width = max(width, 0);

		/* get precision */
		ops |= *format == L'.' ? OPS_PREC_IS_GIVEN : 0;
		if ((*format == L'.') && (*++format == L'*')) {
			precision = va_arg(args, int);
			++format;
		} else {
			precision = wcstol(format, NULL, 10);
			while (iswdigit(*format)) {
				++format;
			}
		}
		precision = precision >= 0 ? precision : (ops &= ~OPS_PREC_IS_GIVEN, 0);

		/* get length */
		switch (*format) {
		case L'h': ops |= *++format != L'h' ? OPS_LEN_SHORT : (++format, OPS_LEN_MIN); break;
		case L'l': ops |= *++format != L'l' ? OPS_LEN_LONG : (++format, OPS_LEN_LONGLONG); break;
		case L'j': ops |= OPS_LEN_MAX; ++format; break;
		case L'z': ops |= OPS_LEN_SIZE; ++format; break;
		case L't': ops |= OPS_LEN_PTRDIFF; ++format; break;
		case L'L': ops |= OPS_LEN_LONGFP; ++format; break;
		}

		/* handle specifier */
		ops |= isupper(*format) ? OPS_SPEC_UPPER_CASE : 0;
		switch (*format) {
		default:
			pc += format - begin + 1;
			do {
				ret = printchar_handler(printchar_data, *begin);
				if (ret < 0) {
					return ret;
				}
			} while (++begin <= format);
			break;
		case L'%': goto single_print;
		case L'd':
		case L'i':
			tmp.ulli = ops & OPS_LEN_MIN ? (signed char)va_arg(args, int)
					: ops & OPS_LEN_SHORT ? (short int)va_arg(args, int)
					: ops & OPS_LEN_LONG ? va_arg(args, long int)
					: ops & OPS_LEN_LONGLONG ? va_arg(args, long long int)
					: ops & OPS_LEN_MAX ? va_arg(args, intmax_t)
					: ops & OPS_LEN_SIZE ? va_arg(args, ssize_t)
					: ops & OPS_LEN_PTRDIFF ? va_arg(args, ptrdiff_t)
					: va_arg(args, int);
			ret = print_i(printchar_handler, printchar_data, tmp.ulli, 1, width,
					precision, ops, 10);
			if (ret < 0) {
				return ret;
			}
			pc += ret;
			break;
		case L'u':
		case L'o':
		case L'x':
		case L'X':
			tmp.ulli = ops & OPS_LEN_MIN ? (unsigned char)va_arg(args, unsigned int)
					: ops & OPS_LEN_SHORT ? (unsigned short int)va_arg(args, unsigned int)
					: ops & OPS_LEN_LONG ? va_arg(args, unsigned long int)
					: ops & OPS_LEN_LONGLONG ? va_arg(args, unsigned long long int)
					: ops & OPS_LEN_MAX ? va_arg(args, uintmax_t)
					: ops & OPS_LEN_SIZE ? va_arg(args, size_t)
					: ops & OPS_LEN_PTRDIFF ? va_arg(args, ptrdiff_t)
					: va_arg(args, unsigned int);
			ret = print_i(printchar_handler, printchar_data, tmp.ulli, 0, width,
					precision, ops, *format == L'u' ? 10 : (*format == L'o' ? 8 : 16));
			if (ret < 0) {
				return ret;
			}
			pc += ret;
			break;
		case L'f':
		case L'F':
		case L'e':
		case L'E':
		case L'g':
		case L'G':
		case L'a':
		case L'A':

			/* TODO We don't support float point */
			assert(0);

			break;
		case L'c':
			/* TODO handle (ops & OPS_LEN_LONG) for wint_t */
			tmp.ca[0] = (char)va_arg(args, int);
			tmp.ca[1] = L'\0';
			ret = print_s(printchar_handler, printchar_data, &tmp.ca[0],
					width, precision, ops);
			if (ret < 0) {
				return ret;
			}
			pc += ret;
			break;
		case L's':
			/* TODO handle (ops & OPS_LEN_LONG) for wchar_t* */
			tmp.cp = va_arg(args, wchar_t *);
			ret = print_s(printchar_handler, printchar_data,
					tmp.cp ? tmp.cp : PRINT_S_NULL_STR, width, precision, ops);
			if (ret < 0) {
				return ret;
			}
			pc += ret;
			break;
		case L'p':
			tmp.vp = va_arg(args, void *);
			ret = print_i(printchar_handler, printchar_data, (size_t)tmp.vp, 0,
					width, sizeof tmp.vp * 2 + 2,
					ops | (OPS_FLAG_WITH_SPEC | OPS_FLAG_ZERO_PAD), 16);
			if (ret < 0) {
				return ret;
			}
			pc += ret;
			break;
		case L'n':
			if (ops & OPS_LEN_MIN) *va_arg(args, signed char *) = (signed char)pc;
			else if (ops & OPS_LEN_SHORT) *va_arg(args, short int *) = (short int)pc;
			else if (ops & OPS_LEN_LONG) *va_arg(args, long int *) = (long int)pc;
			else if (ops & OPS_LEN_LONGLONG) *va_arg(args, long long int *) = (long long int)pc;
			else if (ops & OPS_LEN_MAX) *va_arg(args, intmax_t *) = (intmax_t)pc;
			else if (ops & OPS_LEN_SIZE) *va_arg(args, size_t *) = (size_t)pc;
			else if (ops & OPS_LEN_PTRDIFF) *va_arg(args, ptrdiff_t *) = (ptrdiff_t)pc;
			else *va_arg(args, int *) = pc;
			break;
		}
	}

	return pc;
}
