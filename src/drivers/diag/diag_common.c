/**
 * @file
 *
 * @brief
 *
 * @date 28.11.2011
 * @author Anton Bondarev
 */

#include <drivers/diag.h>

static char diag_default_getc(void) { return 0; }
static void diag_default_putc(char ch) { }
static int diag_default_kbhit(void) { return 0; }

const static struct diag_ops diag_default_ops = {
	.getc = &diag_default_getc,
	.putc = &diag_default_putc,
	.kbhit = &diag_default_kbhit
};

static const struct diag_ops *current_diag = &diag_default_ops;

char diag_getc(void) {
	return current_diag->getc();
}

void diag_putc(char ch) {
	current_diag->putc(ch);
}

int diag_kbhit(void) {
	return current_diag->kbhit();
}

const struct diag_ops * diag_common_set_ops(const struct diag_ops *new_diag) {
	const struct diag_ops *old_diag;
	old_diag = current_diag;
	current_diag = new_diag;
	return old_diag;
}
