/**
 * @file
 * @brief
 *
 * @date 17.03.2012
 * @author Anton Kozlov
 */

#include <drivers/diag.h>
#include <string.h>

#include <framework/mod/options.h>

#define BUF_LEN OPTION_GET(NUMBER,buffer_len)

#if OPTION_GET(NUMBER, buffer_addr)
static char *const diag_buf = (char *const)OPTION_GET(NUMBER, buffer_addr);
#else
static char diag_buf[BUF_LEN];
#endif

static int diag_buf_head = 0;

static void diag_mem_putc(const struct diag *diag, char ch) {

	diag_buf[diag_buf_head++] = ch;

	if (diag_buf_head == BUF_LEN) {
		diag_buf_head = 0;
	}

	diag_buf[diag_buf_head] = '\0';
}

DIAG_OPS_DEF(
	.putc = diag_mem_putc,
);
