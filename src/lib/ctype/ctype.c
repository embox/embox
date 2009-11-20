/**
 * \file ctype.c
 * \date 14.10.09
 * \author Sikmir
 */
#include "types.h"
#include "ctype.h"

char ch_upcase(char ch) {
        if (ch >= 'a' && ch <= 'z')
                return (ch + 'A' - 'a');
        else
                return ch;
}

int ch_to_digit(char ch, int base) {
        ch = ch_upcase(ch);
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

int isdigit_base(int ch, int base) {
	ch = (int) ch_upcase((char) ch);
	switch (base) {
	case 10: {
		if ((ch >= '0') && (ch <= '9'))
			return TRUE;
		return FALSE;
	}
	case 8: {
		if ((ch >= '0') && (ch <= '7'))
			return TRUE;
		return FALSE;
	}
	case 16: {
		if (((ch >= '0') && (ch <= '9')) || ((ch >= 'A') && (ch <= 'F')))
			return TRUE;
		return FALSE;
	}
	default:
		return FALSE;
	}
	return FALSE;
}

int isalpha(int ch) {
	return ((ch > 0x20) && (ch < 0x7F));
}
