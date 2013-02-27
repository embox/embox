/**
 * @file
 * @brief phoneME MIDP glue layer
 *
 * @date 11.01.2013
 * @author Felix Sulima
 */

#include <javacall_events.h>
#include <kernel/task.h>

#include "midp.h"
#include "cldc.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <drivers/input/input_dev.h>

#include <javacall_lifecycle.h>


static void *phoneme_run(void *data) {
	//ToDo:
	printf("NOT IMPLEMENTED\n");
	JavaTask();
	return NULL;
}

struct __jvm_params {
	int argc;
	char **argv;
	int code;
};

/* Notify java task about system events. */
static void notify_loop(void) {
	int fd;
	struct input_event buf[16];

	if (0 > (fd = open("/dev/input/keyboard", 0))) {
		return;
	}

	while(!list_empty(&task_self()->children)) { /* XXX make it throw signals */
		size_t len, i, event_size = sizeof(struct input_event);
		if ((len = read(fd, buf, event_size * 16)) > 0) {
			/* Send all events we read to java task. */
			for (i = 0; i < len / event_size; i++) {
				/*javacall_event_send(buf[i], event_size);*/
			}
		}
	}
}

int phoneme_midp(int argc, char **argv) {
	struct __jvm_params params = {
			.argc = argc,
			.argv = argv,
			.code = -1
	};

	if (JAVACALL_OK != javacall_events_init()) {
		return -1;
	}

	new_task("", phoneme_run, &params);
	notify_loop();

	return params.code;
}

int phoneme_cldc(int argc, char **argv) {
	struct __jvm_params params = {
			.argc = argc,
			.argv = argv,
			.code = -1
	};

	new_task("", phoneme_run, &params);
	while(!list_empty(&task_self()->children)) { } /* XXX make it throw waitpid() */

	return params.code;
}
