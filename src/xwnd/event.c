/*
 * event.c
 *
 *  Created on: 18.10.2012
 *      Author: alexandr
 */

#include <stdlib.h>

#define EV_PIPES_DEF_SIZE 512

static struct xwnd_event_pipes_array event_pipes = {0,0,NULL};

int xwnd_event_pipes_init(void) {
	event_pipes.array = malloc(EV_PIPES_DEF_SIZE * sizeof(struct xwnd_event_pipe));
	if (!event_pipes.array) {
		return -1;
	}
	event_pipes.allocated = EV_PIPES_DEF_SIZE;
	event_pipes.count = 0;
}

const struct xwnd_event_pipe * xwnd_event_pipe_add(void) {
}

void xwnd_event_pipe_delete(void){
}
