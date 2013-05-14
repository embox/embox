#ifndef LIB_X_EVENT_H_
#define LIB_X_EVENT_H_

#include <kernel/event.h>
#include <kernel/thread/sync/mqueue.h>
#include <mem/misc/pool.h>

#define X_EVENT_MASTERS_POOL_SIZE 0x100

enum x_event_engine {XEVENG_PIPE, XEVENG_MQUEUE, XEVENG_MAX};

enum x_event_type {XEV_CREAT, XEV_SIGNALLED, XEV_QUIT,
	XEV_KBD, XEV_DRAW, XEV_MAX /*to be continued*/};

struct x_kbd_event {
	char key; /*Oh, forgive me, mom*/
};
struct x_mouse_event {
	int x;
	int y;
	int state;
};
struct x_quit_event {
	int exit_status;
};
struct x_sys_event {
};
union x_event_info {
	struct x_sys_event sys;
	struct x_kbd_event kbd;
	struct x_mouse_event mouse;
	struct x_quit_event quit;
};
struct x_event {
	enum x_event_type type;
	struct event krnl_event;
	union x_event_info info;
};

struct x_event_slave {
	enum x_event_engine engine;
	int msg_pipe;
	struct mqueue * mque;
};

struct x_event_master {
	enum x_event_engine engine;
	int msg_pipe;
	struct mqueue mque;
};

/*struct x_event_supervisor {
	//struct x_event_master * masters;
	struct pool * masters;
	int focus;
	int allocated;
	int used;
};*/

extern struct x_event_master * x_event_subscribe (struct x_event_slave * slave, enum x_event_engine eng);
extern void x_event_unsubscribe (struct x_event_master * master);
extern int x_event_send (struct x_event_master * master, const struct x_event * ev);
extern int x_event_recv (struct x_event_slave * slave, struct x_event * ev);

#endif
