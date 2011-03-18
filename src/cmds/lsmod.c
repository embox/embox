/**
 * @file
 * @brief TODO
 *
 * @date 24.02.10
 * @author Eldar Abusalimov
 */

#include <types.h>
#include <embox/cmd.h>
#include <getopt.h>
#include <mod/core.h>

EMBOX_CMD(exec);

#if 0
static void print_usage(void) {
	printf("Usage: lsmod\n");
}
#endif

static void mod_print(const struct mod *mod, int depth) {
	const struct mod *dep;
	int i;

	for (i = 0; i < depth - 1; ++i) {
		printf("|  ");
	}
	if (depth > 0) {
		printf("|- ");
	}
	printf("%s.%s\n", mod->package->name, mod->name);

	mod_foreach_requires(dep, mod) {
		mod_print(dep, depth + 1);
	}
}

static int exec(int argc, char **argv) {
	extern const struct mod *__mods_start, *__mods_end;
	const struct mod **p_mod;

	for (p_mod = &__mods_start; p_mod < &__mods_end; ++p_mod) {
		mod_print(*p_mod, 0);
	}

	return 0;
}
