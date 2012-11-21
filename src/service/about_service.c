/*
 * about_service.c
 *
 *  Created on: Nov 8, 2012
 *      Author: vita
 */

#include <embox/web_service.h>
#include <stdio.h>
#include <net/util/request_parser.h>

static void *index_thread_handler(void* args) {
	printf("\n\n[world]\n\n");
	return NULL;
}

EMBOX_WEB_SERVICE("about.html", index_thread_handler);

