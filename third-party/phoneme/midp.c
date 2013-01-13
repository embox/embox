/**
 * @file
 * @brief phoneME MIDP glue layer
 *
 * @date 11.01.2013
 * @author Felix Sulima
 */

#include <kernel/task.h>

#include "midp.h"
#include "cldc.h"

#include <stdio.h>

static void *phoneme_run(void *data) {
	//ToDo:
	printf("NOT IMPLEMENTED\n");
	return NULL;
}

struct __jvm_params {
	int argc;
	char **argv;
	int code;
};

int phoneme_midp(int argc, char **argv) {
	struct __jvm_params params = {
			.argc = argc,
			.argv = argv,
			.code = -1
	};

	new_task(phoneme_run, &params);
	while(!list_empty(&task_self()->children)) { } /* XXX make it throw signals */

	return params.code;
}

int phoneme_cldc(int argc, char **argv) {
	struct __jvm_params params = {
			.argc = argc,
			.argv = argv,
			.code = -1
	};

	new_task(phoneme_run, &params);
	while(!list_empty(&task_self()->children)) { } /* XXX make it throw signals */

	return params.code;
}
