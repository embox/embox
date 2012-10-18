/*
 * app_registry.h
 *
 *  Created on: 18.10.2012
 *      Author: alexandr
 */

#ifndef APP_REGISTRY_H_
#define APP_REGISTRY_H_

#include <xwnd/xwndapp.h>

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

int xwnd_app_reg_init (void);
struct xwnd_application * xwnd_app_create (void);
void xwnd_app_remove(void);

#endif /* APP_REGISTRY_H_ */
