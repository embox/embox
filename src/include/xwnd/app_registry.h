/*
 * app_registry.h
 *
 *  Created on: 18.10.2012
 *      Author: alexandr
 */

#ifndef XWND_APP_REGISTRY_H_
#define XWND_APP_REGISTRY_H_

#include <xwnd/xwndapp.h>
#include <kernel/thread/sync/sem.h>

struct xwnd_app_registry_node {
	int pipe_in;
	int pipe_out;
	int tid;
	struct xwnd_application * xapp;
};

struct xwnd_app_registry {
	int allocated;
	int used;
	struct xwnd_app_registry_node * nodes;
};

extern int xwnd_app_reg_init (void);
extern struct xwnd_application * xwnd_app_create (void* (*entry_point) (void*));
extern void xwnd_app_remove(void);
extern int xwnd_app_put_message(int app_id, void * data, size_t size);
extern int xwnd_app_set_msg_sem(void);

#endif /* XWND_APP_REGISTRY_H_ */
