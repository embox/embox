/**
 * @file
 *
 * @brief
 *
 * @date 17.03.2012
 * @author Anton Kozlov
 */

#include <drivers/diag.h>
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

static char diag_mem_getc(void) {
	return 0;
}

static void diag_mem_putc(char ch) {
	if (diag_buf_head == BUF_LEN) {
		diag_buf_head = 0;
	}

	diag_buf[diag_buf_head] = ch;
	diag_buf[++diag_buf_head] = '\0';

}

static int diag_mem_kbhit(void) {
	return 0;
}

static const struct diag_ops diag_mem_ops = {
	.getc = &diag_mem_getc,
	.putc = &diag_mem_putc,
	.kbhit = &diag_mem_kbhit
};

void diag_init(void) {
	diag_common_set_ops(&diag_mem_ops);
	diag_buf_head = 0;
	memset(diag_buf, 0, BUF_LEN + 1);
}
