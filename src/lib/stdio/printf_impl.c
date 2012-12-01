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
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/**
 * Format specifiers
 */
#define OPS_FLAG_LEFT_ALIGN  0x0001 /* left alignment */
#define OPS_FLAG_WITH_SIGN   0x0002 /* print sign */
#define OPS_FLAG_EXTRA_SPACE 0x0004 /* add extra space before digit */
#define OPS_FLAG_WITH_PREFIX 0x0008 /* print a prefix for non-decimal systems */
#define OPS_FLAG_ZERO_PAD    0x0010 /* padding with zeroes */
#define OPS_LEN_MIN          0x0020 /* s_char (d, i); u_char (u, o, x, X); s_char* (n) */
#define OPS_LEN_SHORT        0x0040 /* short (d, i); u_short (u, o, x, X); short* (n) */
#define OPS_LEN_LONG         0x0080 /* long (d, i); u_long (u, o, x, X); wint_t (c); wchar_t(s); long* (n) */
#define OPS_LEN_LONGLONG     0x0100 /* llong (d, i); u_llong (u, o, x, X); llong* (n) */
#define OPS_LEN_MAX          0x0200 /* intmax_t (d, i); uintmax_t (u, o, x, X); intmax_t* (n) */
#define OPS_LEN_SIZE         0x0400 /* size_t (d, i, u, o, x, X); size_t* (n) */
#define OPS_LEN_PTRDIFF      0x0800 /* ptrdiff_t (d, i, u, o, x, X); ptrdiff_t* (n) */
#define OPS_LEN_LONGFP       0x1000 /* long double (f, F, e, E, g, G, a, A) */

/* Space for string representation of a 64 bit integer */
#define PRINT_I_BUFF_SZ 65

struct printchar_handler_data;

static int print_s(void (*printchar_handler)(struct printchar_handler_data *d, int c),
		struct printchar_handler_data *printchar_data,
		const char *str, int width, int max_len, unsigned int ops) {
	int pc, len, pad_count;

	assert(printchar_handler != NULL);
	assert(str != NULL);

	pc = 0;
	len = strlen(str);
	max_len = max_len ? max_len : len;
	len = min(max_len, len);
	pad_count = width > len ? width - len : 0;

	if (!(ops & OPS_FLAG_LEFT_ALIGN)) {
		pc += pad_count;
		for (; pad_count; --pad_count) printchar_handler(printchar_data, ' ');
	}

	pc += len;
	while (len--) printchar_handler(printchar_data, *str++);

	pc += pad_count;
	while (pad_count--) printchar_handler(printchar_data, ' ');

	return pc;
}

static int print_i(void (*printchar_handler)(struct printchar_handler_data *d, int c),
		struct printchar_handler_data *printchar_data,
		unsigned long long int u, int is_signed, int width, int min_len,
		unsigned int ops, int base, int letbase) {
	char buff[PRINT_I_BUFF_SZ], *str, *end;
	unsigned long long int t;
	int neg, len, extra_len, pad_count;

	assert(printchar_handler != NULL);

	str = end = &buff[0] + sizeof buff / sizeof buff[0] - 1;
	*end = '\0';
	neg = is_signed && ((long long int)u < 0);
	if (neg) u = -u;

	do {
		t = u % base;
		if (t >= 10) t += letbase - 10 - '0';
		*--str = t + '0';
		u /= base;
	} while (u);

	len = end - str;
	extra_len = neg || (is_signed && (ops & (OPS_FLAG_WITH_SIGN | OPS_FLAG_EXTRA_SPACE))) ? 1
			: ((base != 10) && (ops & OPS_FLAG_WITH_PREFIX) ? 1 + (base == 16) : 0);
	pad_count = (len < min_len ? min_len :
			((ops & OPS_FLAG_ZERO_PAD) && !(ops & OPS_FLAG_LEFT_ALIGN) ? width : 0)) - len - extra_len;
	while (pad_count-- > 0) *--str = '0';

	if (neg) *--str = '-';
	else if (is_signed && (ops & (OPS_FLAG_WITH_SIGN | OPS_FLAG_EXTRA_SPACE)))
		*--str = ops & OPS_FLAG_WITH_SIGN ? '+' : ' ';
	else if ((base != 10) && (ops & OPS_FLAG_WITH_PREFIX)) {
		if (base == 16) *--str = letbase + 'x' - 'a';
		*--str = '0';
	}

	return print_s(printchar_handler, printchar_data, str,
			ops & OPS_FLAG_ZERO_PAD ? 0 : width, 0, ops);
}

int __print(void (*printchar_handler)(struct printchar_handler_data *d, int c),
		struct printchar_handler_data *printchar_data,
		const char *format, va_list args) {
	int pc, width, precision;
	unsigned int ops;
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

	for (; *format; ++format) {
		/**
		 * %[flags][width][.precision][length]specifier
		 */

		/* check first symbol */
		if (*format != '%') {
single_print:
			printchar_handler(printchar_data, *format);
			++pc;
			continue;
		}

		ops = 0;

		/* get flags */
		while (*++format) {
			switch (*format) {
			default: goto after_flags;
			case '-': ops |= OPS_FLAG_LEFT_ALIGN; break;
			case '+': ops |= OPS_FLAG_WITH_SIGN; break;
			case ' ': ops |= OPS_FLAG_EXTRA_SPACE; break;
			case '#': ops |= OPS_FLAG_WITH_PREFIX; break;
			case '0': ops |= OPS_FLAG_ZERO_PAD; break;
			}
		}
after_flags:

		/* get width */
		if (*format == '*') { width = va_arg(args, int); ++format; }
		else { width = atoi(format); while (isdigit(*format)) ++format; }

		/* get precision */
		if ((*format == '.') && (*++format == '*')) { precision = va_arg(args, int); ++format; }
		else { precision = atoi(format); while (isdigit(*format)) ++format; }

		/* get length */
		switch (*format) {
		case 'h': ops |= *++format != 'h' ? OPS_LEN_MIN : (++format, OPS_LEN_SHORT); break;
		case 'l': ops |= *++format != 'l' ? OPS_LEN_LONG : (++format, OPS_LEN_LONGLONG); break;
		case 'j': ops |= OPS_LEN_MAX; ++format; break;
		case 'z': ops |= OPS_LEN_SIZE; ++format; break;
		case 't': ops |= OPS_LEN_PTRDIFF; ++format; break;
		case 'L': ops |= OPS_LEN_LONGFP; ++format; break;
		}

		/* handle specifier */
		switch (*format) {
		case '%': goto single_print;
		case 'd':
		case 'i':
			tmp.ulli = ops & OPS_LEN_MIN ? (signed char)va_arg(args, int)
					: ops & OPS_LEN_SHORT ? (short int)va_arg(args, int)
					: ops & OPS_LEN_LONG ? va_arg(args, long int)
					: ops & OPS_LEN_LONGLONG ? va_arg(args, long long int)
					: ops & OPS_LEN_MAX ? va_arg(args, intmax_t)
					: ops & OPS_LEN_SIZE ? va_arg(args, ssize_t)
					: ops & OPS_LEN_PTRDIFF ? va_arg(args, ptrdiff_t)
					: va_arg(args, int);
			pc += print_i(printchar_handler, printchar_data, tmp.ulli, 1,
					width, precision, ops, 10, 0);
			break;
		case 'u':
		case 'o':
		case 'x':
		case 'X':
			tmp.ulli = ops & OPS_LEN_MIN ? (unsigned char)va_arg(args, unsigned int)
					: ops & OPS_LEN_SHORT ? (unsigned short int)va_arg(args, unsigned int)
					: ops & OPS_LEN_LONG ? va_arg(args, unsigned long int)
					: ops & OPS_LEN_LONGLONG ? va_arg(args, unsigned long long int)
					: ops & OPS_LEN_MAX ? va_arg(args, uintmax_t)
					: ops & OPS_LEN_SIZE ? va_arg(args, size_t)
					: ops & OPS_LEN_PTRDIFF ? va_arg(args, ptrdiff_t)
					: va_arg(args, unsigned int);
			pc += print_i(printchar_handler, printchar_data, tmp.ulli, 0,
					width, precision, ops,
					*format == 'o' ? 8 : (*format == 'u' ? 10 : 16),
					(*format == 'o') || (*format == 'u') ? 0 : *format - ('x' - 'a'));
			break;
		case 'f':
		case 'F':
		case 'e':
		case 'E':
		case 'g':
		case 'G':
		case 'a':
		case 'A':
			/* TODO printf haven't realized float variable operations; clean stack only */
			tmp.ld = ops & OPS_LEN_LONGFP ? va_arg(args, long double) : va_arg(args, double);
			break;
		case 'c':
			/* TODO handle (ops & OPS_LEN_LONG) for wint_t */
			tmp.ca[0] = (char)va_arg(args, int);
			tmp.ca[1] = '\0';
			pc += print_s(printchar_handler, printchar_data, &tmp.ca[0],
					width, precision, ops);
			break;
		case 's':
			/* TODO handle (ops & OPS_LEN_LONG) for wchar_t* */
			tmp.cp = va_arg(args, char *);
			pc += print_s(printchar_handler, printchar_data,
					tmp.cp ? tmp.cp : "(null)", width, precision, ops);
			break;
		case 'p':
			tmp.vp = va_arg(args, void *);
			pc += print_i(printchar_handler, printchar_data, (size_t)tmp.vp,
					0, width, sizeof tmp.vp * 2 + 2,
					ops | (OPS_FLAG_WITH_PREFIX | OPS_FLAG_ZERO_PAD), 16, 'a');
			break;
		case 'n':
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
