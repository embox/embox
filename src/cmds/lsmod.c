/**
 * @file
 * @brief TODO
 *
 * @date 24.02.10
 * @author Eldar Abusalimov
 */

#include <embox/cmd.h>

#include <stdio.h>

#include <framework/mod/api.h>

EMBOX_CMD(exec);

#if 0
static void print_usage(void) {
	printf("Usage: lsmod\n");
}
#endif

static void mod_print(const struct mod *mod) {
	printf("%s.%s ", mod->package->name, mod->name);

}

static int exec(int argc, char **argv) {
	const struct mod *mod, *dep;

	mod_foreach(mod) {
		printf("\n");
		mod_print(mod);

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

	return 0;
}
