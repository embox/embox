/*
 * app_registry.c
 *
 *  Created on: 18.10.2012
 *      Author: alexandr
 */


#include <xwnd/app_registry.h>
#include <kernel/task.h>
#include <stdlib.h>
#include <unistd.h>
#include <xwnd/event.h>

//FIXME: this have to be removed
#include <xwnd/test_app.h>

#define XAPP_REG_DEF_CNT 256

static struct xwnd_app_registry xapp_reg;

int xwnd_app_reg_init (void) {
	xapp_reg.nodes = malloc (XAPP_REG_DEF_CNT * sizeof(struct xwnd_app_registry_node));
	if (!xapp_reg.nodes) {
		return -1;
	}
	xapp_reg.allocated = XAPP_REG_DEF_CNT;
	xapp_reg.used = 0;
	return 0;
}

struct xwnd_application * xwnd_app_create (void) {
	struct xwnd_application * t_xapp;
	struct xwnd_window * t_wnd;
	int req_pipe[2], msg_pipe[2];
	int xapp_id = xapp_reg.used;
	int xapp_tid;

	//Get an entry in registry
	if (xapp_id >= XAPP_REG_DEF_CNT - 1) {
		return NULL;
	}
	xapp_reg.used++;

	//Allocate client-side application structure
	//FIXME: This allocation is freaking bad. Replace it with something better
	t_xapp = malloc(sizeof(struct xwnd_application));
	if (!t_xapp) {
		xapp_reg.used--;
		return NULL;
	}
	xapp_reg.nodes[xapp_id].xapp = t_xapp;
	//Initialize them

	//Init pipe semaphores
	sem_init(&(t_xapp->msg_sem), 1);
	sem_init(&(t_xapp->req_sem), 1);
	//Connect pipes
	pipe(req_pipe);
	pipe(msg_pipe);
	xapp_reg.nodes[xapp_id].pipe_in  = req_pipe[0];
	xapp_reg.nodes[xapp_id].pipe_out = msg_pipe[1];
	t_xapp->pipe_in  = msg_pipe[0];
	t_xapp->pipe_out = req_pipe[1];

	//Create window
	t_wnd = malloc (sizeof(struct xwnd_window));
	if (!t_wnd) {
		free(t_xapp);
		return NULL;
	}
	t_wnd->caption = NULL;
	t_wnd->wdg.x = 10;
	t_wnd->wdg.y = 10;
	t_wnd->wdg.wd = 100;
	t_wnd->wdg.ht = 60;
	t_wnd->wdg.wdg_list = NULL;
	t_wnd->wdg.next = NULL;

	t_xapp->app_id = xapp_id;

	//Send first messages
	xwnd_app_send_sys_event(xapp_id, XWND_EV_CREAT);
	xwnd_app_send_sys_event(xapp_id, XWND_EV_DRAW);

	//Now ready to start an application
	xapp_tid = new_task(test_app_main, (void*)t_xapp, 0);
	xapp_tid = xapp_tid;
	return t_xapp;
}

void xwnd_app_remove(void) {
}

int xwnd_app_put_message(int app_id, void * data, size_t size) {
	int msg_pipe = xapp_reg.nodes[app_id].pipe_out;
	int err;
	if ((err = write (msg_pipe, data, size)) != size) {
		return -1;
	}
	return 0;
}
