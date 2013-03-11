/**
 * @file
 * @brief TODO
 *
 * @date 24.02.10
 * @author Eldar Abusalimov
 */

#include <embox/cmd.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <framework/mod/api.h>
#include <errno.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: lsmod [-qhpn]\n");
}

static void mod_print(const struct mod *mod) {
	printf("%s.%s ", mod->package->name, mod->name);

}

static int exec(int argc, char **argv) {
	const struct mod *mod, *dep;
	int quiet = 0;
	const char *substr_package = NULL, *substr_name = NULL;

	int opt;
	getopt_init();
	while (-1 != (opt = getopt(argc, argv, "qhp:n:"))) {
		switch (opt) {
		case 'q':
			quiet = 1;
			break;
		case 'h':
			print_usage();
			return 0;
		case 'p':
			substr_package = optarg;
			break;
		case 'n':
			substr_name = optarg;
			break;
		case '?':
			break;
		default:
			return -EINVAL;
		}
	}

	printf("\n");
	mod_foreach(mod) {
		if ((substr_package && !strstr(mod->package->name, substr_package))
				|| (substr_name && !strstr(mod->name, substr_name))) {
			continue;
		}
		mod_print(mod);
		printf("\n");

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
