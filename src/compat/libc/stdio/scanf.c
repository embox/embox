/**
 * @file
 * @brief Simple scanf implementation.
 *
 * @date 10.02.09
 * @author Alexandr Batyukov
 *          - Initial implementation
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdbool.h>

#include <framework/mod/options.h>

/*TODO: throw out.*/
/**
 * convert digit character to integer
 * @param digit character for converting
 * @param base for converting
 * @return converted symbol
 * @return -1 if error
 */
int ch_to_digit(char ch, int base) {
	ch = toupper(ch);
	switch (base) {
	case 16: {
		if (ch >= '0' && ch <= '9') {
			return (ch - '0');
		} else if (ch >= 'A' && ch <= 'F') {
			return (ch - 'A' + 0x0A);
		}
		return -1;
	}
	case 10: {
		if (ch >= '0' && ch <= '9') {
			return (ch - '0');
		}
		return -1;
	}
	case 8: {
		if (ch >= '0' && ch <= '7') {
			return (ch - '0');
		}
		return -1;
	}
	default:
		return -1;
	}
	return -1;
}
extern void __stdio_unscanchar(const char **str, int ch, int *pc_ptr);
extern int __stdio_scanchar(const char **str, int *pc_ptr);

static bool is_space(int ch) {
	if ((ch == ' ') || (ch == '\t') || (ch == '\n'))
		return true;
	return false;
}

static int trim_leading(const char **str, int *pc_ptr) {
	int ch;

	do {
		ch = __stdio_scanchar(str, pc_ptr);
		/*when break*/
		if (ch == EOF)
			break;
	} while (is_space(ch));

	__stdio_unscanchar(str, ch, pc_ptr);
	return ch;
}

static int scan_int(const char **in, int base, int width, int *res, int *pc_ptr) {
	int neg = 0;
	int dst = 0;
	int ch;
	int i;
	int not_empty = 0;

	if (EOF == (ch = trim_leading(in, pc_ptr))) {
		return EOF;
	}

	if ((ch == '-') || (ch == '+')) {
		neg = (ch == '-');
		__stdio_scanchar(in, pc_ptr);
	} else {
		dst = 0;
	}

	for (i = 0; (ch = (int) __stdio_scanchar(in, pc_ptr)) != EOF; i++) {
		if (!(base == 10 ? isdigit(ch) : isxdigit(ch)) || (0 == width)) {
			__stdio_unscanchar(in, ch, pc_ptr);
			/*end conversion*/
			break;
		}
		not_empty = 1;
		dst = base * dst + ch_to_digit(ch, base);
	}

	if (!not_empty) {
		return -1;
	}

	if (neg)
		dst = -dst;
	*res = dst;
	return 0;
}
#define OPS_LEN_MIN           0x00000040 /* s_char (d, i); u_char (u, o, x, X); s_char* (n) */
#define OPS_LEN_SHORT         0x00000080 /* short (d, i); u_short (u, o, x, X); short* (n) */
#define OPS_LEN_LONG          0x00000100 /* long (d, i); u_long (u, o, x, X); wint_t (c); wchar_t(s); long* (n) */
#define OPS_LEN_LONGLONG      0x00000200 /* llong (d, i); u_llong (u, o, x, X); llong* (n) */
#define OPS_LEN_MAX           0x00000400 /* intmax_t (d, i); uintmax_t (u, o, x, X); intmax_t* (n) */
#define OPS_LEN_SIZE          0x00000800 /* size_t (d, i, u, o, x, X); size_t* (n) */
#define OPS_LEN_PTRDIFF       0x00001000 /* ptrdiff_t (d, i, u, o, x, X); ptrdiff_t* (n) */
#define OPS_LEN_LONGFP        0x00002000 /* long double (f, F, e, E, g, G, a, A) */

int stdio_scan(const char **in, const char *fmt, va_list args) {
	int width;
	int converted = 0;
	int ops_len;
	int err;
	int pc=0;

	while (*fmt != '\0') {
		if (*fmt == '%') {
			fmt++;
			width = 80;

			if (*fmt == '\0')
				break;

			if (isdigit((int) *fmt))
				width = 0;

			while (isdigit((int) *fmt)) {

				width = width * 10 + (*fmt++ - '0');
			}

			ops_len = 0;
			switch (*fmt) {
			case 'h': ops_len = *++fmt != 'h' ? OPS_LEN_SHORT : (++fmt, OPS_LEN_MIN); break;
			case 'l': ops_len = *++fmt != 'l' ? OPS_LEN_LONG : (++fmt, OPS_LEN_LONGLONG); break;
			case 'j': ops_len = OPS_LEN_MAX; ++fmt; break;
			case 'z': ops_len = OPS_LEN_SIZE; ++fmt; break;
			case 't': ops_len = OPS_LEN_PTRDIFF; ++fmt; break;
			case 'L': ops_len = OPS_LEN_LONGFP; ++fmt; break;
			}
			if (*fmt == '\0')
				break;

			switch (*fmt) {
			case 's': {
				char *dst = va_arg(args, char*);
				char ch;
				while (isspace(ch = __stdio_scanchar(in, &pc)));

				while (ch != (char) EOF && width--) {
					if (isspace(ch)) {
						__stdio_unscanchar(in, ch, &pc);
						break;
					}

					width--;
					*dst++ = (char) ch;
					ch = __stdio_scanchar(in, &pc);
				}

				if (width == 80) // XXX
					converted = EOF;
				else {
					*dst = '\0';
					++converted;
				}
			}
				break;
			case 'c': {
				int dst;

				dst = __stdio_scanchar(in, &pc);
				*va_arg(args, char*) = dst;

				if (dst == (char) EOF)
					converted = EOF;
				else
					++converted;
			}
				break;
			case 'u': {
				int dst;
				if (0 != (err = scan_int(in, 10, width, &dst, &pc))) {
					if (err == EOF)
						converted = EOF;
					goto out;
				}

				switch (ops_len) {
				default: /* FIXME */
					*va_arg(args, unsigned int*) = dst;
					break;
				case OPS_LEN_MIN:
					*va_arg(args, unsigned char*) = dst;
					break;
				case OPS_LEN_SHORT:
					*va_arg(args, unsigned short*) = dst;
					break;
				}

				++converted;
			}
				break;
			case 'f': /* TODO float scanf haven't realized */
			case 'd': {
				int dst;
				if (0 != (err = scan_int(in, 10, width, &dst, &pc))) {
					if (err == EOF)
						converted = EOF;
					goto out;
				}

				switch (ops_len) {
				default: /* FIXME */
					memcpy(va_arg(args, int*), &dst, sizeof(dst));
					break;
				case OPS_LEN_MIN:
					*va_arg(args, char*) = dst;
					break;
				case OPS_LEN_SHORT:
					*va_arg(args, short*) = dst;
					break;
				}

				++converted;
			}
				break;
			case 'o': {
				int dst;
				if (0 != (err = scan_int(in, 8, width, &dst, &pc))) {
					if (err == EOF)
						converted = EOF;
					goto out;
				}

				*va_arg(args, int*) = dst;

				++converted;
			}
				break;
			case 'x': {
				int dst;
				if (0 != (err = scan_int(in, 16, width, &dst, &pc))) {
					if (err == EOF)
						converted = EOF;
					goto out;
				}
				*va_arg(args, int*) = dst;

				++converted;
			}
				break;
			case 'n': {
				*va_arg(args, int *) = pc;	//%n only allows for int*
			}
				break;
			}
			fmt++;
		} else {
			char ch = __stdio_scanchar(in, &pc);
			if (*fmt++ != ch) {
				return converted;
			}
		}
	}

out:
	return converted;
}

int sscanf(const char *out, const char *format, ...) {
	va_list args;
	int rv;

	va_start(args, format);
	rv = stdio_scan(&out, format, args);
	va_end (args);

	return rv;
}
