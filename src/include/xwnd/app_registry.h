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
#include <util/array.h>

struct xwnd_app_init_wrapper {
	int xapp_id;
	struct xwnd_event_slave ev;
};

struct xwnd_app_registry_node {
	int tid;
	//struct xwnd_event_master ev;
	int ev_master;
	int is_empty;
	struct xwnd_app_init_wrapper init_wrap;
};

struct xwnd_app_registry {
	int allocated;
	int used;
	struct xwnd_app_registry_node * nodes;
	struct xwnd_event_supervisor * event_sup;
};

//extern struct xwnd_app_registry * xwnd_app_reg_init (void);
//extern int xwnd_app_create (void* (*entry_point) (void*));
//extern void xwnd_app_remove(void);

struct xwnd_app_registry *xwnd_app_reg(void);
extern int xwnd_app_put_message_by_app_id(int app_id, void * data, size_t size);
extern int xwnd_app_put_message_by_event_id(int ev_id, void * data, size_t size);
extern int xwnd_app_set_msg_sem(void);




struct xwnd_app_desc {
	const char *app_name;
	void *(*run)(void *);
};

struct xwnd_app_instance {
	struct task *task;
	//struct event *e;
	//void * params;
	const struct xwnd_app_desc *desc;
	struct dlist_head lst;
};


extern const struct xwnd_app_desc __xwnd_app_repository[];

#define EMBOX_XWND_APP(name, app_handler) \
	ARRAY_SPREAD_ADD(__xwnd_app_repository, {name,app_handler})


#endif /* XWND_APP_REGISTRY_H_ */
