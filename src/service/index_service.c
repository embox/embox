/*
 * hello_service.c
 *
 *  Created on: Nov 8, 2012
 *      Author: vita
 */


#include <embox/web_service.h>
#include <stdio.h>


struct web_service_params {
	char par[80];
};


static void *index_thread_handler(void* args) {
	/*struct web_service_instance *inst;

	inst = (struct web_service_instance *) args;*/

	printf("\n\n[Hello]\n\n");

	return NULL;
}

EMBOX_WEB_SERVICE("index.html", index_thread_handler);

