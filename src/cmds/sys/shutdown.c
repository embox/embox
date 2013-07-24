/**
 * @file
 *
 * @date Oct 29, 2012
 * @author: Anton Bondarev
 */

#include <embox/cmd.h>
#include <unistd.h>
#include <hal/arch.h>

EMBOX_CMD(exec);

static int exec(int argc, char **argv) {
	arch_shutdown(0);
	return 0;
}
