/**
 * @file
 *
 * @date Mar 24, 2022
 * @author Anton Bondarev
 */

#include <wchar.h>
#include <inttypes.h>
#include <string.h>
#include <stdio.h>

#include <unistd.h>
#include <stdio/internal/file_struct.h>

wint_t putwc(wchar_t wc, FILE *f) {
	char ch = (char) wc;

	write(f->fd,&ch, 1);

	return 0;
}
