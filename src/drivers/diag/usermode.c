/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    01.03.2013
 */

#include <stddef.h>
#include <hal/ipl.h>
#include <kernel/host.h>

int diag_kbhit(void) {
	return 0;
}

char diag_getc(void) {
	int len = 1;

	emvisor_send(UV_PWRUPSTRM, EMVISOR_DIAG_IN, &len, sizeof(int));

	while (1);
	return '\0';
}

extern int ipl_num;

void diag_putc(char ch) {
	ipl_t ipl = ipl_save();
	int ret;

	if (0 > (ret = emvisor_send(UV_PWRUPSTRM, EMVISOR_DIAG_OUT, &ch, 1))) {
	}

	ipl_restore(ipl);
}

void diag_init(void) {

}
