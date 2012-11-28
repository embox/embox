/**
 * @file hello world service
 *
 * @date Nov 7, 2012
 * @author: Vita Loginova
 */

#include <embox/web_service.h>
#include <stdio.h>

static void *process_params(void* args) {
	printf("\n\n[Hello]\n\n");
	return NULL;
}

EMBOX_WEB_SERVICE("index.html", process_params);

