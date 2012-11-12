/**
 * @file
 * @brief XWnd application registry API
 *
 * @date Oct 18, 2012
 * @author Alexandr Chernakov
 */

#ifndef XWND_APP_REGISTRY_H_
#define XWND_APP_REGISTRY_H_

#include <xwnd/xwndapp.h>
#include <xwnd/event.h>
#include <kernel/thread/sync/sem.h>

struct xwnd_app_init_wrapper {
	int xapp_id;
	struct xwnd_event_slave ev;
};

struct xwnd_app_registry_node {
	int tid;
	struct xwnd_event_master ev;
	int is_empty;
	struct xwnd_app_init_wrapper init_wrap;
};

struct xwnd_app_registry {
	int allocated;
	int used;
	struct xwnd_app_registry_node * nodes;
	struct xwnd_event_supervisor * event_sup;
};

extern int xwnd_app_reg_init (void);
extern int xwnd_app_create (void* (*entry_point) (void*));
extern void xwnd_app_remove(void);
extern int xwnd_app_put_message(int app_id, void * data, size_t size);
extern int xwnd_app_set_msg_sem(void);

#endif /* XWND_APP_REGISTRY_H_ */
