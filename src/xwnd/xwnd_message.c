/**
 * @file
 *
 * @date Nov 12, 2012
 * @author: Anton Bondarev
 */
#include <types.h>
#include <xwnd/app_registry.h>
#include <unistd.h>


#define xapp_reg  xwnd_app_reg()

int xwnd_app_put_message_by_app_id (int app_id, void * data, size_t size) {
	int msg_pipe;
	int err;

	msg_pipe = xapp_reg->event_sup->masters[xapp_reg->nodes[app_id].ev_master].msg_pipe; /*Yes, I do love anal sex*/
	if (!sem_tryenter(&(xapp_reg->event_sup->masters[xapp_reg->nodes[app_id].ev_master].msg_sem))) {
		err = write(msg_pipe, data, size);
		sem_leave(&(xapp_reg->event_sup->masters[xapp_reg->nodes[app_id].ev_master].msg_sem));
	}
	if (err != size) {
		return -1;
	}
	return 0;
}

int xwnd_app_put_message_by_event_id (int ev_id, void * data, size_t size) {
	int msg_pipe = xapp_reg->event_sup->masters[ev_id].msg_pipe;
	int err;
	if (!sem_tryenter(&(xapp_reg->event_sup->masters[ev_id].msg_sem))) {
		err = write(msg_pipe, data, size);
		sem_leave(&(xapp_reg->event_sup->masters[ev_id].msg_sem));
	}
	if (err != size) {
		return -1;
	}
	return 0;
}
