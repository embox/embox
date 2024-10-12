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
#include <math.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <framework/mod/options.h>
#include <util/math.h>

#include "printf_impl.h"

#define SUPPORT_FLOATING OPTION_GET(NUMBER, support_floating)

/* clang-format off */

/**
 * Format specifiers
 */
#define OPS_FLAG_LEFT_ALIGN  0x00000001 /* left alignment */
#define OPS_FLAG_WITH_SIGN   0x00000002 /* print sign */
#define OPS_FLAG_EXTRA_SPACE 0x00000004 /* add extra space before digit */
#define OPS_FLAG_WITH_SPEC   0x00000008 /* print a prefix for non-decimal systems */
#define OPS_FLAG_ZERO_PAD    0x00000010 /* padding with zeroes */
#define OPS_PREC_IS_GIVEN    0x00000020 /* precision is specified */
#define OPS_LEN_MIN          0x00000040 /* s_char (d, i); u_char (u, o, x, X); s_char* (n) */
#define OPS_LEN_SHORT        0x00000080 /* short (d, i); u_short (u, o, x, X); short* (n) */
#define OPS_LEN_LONG         0x00000100 /* long (d, i); u_long (u, o, x, X); wint_t (c); wchar_t(s); long* (n) */
#define OPS_LEN_LONGLONG     0x00000200 /* llong (d, i); u_llong (u, o, x, X); llong* (n) */
#define OPS_LEN_MAX          0x00000400 /* intmax_t (d, i); uintmax_t (u, o, x, X); intmax_t* (n) */
#define OPS_LEN_SIZE         0x00000800 /* size_t (d, i, u, o, x, X); size_t* (n) */
#define OPS_LEN_PTRDIFF      0x00001000 /* ptrdiff_t (d, i, u, o, x, X); ptrdiff_t* (n) */
#define OPS_LEN_LONGFP       0x00002000 /* long double (f, F, e, E, g, G, a, A) */
#define OPS_SPEC_UPPER_CASE  0x00004000 /* specifier is tall */

/**
 * Options for print_s
 */
#define PRINT_S_NULL_STR "(null)" /* default value of NULL */

/**
 * Options for print_i
 */
/* size of buffer for long long int (64bit) -- that's enough for oct, dec and hex base systems */
#define PRINT_I_BUFF_SZ 23

/**
 * Options for print_f
 */
#define PRINT_F_BUFF_SZ      65 /* size of buffer for long double -- FIXME this may not be enough */
#define PRINT_F_PREC_DEFAULT 6  /* default precision for real numbers */
// #define PRINT_F_PREC_SHORTENED 4 /* shortened precision for real numbers */

/* clang-format on */

static int
print_s(int (*printchar_handler)(struct printchar_handler_data *d, int c),
    struct printchar_handler_data *printchar_data, const char *str, int width,
    int max_len, unsigned int ops) {
	int pc, len, space_count, ret;

	assert(printchar_handler != NULL);
	assert(str != NULL);
	assert(width >= 0);
	assert(max_len >= 0);

	pc = 0;
	len = strlen(str);
	if (ops & OPS_PREC_IS_GIVEN) {
		len = min(max_len, len);
	}
	space_count = width > len ? width - len : 0;

	if (!(ops & OPS_FLAG_LEFT_ALIGN)) {
		pc += space_count;
		for (; space_count; --space_count) {
			ret = printchar_handler(printchar_data, ' ');
			if (ret < 0) {
				return ret;
			}
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
		ret = printchar_handler(printchar_data, ' ');
		if (ret < 0) {
			return ret;
		}
	}

	return pc;
}

static int
print_i(int (*printchar_handler)(struct printchar_handler_data *d, int c),
    struct printchar_handler_data *printchar_data, unsigned long long int u,
    int is_signed, int width, int min_len, unsigned int ops, int base) {
	char buff[PRINT_I_BUFF_SZ], *str, *end, *prefix;
	int pc, ch, len, prefix_len, zero_count, space_count, letter_base, ret;

	assert(printchar_handler != NULL);
	assert(width >= 0);
	assert(min_len >= 0);

	str = end = &buff[0] + sizeof buff / sizeof buff[0] - 1;
	*end = '\0';

	if (is_signed && ((long long int)u < 0)) {
		u = -u;
		prefix = "-";
	}
	else if (is_signed && (ops & OPS_FLAG_WITH_SIGN)) {
		prefix = "+";
	}
	else if (is_signed && (ops & OPS_FLAG_EXTRA_SPACE)) {
		prefix = " ";
	}
	else if ((base == 8) && (ops & OPS_FLAG_WITH_SPEC)) {
		prefix = "0";
	}
	else if ((base == 16) && (ops & OPS_FLAG_WITH_SPEC)) {
		prefix = (ops & OPS_SPEC_UPPER_CASE) ? "0X" : "0x";
	}
	else {
		prefix = "";
	}

	pc = 0;
	prefix_len = strlen(prefix);
	letter_base = ops & OPS_SPEC_UPPER_CASE ? 'A' : 'a';

	do {
		ch = u % base;
		if (ch >= 10) {
			ch += letter_base - 10 - '0';
		}
		*--str = ch + '0';
		u /= base;
	} while (u);

	len = end - str;
	if (len < min_len) {
		zero_count = min_len;
	}
	else if ((ops & OPS_FLAG_ZERO_PAD) && !(ops & OPS_FLAG_LEFT_ALIGN)) {
		zero_count = width;
	}
	else {
		zero_count = 0;
	}
	zero_count -= len + prefix_len;
	zero_count = max(zero_count, 0);
	space_count = width - len - prefix_len - zero_count;
	space_count = max(space_count, 0);

	if (!(ops & OPS_FLAG_LEFT_ALIGN)) {
		pc += space_count;
		for (; space_count; --space_count) {
			ret = printchar_handler(printchar_data, ' ');
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
		ret = printchar_handler(printchar_data, '0');
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
		ret = printchar_handler(printchar_data, ' ');
		if (ret < 0) {
			return ret;
		}
	}

	return pc;
}

#if SUPPORT_FLOATING
#ifdef LONG_DOUBLE
#define DOUBLE long double
#define MODF   modfl
#define LOG10  log10l
#define FMOD   fmodl
#define POW    powl
#define FABS   fabsl
#else
#define DOUBLE double
#define MODF   modf
#define LOG10  log10
#define FMOD   fmod
#define POW    pow
#define FABS   fabs
#endif

static int
print_f(int (*printchar_handler)(struct printchar_handler_data *d, int c),
    struct printchar_handler_data *printchar_data, long double r, int width,
    int precision, unsigned int ops, int base, int with_exp, int is_shortened) {
	char buff[PRINT_F_BUFF_SZ], prefix_buf[4], *str, *end, *prefix, *postfix;

	DOUBLE ip, fp, ep;
	int pc, i, ch, len, prefix_len, postfix_len, pad_count, sign_count,
	    zero_left, letter_base, ret;

	assert(printchar_handler != NULL);
	assert(width >= 0);
	assert(precision >= 0);

	if (isnan(r)) {
		r = 0.0;
	}

	postfix = end = str = &buff[0] + sizeof buff / sizeof buff[0] - 1;
	*end = '\0';

	prefix = prefix_buf;
	if (signbit(r)) {
		r = -r;
		*prefix++ = '-';
	}
	else if (ops & OPS_FLAG_WITH_SIGN) {
		*prefix++ = '+';
	}
	if (base == 16) {
		*prefix++ = '0';
		*prefix++ = (ops & OPS_SPEC_UPPER_CASE) ? 'X' : 'x';
	}
	*prefix = '\0';
	prefix = prefix_buf;

	sign_count = i = pc = 0;
	prefix_len = strlen(prefix);
	letter_base = (ops & OPS_SPEC_UPPER_CASE) ? 'A' : 'a';

	if (ops & OPS_PREC_IS_GIVEN) {
		if (is_shortened) {
			precision = max(precision, 1);
		}
	}
	else {
		precision = (base == 16) ? 12 : PRINT_F_PREC_DEFAULT;
	}

	fp = MODF(r, &ip);
	if (with_exp || is_shortened) {
		ep = 0.0L;
		while (ip >= base) {
			fp = MODF((ip + fp) / base, &ip), ep += 1.0L;
		}
		if (fp != 0.0L) {
			while (ip == 0.0L) {
				fp = MODF((ip + fp) * base, &ip), ep -= 1.0L;
			}
		}
		if ((ep < -4) || (ep >= precision)) {
			with_exp = 1;
		}
	}
	fp = with_exp ? fp : MODF(r, &ip);

	if (is_shortened) {
		precision -= ceil(LOG10(ip)) + (ip != 0.0L);
	}

	assert(precision >= 0);
	for (; (sign_count < precision) && (FMOD(fp, 1.0L) != 0.0L); ++sign_count) {
		fp *= base;
	}

	fp = roundl(fp);
	if (precision) {
		if (fp == POW(base, sign_count)) {
			ip += 1.0L;
		}
	}
	else {
		ip = roundl(ip + fp);
	}

	if (fp == POW(base, sign_count)) {
		fp = 0.0L;
	}

	if (with_exp && (ip >= base)) {
		fp = MODF((ip + fp) / base, &ip), ep += 1.0L;
	}

	if (with_exp) {
		do {
			ch = FMOD(FABS(ep), base);
			assert((ch >= 0) && (ch < base));
			if (ch >= 10) {
				ch += letter_base - 10 - '0';
			}
			*--postfix = ch + '0';
			MODF(ep / base, &ep);
		} while (ep != 0.0L);
		if ((strlen(postfix) == 1) && (base != 16)) {
			*--postfix = '0';
		}
		*--postfix = signbit(ep) ? '-' : '+';
		*--postfix = base == 16 ? ops & OPS_SPEC_UPPER_CASE ? 'P' : 'p'
		             : ops & OPS_SPEC_UPPER_CASE ? 'E'
		                                         : 'e';
		str = end = postfix - 1;
		*end = '\0';
	}

	for (; i < sign_count; ++i) {
		ch = FMOD(fp, base);
		assert((ch >= 0) && (ch < base));
		if (ch >= 10) {
			ch += letter_base - 10 - '0';
		}
		*--str = ch + '0';
		MODF(fp / base, &fp);
	}

	if ((precision && !is_shortened) || sign_count
	    || (ops & OPS_FLAG_WITH_SPEC)) {
		*--str = '.';
	}

	do {
		ch = (int)FMOD(ip, (long double)base);
		assert((ch >= 0) && (ch < base));
		if (ch >= 10) {
			ch += letter_base - 10 - '0';
		}
		*--str = ch + '0';
		MODF(ip / base, &ip);
	} while (ip != 0.0L);

	len = end - str;
	postfix_len = strlen(postfix);
	zero_left = is_shortened ? 0 : precision - sign_count;
	pad_count = max(width - prefix_len - len - zero_left - postfix_len, 0);

	if (!(ops & (OPS_FLAG_ZERO_PAD | OPS_FLAG_LEFT_ALIGN))) {
		pc += pad_count;
		while (pad_count--) {
			ret = printchar_handler(printchar_data, ' ');
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

	if (ops & OPS_FLAG_ZERO_PAD) {
		pc += pad_count;
		while (pad_count--) {
			ret = printchar_handler(printchar_data, '0');
			if (ret < 0) {
				return ret;
			}
		}
	}

	pc += len;
	while (len--) {
		ret = printchar_handler(printchar_data, *str++);
		if (ret < 0) {
			return ret;
		}
	}

	pc += zero_left;
	while (zero_left--) {
		ret = printchar_handler(printchar_data, '0');
		if (ret < 0) {
			return ret;
		}
	}

	pc += postfix_len;
	while (postfix_len--) {
		ret = printchar_handler(printchar_data, *postfix++);
		if (ret < 0) {
			return ret;
		}
	}

	if (ops & OPS_FLAG_LEFT_ALIGN) {
		pc += pad_count;
		while (pad_count--) {
			ret = printchar_handler(printchar_data, ' ');
			if (ret < 0) {
				return ret;
			}
		}
	}

	return pc;
}
#else
static int
print_f(int (*printchar_handler)(struct printchar_handler_data *d, int c),
    struct printchar_handler_data *printchar_data, double r, int width,
    int precision, unsigned int ops, int base, int with_exp, int is_shortened) {
	return print_s(printchar_handler, printchar_data, "%f", 0, 0, 0);
}
#endif

int __print(int (*printchar_handler)(struct printchar_handler_data *d, int c),
    struct printchar_handler_data *printchar_data, const char *format,
    va_list args) {
	int pc, width, precision, ret;
	unsigned int ops;
	const char *begin;
	union {
		void *vp;
		char ca[2];
		char *cp;
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
		if (*format != '%') {
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
			default:
				goto after_flags;
			case '-':
				ops |= OPS_FLAG_LEFT_ALIGN;
				break;
			case '+':
				ops |= OPS_FLAG_WITH_SIGN;
				break;
			case ' ':
				ops |= OPS_FLAG_EXTRA_SPACE;
				break;
			case '#':
				ops |= OPS_FLAG_WITH_SPEC;
				break;
			case '0':
				ops |= OPS_FLAG_ZERO_PAD;
				break;
			}
		}
after_flags:

		/* get width */
		if (*format == '*') {
			width = va_arg(args, int);
			++format;
		}
		else {
			width = atoi(format);
			while (isdigit(*format))
				++format;
		}
		width = max(width, 0);

		/* get precision */
		ops |= *format == '.' ? OPS_PREC_IS_GIVEN : 0;
		if ((*format == '.') && (*++format == '*')) {
			precision = va_arg(args, int);
			++format;
		}
		else {
			precision = atoi(format);
			while (isdigit(*format))
				++format;
		}
		precision = precision >= 0 ? precision : (ops &= ~OPS_PREC_IS_GIVEN, 0);

		/* get length */
		switch (*format) {
		case 'h':
			ops |= *++format != 'h' ? OPS_LEN_SHORT : (++format, OPS_LEN_MIN);
			break;
		case 'l':
			ops |= *++format != 'l' ? OPS_LEN_LONG
			                        : (++format, OPS_LEN_LONGLONG);
			break;
		case 'j':
			ops |= OPS_LEN_MAX;
			++format;
			break;
		case 'z':
			ops |= OPS_LEN_SIZE;
			++format;
			break;
		case 't':
			ops |= OPS_LEN_PTRDIFF;
			++format;
			break;
		case 'L':
			ops |= OPS_LEN_LONGFP;
			++format;
			break;
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
		case '%':
			goto single_print;
		case 'd':
		case 'i':
			tmp.ulli = ops & OPS_LEN_MIN        ? (signed char)va_arg(args, int)
			           : ops & OPS_LEN_SHORT    ? (short int)va_arg(args, int)
			           : ops & OPS_LEN_LONG     ? va_arg(args, long int)
			           : ops & OPS_LEN_LONGLONG ? va_arg(args, long long int)
			           : ops & OPS_LEN_MAX      ? va_arg(args, intmax_t)
			           : ops & OPS_LEN_SIZE     ? va_arg(args, ssize_t)
			           : ops & OPS_LEN_PTRDIFF  ? va_arg(args, ptrdiff_t)
			                                    : va_arg(args, int);
			ret = print_i(printchar_handler, printchar_data, tmp.ulli, 1, width,
			    precision, ops, 10);
			if (ret < 0) {
				return ret;
			}
			pc += ret;
			break;
		case 'u':
		case 'o':
		case 'x':
		case 'X':
			tmp.ulli = ops & OPS_LEN_MIN
			               ? (unsigned char)va_arg(args, unsigned int)
			           : ops & OPS_LEN_SHORT
			               ? (unsigned short int)va_arg(args, unsigned int)
			           : ops & OPS_LEN_LONG ? va_arg(args, unsigned long int)
			           : ops & OPS_LEN_LONGLONG
			               ? va_arg(args, unsigned long long int)
			           : ops & OPS_LEN_MAX     ? va_arg(args, uintmax_t)
			           : ops & OPS_LEN_SIZE    ? va_arg(args, size_t)
			           : ops & OPS_LEN_PTRDIFF ? va_arg(args, ptrdiff_t)
			                                   : va_arg(args, unsigned int);
			ret = print_i(printchar_handler, printchar_data, tmp.ulli, 0, width,
			    precision, ops,
			    *format == 'u' ? 10 : (*format == 'o' ? 8 : 16));
			if (ret < 0) {
				return ret;
			}
			pc += ret;
			break;
		case 'f':
		case 'F':
		case 'e':
		case 'E':
		case 'g':
		case 'G':
		case 'a':
		case 'A':
			tmp.ld = ops & OPS_LEN_LONGFP ? va_arg(args, long double)
			                              : va_arg(args, double);
			ret = print_f(printchar_handler, printchar_data, tmp.ld, width,
			    precision, ops, tolower(*format) == 'a' ? 16 : 10,
			    tolower(*format) == 'e' || tolower(*format) == 'a',
			    tolower(*format) == 'g');
			if (ret < 0) {
				return ret;
			}
			pc += ret;
			break;
		case 'c':
			/* TODO handle (ops & OPS_LEN_LONG) for wint_t */
			tmp.ca[0] = (char)va_arg(args, int);
			tmp.ca[1] = '\0';
			ret = print_s(printchar_handler, printchar_data, &tmp.ca[0], width,
			    precision, ops);
			if (ret < 0) {
				return ret;
			}
			pc += ret;
			break;
		case 's':
			/* TODO handle (ops & OPS_LEN_LONG) for wchar_t* */
			tmp.cp = va_arg(args, char *);
			ret = print_s(printchar_handler, printchar_data,
			    tmp.cp ? tmp.cp : PRINT_S_NULL_STR, width, precision, ops);
			if (ret < 0) {
				return ret;
			}
			pc += ret;
			break;
		case 'p':
			tmp.vp = va_arg(args, void *);
			ret = print_i(printchar_handler, printchar_data, (size_t)tmp.vp, 0,
			    width, sizeof tmp.vp * 2 + 2,
			    ops | (OPS_FLAG_WITH_SPEC | OPS_FLAG_ZERO_PAD), 16);
			if (ret < 0) {
				return ret;
			}
			pc += ret;
			break;
		case 'n':
			if (ops & OPS_LEN_MIN)
				*va_arg(args, signed char *) = (signed char)pc;
			else if (ops & OPS_LEN_SHORT)
				*va_arg(args, short int *) = (short int)pc;
			else if (ops & OPS_LEN_LONG)
				*va_arg(args, long int *) = (long int)pc;
			else if (ops & OPS_LEN_LONGLONG)
				*va_arg(args, long long int *) = (long long int)pc;
			else if (ops & OPS_LEN_MAX)
				*va_arg(args, intmax_t *) = (intmax_t)pc;
			else if (ops & OPS_LEN_SIZE)
				*va_arg(args, size_t *) = (size_t)pc;
			else if (ops & OPS_LEN_PTRDIFF)
				*va_arg(args, ptrdiff_t *) = (ptrdiff_t)pc;
			else
				*va_arg(args, int *) = pc;
			break;
		}
	}

	return pc;
}
