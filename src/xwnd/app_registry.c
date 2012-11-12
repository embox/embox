/**
 * @file
 * @brief XWnd application registry implementation
 *
 * @date Oct 18, 2012
 * @author Alexandr Chernakov
 */


#include <xwnd/app_registry.h>
#include <kernel/task.h>
#include <stdlib.h>
#include <unistd.h>
#include <xwnd/event.h>
#include <xwnd/window.h>

#define XAPP_REG_DEF_CNT 256


static struct xwnd_app_registry xapp_reg;

int xwnd_app_reg_init (void) {
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

int xwnd_app_create (void* (*entry_point) (void*)) {
	int xapp_id = xapp_reg.used;
	int xapp_tid;

	/*FIXME: Get an entry in registry*/
	if (xapp_id >= XAPP_REG_DEF_CNT - 1) {
		return -1;
	}
	xapp_reg.used++;

	/*Initialize event queue connection*/
	xwnd_event_create_pair(&(xapp_reg.nodes[xapp_id].ev), &(xapp_reg.nodes[xapp_id].init_wrap.ev));

	/*Send first messages*/
	xwnd_app_send_sys_event(xapp_id, XWND_EV_CREAT);
	xwnd_app_send_sys_event(xapp_id, XWND_EV_DRAW);

	/*Now ready to start an application*/
	xapp_tid = new_task(entry_point, (void*)(&(xapp_reg.nodes[xapp_id].init_wrap)), 0);
	xapp_tid = xapp_tid;
	return xapp_id;
}

void xwnd_app_remove(void) {
}

int xwnd_app_put_message(int app_id, void * data, size_t size) {
	int msg_pipe = xapp_reg.nodes[app_id].ev.msg_pipe;
	int err;
	if (!sem_tryenter(&(xapp_reg.nodes[app_id].ev.msg_sem))) {
		err = write(msg_pipe, data, size);
		sem_leave(&(xapp_reg.nodes[app_id].ev.msg_sem));
	}
	if (err != size) {
		return -1;
	}
	return 0;
}
