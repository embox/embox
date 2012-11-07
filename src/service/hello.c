/*
 * @file
 *
 * @date Nov 7, 2012
 * @author: Anton Bondarev
 */

#include <embox/web_service.h>


struct web_service_params {
	char par[80];
};


static void *hello_thread_handler(void* args) {
	struct web_service_instance *inst;

	inst = 	(struct web_service_instance *)args;

	while (1) {
		event_wait(inst->e, EVENT_TIMEOUT_INFINITE);
		//here you can receive the message
	}

	return NULL;
}

EMBOX_WEB_SERVICE("hello", hello_thread_handler);

