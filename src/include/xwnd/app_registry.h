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
#include <kernel/thread/sync/sem.h>

struct xwnd_app_init_wrapper {
	int xapp_id;
	int req_pipe;
	int msg_pipe;
	sem_t * msg_sem;
	sem_t * req_sem;
};

struct xwnd_app_registry_node {
	int pipe_in;
	int pipe_out;
	int tid;
	sem_t msg_sem;
	sem_t req_sem;
	struct xwnd_app_init_wrapper init_wrap;
	//struct xwnd_application * xapp;
};

struct xwnd_app_registry {
	int allocated;
	int used;
	struct xwnd_app_registry_node * nodes;
};

extern int xwnd_app_reg_init (void);
extern int xwnd_app_create (void* (*entry_point) (void*));
extern void xwnd_app_remove(void);
extern int xwnd_app_put_message(int app_id, void * data, size_t size);
extern int xwnd_app_set_msg_sem(void);

#endif /* XWND_APP_REGISTRY_H_ */
