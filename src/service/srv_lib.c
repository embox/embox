/**
 * @file
 *
 * @date Nov 7, 2012
 * @author: Anton Bondarev, Vita Loginova
 */

#include <embox/web_service.h>
#include <kernel/thread.h>
#include <mem/misc/pool.h>
#include <string.h>
#include <kernel/task.h>
#include <unistd.h>
#include <util/array.h>

ARRAY_SPREAD_DEF(const struct web_service_desc, __web_services_repository);

//POOL_DEF(instance_pool, struct web_service_instance, 0x10);

static DLIST_DEFINE(run_instances);

struct web_service_instance *web_service_lookup(const char *srv_name) {
	struct web_service_instance *inst;

	dlist_foreach_entry(inst, &run_instances, lst) {
		if (0 == strcmp(srv_name, inst->desc->srv_name)) {
			return inst;
		}
	}
	return NULL;
}

const struct web_service_desc *web_service_desc_lookup(const char *srv_name) {
	const struct web_service_desc *srv_desc;

	array_spread_foreach_ptr(srv_desc, __web_services_repository) {
		if (0 == strcmp(srv_name, srv_desc->srv_name)) {
			return srv_desc;
		}
	}

	return NULL;
}

int web_service_add(const char *srv_name) {
	const struct web_service_desc *srv_desc;

	if (NULL != web_service_lookup(srv_name)) {
		return -1;
	}
	if (NULL == (srv_desc = web_service_desc_lookup(srv_name))) {
		return -1;
	}
	*srv_desc->is_started = 1;

	return 0;
}

/*int web_service_remove(struct web_service_instance *srv_inst) {
	dlist_del(&srv_inst->lst);
	service_free_service_data(srv_inst->srv_data);
	pool_free(&instance_pool, srv_inst);
	return 0;
}*/

void web_service_remove_all() {
	//struct web_service_instance *inst;

	/*dlist_foreach_entry(inst, &run_instances, lst)
	{
		web_service_remove(inst);
	}*/
	const struct web_service_desc *srv_desc;

	array_spread_foreach_ptr(srv_desc, __web_services_repository) {
		*srv_desc->is_started = 0;
	}
}

int web_service_start_service(const char *srv_name,
		struct service_data * srv_data) {
	const struct web_service_desc *srv_desc;

	srv_desc = web_service_desc_lookup(srv_name);
	if (srv_desc == NULL) {
		return -1;
	}
	if (!*srv_desc->is_started) {
		return -1;
	}

	if (new_task(srv_name, srv_desc->run, (void *)srv_data) < 0) {
		return -1;
	}

	close(srv_data->sock);

	return 0;
}

int is_service_started(const char *srv_name) {
	const struct web_service_desc *srv_desc;
	srv_desc = web_service_desc_lookup(srv_name);
	if (srv_desc == NULL) {
		return 0;
	}
	if (!*srv_desc->is_started){
		return 0;
	}
	return 1;
}
