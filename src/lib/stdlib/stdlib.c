/**
 * @file
 *
 * @date 20.11.09
 * @author Nikolay Korotky
 */

#include <ctype.h>
#include <types.h>
#include <stdlib.h>

/*TODO: throw out.*/
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
#if 1
double atof(const char *s) {
	char    c;
	double  val = 0, power = 1;
	unsigned char sign = 0;

skipws:
	c = *s;
	if(!isdigit(c) && c != '-' && c != '+') {
		s++;
		goto skipws;
	}
	if(c == '-') {
		sign++;
		s++;
	} else if(c == '+') {
		s++;
	}
	if(!isdigit(*s)) {
		goto skipws;
	}
val:
	c = *s;
	if(isdigit(c)) {
		val = val * 10 + (c - '0');
		s++;
		goto val;
	}

	if (c == '.') {
		s++;
	}
power:
	c = *s;
	if(isdigit(c)) {
		val = val * 10 + (c - '0');
		power *= 10;
		s++;
		goto power;
	}
	return (sign ? -1 : 1)*(val / power);
}
#endif
int atoi(const char *s) {
	char   c;
	int    a = 0;
	unsigned char sign = 0;

skipws:
	c = *s;
	if(!isdigit(c) && c != '-' && c != '+') {
		s++;
	goto skipws;
	}
	if(c == '-') {
		sign++;
		s++;
	} else if(c == '+') {
		s++;
	}
	if(!isdigit(*s)) {
		goto skipws;
	}
conv:
	c = *s;
	if(isdigit(c)) {
		a = a*10 + (c - '0');
		s++;
		goto conv;
	}
	return sign ? -a : a;
}
#if 1
long int atol(const char *nptr) {
	return strtol(nptr,(char **)NULL, 10);
}
#endif
