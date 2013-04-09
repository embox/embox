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

EMBOX_CMD(exec);

static int exec(int argc, char **argv) {
	return nano_main(argc, argv);
}
