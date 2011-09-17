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
	char *av[argc + 2];
	for (int i = argc; i > 0; i--) {
		av[i + 1] = argv[i];
	}
	av[1] = "init500lite.lisp";
	return lisp5000_main(argc + 1, av);
}
