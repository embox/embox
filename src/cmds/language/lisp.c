/**
 * @file
 * @brief C
 *
 * @date 10.09.11
 * @author Anton Bondarev
 */

#include <embox/cmd.h>

EMBOX_CMD(exec);

extern int lisp5000_main(int argc, char *argv[]);

static int exec(int argc, char **argv) {
	char *av[argc + 2];
	av[0] = argv[0];

	av[1] = "init500lite.lisp";

	for (int i = 1; i < argc; i++) {
		av[i + 1] = argv[i];
	}

	return lisp5000_main(argc + 1, av);
}
