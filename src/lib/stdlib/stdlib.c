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

