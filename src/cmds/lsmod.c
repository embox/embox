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
	printf("Usage: lsmod [-qdhpn]\n");
}

static void mod_print(const struct mod *mod) {
	int enabled = mod->priv->flags & 0x1; // XXX fix later
	printf(" %c  %s.%s ", enabled ? '*' : ' ', mod_pkg_name(mod), mod_name(mod));
}

static int exec(int argc, char **argv) {
	const struct mod *mod, *dep;
	const char *substr_package = NULL, *substr_name = NULL;
	int print_deps = 0;
	int opt;

	getopt_init();
	while (-1 != (opt = getopt(argc, argv, "qdhp:n:"))) {
		switch (opt) {
		case 'd':
			print_deps = 1;
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
		if ((substr_package && !strstr(mod_pkg_name(mod), substr_package)) ||
			(substr_name && !strstr(mod_name(mod), substr_name))) {
			continue;
		}
		mod_print(mod);
		printf("\n");

		if (print_deps) {
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
