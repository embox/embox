/**
 * @file
 * @brief CLDC java virtual machine
 *
 * @date 24.12.12
 * @author Felix Sulima
 */

#include <embox/cmd.h>

#include <midp.h>

EMBOX_CMD(exec);

static int exec(int argc, char **argv) {
	return phoneme_midp(argc, argv);
}
