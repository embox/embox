/*
 * scanf.c
 *	Realization of function scanf
 *  Created on: 10.02.2009
 *      Author: abatyukov
 */
#include "types.h"
#include "conio.h"
#include "stdarg.h"


const int EOF = -1;

int getchar() {
	static char prev = 0;

	prev = uart_getc();
	return (int) prev;
}

static void ungetchar(int ch) {
	uart_putc(ch);
}

static int scanchar(char **str) {
	extern int getchar();
	if (str) {
		/* **str = getchar();
		++(*str);
		return **str;*/
		int ch;
		ch = str[0][0];
		++(*str);
		return ch;

	} else {
		return getchar();
	}
}
BOOL isspace(int ch) {
	if (ch == ' ')
		return TRUE;
	if (ch == '\t')
		return TRUE;
	if (ch == '\n')
		return TRUE;
	return FALSE;
}

BOOL isdigit(int ch, int base) {
	//toupcase();
	switch (base) {
		case 10:
		{
			if ((ch >= '0') && (ch <= '9'))
				return TRUE;
			return FALSE;
		}
		case 8:
		{
			if ((ch >= '0') && (ch <= '7'))
				return TRUE;
			return FALSE;
		}
		case 16:
		{
			if (((ch >= '0') && (ch <= '9')) || ((ch >= 'A') && (ch <= 'F')))
				return TRUE;
			return FALSE;
		}
		default:
			return FALSE;
	}
	return FALSE;
}


static int trim_leading(char **str) {
	int ch;

	do {
		ch = scanchar(str);
		if (ch == EOF)
			break;
	} while (isspace(ch));

	ungetchar(ch);

	return ch;
}

static int scan_int(char **in, int base, int widht) {
	int neg = 0;
	int dst = 0;
	int ch;
	int i;

	trim_leading(in);

	for (i = 0; (ch = scanchar(in)) != EOF; i++) {
		if (i == 0 && (ch == '-' || ch == '+')) {
			neg = (ch == '-');
			continue;
		}

		if (!isdigit( ch , base ) || widht--) {
			ungetchar(ch);
			break;
		}
		//for different bases
		if (base >10)
			dst = base * dst + (ch - '0' - 7);
		else
			dst = base * dst + (ch - '0');
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
		if (*fmt++ == '%') {
			widht = 80;


			if (*fmt == '\0')
				break;

			if (isdigit((int)*fmt, 10)) widht = 0;

			while (isdigit((int)*fmt++, 10)) {
				widht = widht * 10 + (*fmt - '0');
			}

			switch (*fmt) {
			case 's': {
				char *dst = va_arg ( args, char* );
				int ch;

				trim_leading(in);//???

				while (EOF != (ch = scanchar(in)) && !isspace(ch) && widht--)
					*dst++ = (char) ch;
				*dst = '\0';

				++converted;
			}
				continue;
			case 'c':{
				int dst;

				trim_leading(in);

				dst = scanchar(in);
				va_arg ( args, char) = dst;
				++converted;

			}
				continue;
			case 'd': {
				int dst;
				dst = scan_int(in,10,widht);
				va_arg ( args, int ) = dst;
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
			case 'u': {
				int dst;
				dst = scan_int(in,8,widht);
				va_arg ( args, int ) = dst;
				++converted;
			}
				continue;
			/*case 'U': {
				double dst;
				dst = scan_double(in,8,widht);
				va_arg ( args, int ) = dst;
				++converted;
			}
				continue;*/
			case 'x': {
				int dst;
				dst = scan_int(in,16,widht);
				va_arg ( args, int ) = dst;
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
		}
	}

	return converted;
}


int scanf(const char *format, ...) {

	va_list args;
	int rv;
	va_start ( args, format );
	rv = scan( 0,format, args );
	va_end ( args );

	return rv;
}

int sscanf(char *out, const char *format, ...)
 {
	va_list args;
	int rv;
	va_start ( args, format );
	rv = scan( &out,format, args );
	va_end ( args );

	return rv;
 }

