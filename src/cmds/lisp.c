/**
 * @file
 * @brief C
 *
 * @date 10.09.11
 * @author Anton BOndarev
 */

#include <embox/cmd.h>
#include <getopt.h>
#include <types.h>

EMBOX_CMD(exec);

extern int lisp5000_main(int argc, char *argv[]);

static int exec(int argc, char **argv) {
	return lisp5000_main(argc, argv);
}
