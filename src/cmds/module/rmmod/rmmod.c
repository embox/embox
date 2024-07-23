/**
 * @file
 * @brief TODO
 *
 * @date 15.03.13
 * @author Eldar Abusalimov
 */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <framework/mod/api.h>
#include <errno.h>

static void print_usage(void) {
	printf("Usage: rmmod <mod_name>\n");
}

static void fake_disable(const struct mod *mod) {
	const struct mod *dep;
	int enabled = mod_is_running(mod); // XXX fix later

	if (!enabled) {
		return;
	}

	mod->priv->stat &= ~(mod_stat_t)MOD_STAT_ENABLED;
	mod_foreach_provides(dep, mod) {
		fake_disable(dep);
	}

	printf("    %s.%s\n", mod_pkg_name(mod), mod_name(mod));
}

int main(int argc, char **argv) {
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

	fake_disable(mod);

	return 0;
}
