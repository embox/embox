/**
 * @file
 *
 * @date Nov 7, 2012
 * @author: Anton Bondarev, Vita Loginova
 */

#include <embox/web_service.h>
#include <kernel/thread/api.h>
#include <mem/misc/pool.h>
#include <string.h>
#include <kernel/task.h>
#include <fcntl.h>

ARRAY_SPREAD_DEF(const struct web_service_desc, __web_services_repository);

POOL_DEF(instance_pool, struct web_service_instance, 0x10);

static DLIST_DEFINE(run_instances);

static void *service_thread_handler(void* args) {
	struct web_service_instance *inst;

	inst = (struct web_service_instance *) args;

	while (1) {
		inst->thread_started = 1;
		event_wait(inst->e, EVENT_TIMEOUT_INFINITE);
		if (inst->params == NULL) {
			inst->params = (void *) 1;
			return NULL;
		}
		inst->desc->run(inst);


	}

	return NULL;
}

void *web_service_trampoline(void *param) {
	struct web_service_instance * inst;
	struct event event;

	inst = (struct web_service_instance *) param;
	inst->e = &event;
	event_init(inst->e, "web_srv_event");

	service_thread_handler(inst);

	return NULL;
}

struct web_service_instance *web_service_lookup(const char *srv_name) {
	struct web_service_instance *inst, *tmp;

	dlist_foreach_entry(inst, tmp, &run_instances, lst) {
		if (0 == strcmp(srv_name, inst->desc->srv_name)) {
			return inst;
		}
	}
	return NULL;
}

const struct web_service_desc *web_service_desc_lookup(const char *srv_name) {
	int i;

	for (i = 0; i < ARRAY_SPREAD_SIZE(__web_services_repository); i++) {
		if (0 == strcmp(srv_name, __web_services_repository[i].srv_name)) {
			return &__web_services_repository[i];
		}
	}
	return NULL;
}

int web_service_add(const char *srv_name) {
	struct web_service_instance *inst;
	const struct web_service_desc *srv_desc;

	if (NULL != web_service_lookup(srv_name)) {
		return -1;
	}
	if (NULL == (srv_desc = web_service_desc_lookup(srv_name))) {
		return -1;
	}
	if (NULL == (inst = pool_alloc(&instance_pool))) {
		return -1;
	}
	inst->desc = srv_desc;

	/*if (0 != thread_create(&inst->thr, THREAD_FLAG_DETACHED,
	 web_service_trampoline, inst)) {
	 return -1;
	 }*/
	dlist_add_next(dlist_head_init(&inst->lst), &run_instances);
	return 0;
}

int send_message(struct web_service_instance *srv_inst, void *par) {
	while (!srv_inst->thread_started)
		;
	srv_inst->params = par;
	event_notify(srv_inst->e);

	return 0;
}

int web_service_remove(struct web_service_instance *srv_inst) {
	send_message(srv_inst, NULL);

	while ((int) srv_inst->params != 1)
		;
	dlist_del(&srv_inst->lst);
	pool_free(&instance_pool, srv_inst);
	return 0;
}

int web_service_stop(const char *srv_name) {
	struct web_service_instance *srv_inst;

	if (NULL == (srv_inst = web_service_lookup(srv_name))) {
		return -1;
	}
	return web_service_remove(srv_inst);
}

int web_service_send_message(const char *srv_name, void *par) {
	struct web_service_instance *srv_inst;

	if (NULL == (srv_inst = web_service_lookup(srv_name))) {
		return -1;
	}
	return send_message(srv_inst, par);
}

void web_service_remove_all() {
	struct web_service_instance *inst, *tmp;

	dlist_foreach_entry(inst, tmp, &run_instances, lst)
	{
		web_service_remove(inst);
	}
}

int web_service_start_service(const char *srv_name,
		struct service_data * srv_data) {
	struct web_service_instance *inst;
	const struct web_service_desc *srv_desc;

	if (NULL == (srv_desc = web_service_desc_lookup(srv_name))) {
		return -1;
	}

	if (NULL == (inst = pool_alloc(&instance_pool))) {
		return -1;
	}
	inst->desc = srv_desc;

	new_task(inst->desc->run, (void *) srv_data);
	/* When we closing http connection after content sending
	 * this means socket must be opened only in one task.  */
	close(srv_data->sock);

	dlist_add_next(dlist_head_init(&inst->lst), &run_instances);
	return 0;
}

int is_service_started(const char *srv_name) {
	const struct web_service_desc *srv_desc;
	if (NULL == (srv_desc = web_service_desc_lookup(srv_name))) {
		return 0;
	}
	return 1;
}
