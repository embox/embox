/**
 * @file
 * @brief TODO
 *
 * @date 15.03.13
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
	printf("Usage: modprobe <mod_name>\n");
}

static void fake_enable(const struct mod *mod) {
	const struct mod *dep;
	int enabled = mod->priv->flags & 0x1; // XXX fix later

	if (enabled) {
		return;
	}

	mod->priv->flags |= 0x1;
	mod_foreach_requires(dep, mod) {
		fake_enable(dep);
	}

	printf(" *  %s.%s\n", mod_pkg_name(mod), mod_name(mod));
}

static int exec(int argc, char **argv) {
	const struct mod *mod;
	const char *name;

	if (argc != 2) {
		print_usage();
		return -EINVAL;
	}

	name = argv[1];
	mod = mod_lookup(name);

	if (!mod) {
		printf("No such module: %s\n", name);
		return -ENOENT;
	}

	fake_enable(mod);

	return 0;
}
