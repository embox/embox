/*
 * scanf.c
 *	Realization of function scanf
 *  Created on: 10.02.2009
 *      Author: abatyukov
 */
#include "asm/types.h"
#include "conio.h"
#include "asm/stdarg.h"
#include "string.h"
#include "ctype.h"

const int EOF = -1;

inline static int getchar() {
	return (int) uart_getc();
}

inline static void ungetchar(int ch) {
	uart_putc(ch);
}

static void unscanchar(char **str, int ch) {
     /*	extern int ungetchar();*/
	if (str) {
		//	*str --;
		//	**str = ch;

		/*int *p;
		 p = *str - 4;
		 *p = ch;
		 *str = p;*/

	} else {
		ungetchar(ch);
	}
}

static int scanchar(char **str) {
	extern int getchar();
	int ch;
	if (str) {
		ch = **str;
		(*str)++;
		return ch;

	} else {
		if ('\r' == (ch = getchar())) {
			return EOF;
		}
		return ch;
	}
}

static BOOL is_space(int ch) {
	if ((ch == ' ') || (ch == '\t') || (ch == '\n'))
		return TRUE;
	return FALSE;
}

static int trim_leading(char **str) {
	int ch;

	do {
		ch = scanchar(str);
		//when break
		if (ch == EOF)
			break;
	} while (is_space(ch));

	unscanchar(str, ch);

	return ch;
}

static int scan_int(char **in, int base, int widht) {
	int neg = 0;
	int dst = 0;
	int ch;
	int i;

	if (EOF == (ch = trim_leading(in)))
		return 0;//error

	if ((ch == '-') || (ch == '+')) {
		neg = (ch == '-');
	} else
		dst = ch_to_digit(ch, base);

	for (i = 0; (ch = (int) ch_upcase(scanchar(in))) != EOF; i++) {
		if (!isdigit(ch, base) || (0 == widht)) {
			unscanchar(in, ch);
			//end conversion
			break;
		}
		unscanchar(in, ch);

		dst = base * dst + ch_to_digit(ch, base);
	}

	if (neg)
		dst = -dst;
	return dst;
}
/*
 static double scan_double(char **in, int base) {
 int neg = 0;
 double dst = 0;
 int ch;
 int i;

 trim_leading(in);

 for (i = 0; (ch = scanchar(in)) != EOF; i++) {
 if (i == 0 && (ch == '-' || ch == '+')) {
 neg = (ch == '-');
 continue;
 }

 if (!isdigit( ch , base )) {
 ungetchar(ch);
 break;
 }
 //for different bases
 if (base >10)
 dst = dst * base + (ch - '0' - 7);
 else
 dst = dst * base + (ch - '0');
 }

 if (neg)
 dst = -dst;
 return dst;
 }
 */

static int scan(char **in, const char *fmt, va_list args) {
	int widht;
	int converted = 0;

	while (*fmt != '\0') {
		if (*fmt == '%') {
			fmt++;
			widht = 80;

			if (*fmt == '\0')
				break;

			if (isdigit((int) *fmt, 10))
				widht = 0;

			while (isdigit((int) *fmt, 10)) {

				widht = widht * 10 + (*fmt++ - '0');
			}

			switch (*fmt) {
			case 's': {
				char *dst = va_arg ( args, char* );
				int ch;

				//trim_leading(in);

				while (EOF != (ch = scanchar(in)) && widht--)
					*dst++ = (char) ch;
				*dst = '\0';

				++converted;
			}
				continue;
			case 'c': {
				int dst;

				//trim_leading(in);

				dst = scanchar(in);
				*va_arg ( args, char*) = dst;
				++converted;

			}
				continue;
			case 'd': {
				int dst;
				dst = scan_int(in, 10, widht);

				*va_arg ( args, int* ) = dst;

				++converted;
			}
				continue;
				/*case 'D': {
				 double dst;
				 dst = scan_double(in,10,widht);
				 va_arg ( args, int ) = dst;
				 ++converted;
				 }
				 continue;*/
			case 'o': {
				int dst;
				dst = scan_int(in, 8, widht);

				*va_arg ( args, int* ) = dst;

				++converted;
			}
				continue;
				/*case 'O': {
				 double dst;
				 dst = scan_double(in,8,widht);
				 va_arg ( args, int ) = dst;
				 ++converted;
				 }
				 continue;*/
			case 'x': {
				int dst;
				dst = scan_int(in, 16, widht);
				*va_arg ( args, int* ) = dst;

				++converted;
			}
				continue;
				/*case 'X': {
				 double dst;
				 dst = scan_double(in,16,widht);
				 va_arg ( args, int ) = dst;
				 ++converted;
				 }
				 continue;*/
			}
		} else {
			if (*fmt++ != *(*in)++) {
				return converted;
			}
		}
	}

	return converted;
}

int scanf(const char *format, ...) {

	va_list args;
	int rv;

	va_start ( args, format );
	rv = scan(0, format, args);
	va_end ( args );

	return rv;
}

int sscanf(char *out, const char *format, ...) {
	va_list args;
	int rv;

	va_start ( args, format );
	rv = scan(&out, format, args);
	va_end ( args );

	return rv;
}

