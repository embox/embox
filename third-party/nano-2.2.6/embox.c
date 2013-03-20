/**
 * @file
 * @brief
 *
 * @date 08.03.13
 * @author Ilia Vaprol
 */

#include <embox/cmd.h>
#include "embox.h"
#include <assert.h>

int nano_exit_ret;
int nano_exited;
jmp_buf nano_exit_point;

EMBOX_CMD(exec);

static int exec(int argc, char **argv) {
	int ret;

	nano_exit_ret = -1;
	nano_exited = 0;

	ret = setjmp(nano_exit_point);
	if (ret == 0) {
		return nano_main(argc, argv);
	}

	assert(ret == NANO_EXIT_JMP_VALUE);
	assert(nano_exited);

	return nano_exit_ret;
}
