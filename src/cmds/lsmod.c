/**
 * @file
 * @brief TODO
 *
 * @date 24.02.10
 * @author Eldar Abusalimov
 */

#include <embox/cmd.h>
#include <getopt.h>
#include <stdio.h>
#include <framework/mod/api.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: lsmod [-qh]\n");
}

static void mod_print(const struct mod *mod) {
	printf("%s.%s ", mod->package->name, mod->name);

}

static int exec(int argc, char **argv) {
	const struct mod *mod, *dep;
	int quiet = 0;

	int opt;
	getopt_init();
	while (-1 != (opt = getopt(argc, argv, "qh"))) {
		switch (opt) {
		case 'q':
			quiet = 1;
			break;
		case 'h':
			print_usage();
			return 0;
		case '?':
			break;
		default:
			return -1;
		}
	}

	mod_foreach(mod) {
		printf("\n");
		mod_print(mod);

		if (!quiet) {
			printf("\n\t-> ");
			mod_foreach_requires(dep, mod) {
				mod_print(dep);
			}

			printf("\n\t<- ");
			mod_foreach_provides(dep, mod) {
				mod_print(dep);
			}

			printf("\n");
		}
	}

	return 0;
}
