/**
 * @file
 *
 * @date Nov 7, 2012
 * @author: Anton Bondarev
 */

#include <embox/web_service.h>
#include <kernel/thread/api.h>
#include <mem/misc/pool.h>
#include <string.h>

ARRAY_SPREAD_DEF(const struct web_service_desc, __web_services_repository);

POOL_DEF(instance_pool, struct web_service_instance, 0x10);

static DLIST_DEFINE(run_instances);

static void *service_thread_handler(void* args) {
	struct web_service_instance *inst;

	inst = 	(struct web_service_instance *)args;

	while (1) {
		event_wait(inst->e, EVENT_TIMEOUT_INFINITE);
		inst->desc->run(inst);
		event_notify(&inst->params->info->unlock_sock_event);
	}

	return NULL;
}

static void *web_service_trampoline(void *param) {
	struct web_service_instance * inst;
	struct event event;

	inst = (struct web_service_instance *)param;
	inst->e = &event;
	event_init(inst->e, "web_srv_event");


	service_thread_handler(inst);

	return NULL;
}

struct web_service_instance *web_service_lookup(const char *srv_name) {
	struct web_service_instance *inst, *tmp;

	dlist_foreach_entry(inst, tmp, &run_instances, lst) {
		if(0 == strcmp(srv_name, inst->desc->srv_name)) {
			return inst;
		}
	}
	return NULL;
}

const struct web_service_desc *web_service_desc_lookup(const char *srv_name) {
	int i;

	for (i = 0; i < ARRAY_SPREAD_SIZE(__web_services_repository); i++) {
		if(0 == strcmp(srv_name, __web_services_repository[i].srv_name)) {
			return &__web_services_repository[i];
		}
	}
	return NULL;
}

int web_service_start(const char *srv_name) {
	struct web_service_instance *inst;
	const struct web_service_desc *srv_desc;

	if(NULL == (srv_desc = web_service_desc_lookup(srv_name))) {
		return -1;
	}
	if(NULL == (inst = pool_alloc(&instance_pool))) {
		return -1;
	}
	inst->desc = srv_desc;

	if(0 != thread_create(&inst->thr, THREAD_FLAG_DETACHED, web_service_trampoline, inst)) {
		return -1;
	}
	dlist_add_next(dlist_head_init(&inst->lst), &run_instances);
	return 0;
}

int web_service_send_message(const char *srv_name, void *par) {
	struct web_service_instance *srv_inst;

	if(NULL == (srv_inst = web_service_lookup(srv_name))) {
		return -1;
	}
	srv_inst->params = par;
	event_notify(srv_inst->e);

	return 0;
}
