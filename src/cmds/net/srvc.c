/*
 * servrun.c
 *
 *  Created on: Nov 21, 2012
 *      Author: vita
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <embox/web_service.h>

#include <embox/cmd.h>

EMBOX_CMD(exec);

int srvcrun(char **argv) {
	char * service = argv[2];

	if (0 <= web_service_start(service)) {
		printf ("service %s started\n", service);
		return 0;
	}

	printf ("service %s doesn't exists or starting error\n", service);
	return -1;
}

int srvcstop(char **argv) {
	char * service = argv[2];

	if (0 <= web_service_send_message(service, NULL)) {
		printf ("service %s stopped\n", service);
		return 0;
	}

	printf ("service %s isn't started\n", service);
	return -1;
}

static int exec(int argc, char **argv) {
	if (0 == strcmp(argv[1], "run")) {
		return srvcrun(argv);
	}

	if (0 == strcmp(argv[1], "stop")) {
		return srvcstop(argv);
	}

	printf ("couldn't resolve %s\n", argv[1]);
	return -1;
}
