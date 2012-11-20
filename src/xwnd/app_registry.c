/**
 * @file
 * @brief XWnd application repository implementation
 *
 * @date Oct 18, 2012
 * @author Alexandr Chernakov
 */


#include <embox/unit.h>
#include <kernel/task.h>
#include <util/array.h>
#include <mem/misc/pool.h>
#include <stdlib.h>
#include <string.h>

#include <xwnd/app_registry.h>
#include <xwnd/event.h>
#include <xwnd/window.h>


ARRAY_SPREAD_DEF(const struct xwnd_app_desc, __xwnd_app_repository);

POOL_DEF(instance_pool, struct xwnd_app_instance, 0x10);

static DLIST_DEFINE(run_instances);

EMBOX_UNIT_INIT(xwnd_app_reg_init);

#define XAPP_REG_DEF_CNT 256

static struct xwnd_app_registry xapp_reg;


struct xwnd_app_registry * xwnd_app_reg(void) {
	return &xapp_reg;
}

static int xwnd_app_reg_init(void) {
	xapp_reg.nodes = malloc (XAPP_REG_DEF_CNT * sizeof(struct xwnd_app_registry_node));
	if (!xapp_reg.nodes) {
		return -1;
	}
	xapp_reg.event_sup = xwnd_event_init_supervisor(XAPP_REG_DEF_CNT);
	if (!xapp_reg.event_sup) {
		free(xapp_reg.nodes);
		return -1;
	}
	xapp_reg.allocated = XAPP_REG_DEF_CNT;
	xapp_reg.used = 0;

	return 0;
}

static int xwnd_app_create (void* (*entry_point) (void*), const char * arg) {
	int xapp_id = xapp_reg.used;
	int xapp_tid;

	/*FIXME: Get an entry in registry*/
	if (xapp_id >= XAPP_REG_DEF_CNT - 1) {
		return -1;
	}
	xapp_reg.used++;

	/*Initialize event queue connection*/
	//xwnd_event_create_pair(&(xapp_reg.nodes[xapp_id].ev), &(xapp_reg.nodes[xapp_id].init_wrap.ev));
	xapp_reg.nodes[xapp_id].ev_master = xwnd_event_get_supervised_pair (xapp_reg.event_sup, &(xapp_reg.nodes[xapp_id].init_wrap.ev));

	/*Send first messages*/
	xwnd_app_send_sys_event(xapp_id, XWND_EV_CREAT);
	xwnd_app_send_sys_event(xapp_id, XWND_EV_DRAW);
	xapp_reg.nodes[xapp_id].init_wrap.arg = arg;

	/*Now ready to start an application*/
	xapp_tid = new_task(entry_point, (void*)(&(xapp_reg.nodes[xapp_id].init_wrap)));
	xapp_tid = xapp_tid;
	return xapp_id;
}



struct xwnd_app_instance *xwnd_app_lookup(const char *app_name) {
	struct xwnd_app_instance *inst, *tmp;

	dlist_foreach_entry(inst, tmp, &run_instances, lst) {
		if(0 == strcmp(app_name, inst->desc->app_name)) {
			return inst;
		}
	}
	return NULL;
}

const struct xwnd_app_desc *xwnd_app_desc_lookup(const char *app_name) {
	int i;

	for (i = 0; i < ARRAY_SPREAD_SIZE(__xwnd_app_repository); i++) {
		if(0 == strcmp(app_name, __xwnd_app_repository[i].app_name)) {
			return &__xwnd_app_repository[i];
		}
	}
	return NULL;
}


int xwnd_app_start(const char *app_name, const char *arg) {
	const struct xwnd_app_desc *app_desc;

	app_desc = xwnd_app_desc_lookup(app_name);
	if(NULL == app_desc) {
		return -1;
	}

	xwnd_app_create(app_desc->run, arg);
	return 0;
}

void xwnd_app_remove(void) {
}

