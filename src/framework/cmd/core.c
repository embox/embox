/**
 * @file
 * @brief Command registry and invocation code.
 *
 * @date 01.03.11
 * @author Eldar Abusalimov
 */

#include <ctype.h>
#include <errno.h>
#include <stddef.h>
#include <string.h>

#include <framework/cmd/api.h>
#include <framework/cmd/types.h>
#include <framework/mod/options.h>
#include <kernel/addr_space.h>
#include <lib/libds/array.h>
#include <util/getopt.h>

#define APP_DATA_RESERVE_OFFSET                       \
	({                                                \
		extern char _app_reserve_vma, _app_data_vma;  \
		(size_t)(&_app_reserve_vma - &_app_data_vma); \
	})

ARRAY_SPREAD_DEF(const struct cmd *const, __cmd_registry);

static int cmd_mod_init(const struct mod *mod) {
	const struct mod_app *app;
	int i;

	app = ((const struct cmd_mod *)mod)->app;

	if (app) {
		if (app->data_vma == app->data_lma) {
			/* No need to traverse deps here, they are already initialized. */
			memcpy(app->data_vma + APP_DATA_RESERVE_OFFSET, app->data_vma,
			    app->data_sz);

			for (i = 0; app->build_dep_data_vma[i]; i++) {
				assert(app->build_dep_data_vma[i] == app->build_dep_data_lma[i]);

				memcpy(app->build_dep_data_vma[i] + APP_DATA_RESERVE_OFFSET,
				    app->build_dep_data_vma[i], app->build_dep_data_sz[i]);
			}
		}
		else {
			/* No need to traverse deps here, they are already initialized. */
			memcpy(app->data_vma, app->data_lma, app->data_sz);

			for (i = 0; app->build_dep_data_vma[i]; i++) {
				memcpy(app->build_dep_data_vma[i], app->build_dep_data_lma[i],
				    app->build_dep_data_sz[i]);
			}
		}
	}

	return 0;
}

const struct mod_ops __cmd_mod_ops = {
    .enable = cmd_mod_init,
};

/**
 * Loads static data sections from initialization image for app modules,
 * does noting for the rest. Module must be running.
 */
static int cmd_activate_app(const struct mod *mod) {
	const struct mod_app *app;
	int i;

	if (!mod_is_running(mod)) {
		return -ENOENT;
	}

	app = ((const struct cmd_mod *)mod)->app;

	if (app) {
		if (app->data_vma == app->data_lma) {
			memcpy(app->data_vma, app->data_vma + APP_DATA_RESERVE_OFFSET,
			    app->data_sz);
		}
		else {
			memcpy(app->data_vma, app->data_lma, app->data_sz);
		}
		memset(app->bss, 0, app->bss_sz);

		if (app->data_vma == app->data_lma) {
			for (i = 0; app->build_dep_data_vma[i]; i++) {
				memcpy(app->build_dep_data_vma[i],
				    app->build_dep_data_vma[i] + APP_DATA_RESERVE_OFFSET,
				    app->build_dep_data_sz[i]);
			}
		}
		else {
			for (i = 0; app->build_dep_data_vma[i]; i++) {
				memcpy(app->build_dep_data_vma[i], app->build_dep_data_lma[i],
				    app->build_dep_data_sz[i]);
			}
		}

		for (i = 0; app->build_dep_bss[i]; i++) {
			memset(app->build_dep_bss[i], 0, app->build_dep_bss_sz[i]);
		}
	}

	return 0;
}

int cmd_exec(const struct cmd *cmd, int argc, char **argv) {
	int err;

	if (!cmd) {
		return -EINVAL;
	}

	if ((err = cmd_activate_app(cmd2mod(cmd)))) {
		return err;
	}

	getopt_init();

	ADDR_SPACE_PREPARE_APP(cmd2mod(cmd));

	err = cmd->exec(argc, argv);

	ADDR_SPACE_FINISH_APP();

	/* FIXME Here we make app's data and bss as they was
	 * before app execution. It's required because we call all
	 * C++ ctors on every app launch. When we will call only ctors
	 * of the running app, this workaround can be removed. */
	cmd_activate_app(cmd2mod(cmd));

	return err;
}

const struct cmd *cmd_lookup(const char *name) {
	const struct cmd *cmd = NULL;

	if (!strncmp(name, "/bin/", strlen("/bin/"))) {
		name += strlen("/bin/");
	}

	cmd_foreach(cmd) {
		if (strcmp(cmd_name(cmd), name) == 0) {
			return cmd;
		}
	}

	return NULL;
}
