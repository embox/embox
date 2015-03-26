/**
 * @file
 * @brief TODO
 *
 * @date 24.02.10
 * @author Eldar Abusalimov
 */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

#include <framework/mod/api.h>

static void print_usage(void) {
	printf("Usage: lsmod [-qdlhp:n:]\n");
}

static void mod_print(const struct mod *mod) {
	int enabled = mod->priv->flags & 0x1; // XXX fix later
	printf(" %c  %s.%s ", enabled ? '*' : ' ', mod_pkg_name(mod), mod_name(mod));
}

int main(int argc, char **argv) {
	const struct mod *mod, *dep;
	const char *substr_package = NULL, *substr_name = NULL;
	int print_deps = 0, show_label = 0;
	int opt;

	while (-1 != (opt = getopt(argc, argv, "qdlhp:n:"))) {
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
		case 'l':
			show_label = 1;
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
		if (show_label) {
			printf("\n\tlabel:%x:%x:%x:%x\n",
					(uint32_t)mod_label(mod)->text.vma,
					(uint32_t)mod_label(mod)->data.vma,
					(uint32_t)mod_label(mod)->bss.vma,
					(uint32_t)mod_label(mod)->rodata.vma);
		}

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
