#include <lib/x_event.h>

#include <stdlib.h>
#include <unistd.h>
#include <mem/misc/pool.h>

POOL_DEF (x_event_masters_pool, struct x_event_master, X_EVENT_MASTERS_POOL_SIZE);

struct x_event_master * x_event_subscribe (struct x_event_slave * slave, enum x_event_engine eng) {
	struct x_event_master * master;
	int msg_pipe[2];
	master = pool_alloc(&x_event_masters_pool);
	if (!master) {
		return NULL;
	}
	switch (eng) {
		case XEVENG_PIPE:
			if (pipe(msg_pipe)) {
				pool_free(&x_event_masters_pool, master);
				return NULL;
			}
			master->engine = XEVENG_PIPE;
			slave->engine = XEVENG_PIPE;
			master->msg_pipe = msg_pipe[1];
			slave->msg_pipe = msg_pipe[0];
			break;
		case XEVENG_MQUEUE:
			master->engine = XEVENG_MQUEUE;
			slave->engine = XEVENG_MQUEUE;
			mqueue_init(&(master->mque));
			slave->mque = &(master->mque);
			break;
		default:
			pool_free(&x_event_masters_pool, master);
			break;
	}
	return master;
}

void x_event_unsubscribe (struct x_event_master * master) {
	pool_free(&x_event_masters_pool, master);
}

static int x_event_pipe_send (struct x_event_master * master, const struct x_event * ev) {
	int err;
	if ((err = write(master->msg_pipe, ev, sizeof(struct x_event))) != sizeof(struct x_event)) {
		return 0;
	}
	return 1;
}

static int x_event_pipe_recv (struct x_event_slave * slave, struct x_event * ev) {
	int err;
	if ((err = read(slave->msg_pipe, ev, sizeof(struct x_event))) != sizeof(struct x_event)) {
		return 0;
	}
	return 1;
}

static int x_event_mqueue_send (struct x_event_master * master, const struct x_event * ev) {
	return (mqueue_send(&master->mque, (const char *)ev, sizeof(struct x_event))) ? 0 : 1;
}

static int x_event_mqueue_recv (struct x_event_slave* slave, struct x_event * ev) {
	int err;
	err = mqueue_receive(slave->mque, (char *)ev);
	if (err != sizeof(struct x_event)) {
		return 0;
	}
	return 1;
}

int x_event_send (struct x_event_master * master, const struct x_event * ev) {
	switch (master->engine) {
		case XEVENG_PIPE:
			return x_event_pipe_send(master, ev);
			break;
		case XEVENG_MQUEUE:
			return x_event_mqueue_send(master, ev);
			break;
		default:
			return 0;
			break;
	}
}
int x_event_recv (struct x_event_slave * slave, struct x_event * ev) {
	switch (slave->engine) {
		case XEVENG_PIPE:
			return x_event_pipe_recv(slave, ev);
			break;
		case XEVENG_MQUEUE:
			return x_event_mqueue_recv(slave, ev);
			break;
		default:
			return 0;
			break;
	}
}

