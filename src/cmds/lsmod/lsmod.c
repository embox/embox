/**
 * @file
 * @brief TODO
 *
 * @date 24.02.2010
 * @author Eldar Abusalimov
 */

#include <types.h>

#include <shell_command.h>
#include <mod/core.h>

static const char *man_page =
		"I am too lazy to fill these stupid man pages. \n"
		"Author: Eldar Abusalimov";

DECLARE_SHELL_COMMAND("lsmod", exec,
		"lists mod info emitted by EMBuild",
		"Usage: lsmod",
		man_page);

static void mod_print(const struct mod *mod, int depth) {
	struct mod_iterator iterator;
	int i;

	for (i = 0; i < depth - 1; ++i) {
		printf("|  ");
	}
	if (depth > 0) {
		printf("|- ");
	}
	printf("%s.%s\n", mod->package->name, mod->name);

	mod_requires(mod, &iterator);
	while(mod_iterator_has_next(&iterator)) {
		mod_print(mod_iterator_next(&iterator), depth + 1);
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
