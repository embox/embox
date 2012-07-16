/**
 * @file
 *
 * @brief
 *
 * @date 17.03.2012
 * @author Anton Kozlov
 */

#include <prom/diag.h>
#include <string.h>

#define BUF_LEN 0x100

static char diag_buf[BUF_LEN + 1]; /* for trailing \0 */
static int diag_buf_head = 0;

char *diag_get_first_part(void) {
	return diag_buf + diag_buf_head + 1;
}

char *diag_get_last_part(void) {
	return diag_buf;
}

void diag_init(void) {
	diag_buf_head = 0;
	memset(diag_buf, 0, BUF_LEN + 1);
}

char diag_getc(void) {
	return 0;

}

void diag_putc(char ch) {
	if (diag_buf_head == BUF_LEN) {
		diag_buf_head = 0;
	}

	diag_buf[diag_buf_head] = ch;
	diag_buf[++diag_buf_head] = '\0';

}

int diag_has_symbol(void) {
	return 0;
}
