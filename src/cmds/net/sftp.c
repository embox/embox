/**
 * @file
 * @brief
 *
 * @date 31.08.12
 * @author Ilia Vaprol
 */

#include <embox/cmd.h>

EMBOX_CMD(exec);

extern int ftp_cmd(int argc, char **argv);

static int exec(int argc, char **argv) {
	return ftp_cmd(argc, argv);
}
