/**
 * @file
 * @brief
 *
 * @author  Denis Deryugin
 * @date    4 Aug 2014
 */

#include "fork_copy_addr_space.h"
#include <kernel/task/resource/task_fork.h>
#include <kernel/task/resource/module_ptr.h>
#include <sys/types.h>
#include <mem/sysmalloc.h>
#include <framework/mod/types.h>
#include <string.h>

static inline const struct mod_app *task_app_get(void) {
	const struct mod *mod = task_module_ptr_get(task_self());
	return mod ? mod->app : NULL;
}

void fork_static_store(struct static_space *sspc) {
	const struct mod_app *app;

	app = task_app_get();
	if (!app) {
		return;
	}

	if (!sspc->bss_store && app->bss_sz) {
		sspc->bss_store = sysmalloc(app->bss_sz);
		assert(sspc->bss_store);
	}
	memcpy(sspc->bss_store, app->bss, app->bss_sz);

	if (!sspc->data_store && app->data_sz) {
		sspc->data_store = sysmalloc(app->data_sz);
		assert(sspc->data_store);
	}
	memcpy(sspc->data_store, app->data, app->data_sz);
}

void fork_static_restore(struct static_space *sspc) {
	const struct mod_app *app;

	app = task_app_get();
	if (!app) {
		return;
	}

	if (app->bss_sz) {
		assert(sspc->bss_store);
		memcpy(app->bss, sspc->bss_store, app->bss_sz);
	}

	if (app->data_sz) {
		assert(sspc->data_store);
		memcpy(app->data, sspc->data_store, app->data_sz);
	}
}

void fork_static_cleanup(struct static_space *sspc) {

	if (sspc->bss_store) {
		sysfree(sspc->bss_store);
	}

	if (sspc->data_store) {
		sysfree(sspc->data_store);
	}

	sspc->bss_store = NULL;
	sspc->data_store = NULL;
}

