/*
 * sai_thread.c
 *
 *  Created on: Jun 9, 2020
 *      Author: anton
 */

#include <util/log.h>

#include <stddef.h>

#include <kernel/thread.h>
#include <kernel/thread/thread_sched_wait.h>
#include <kernel/sched.h>

#include "adar_sai.h"

static void *sai_thread_hnd(void *arg) {
	struct sai_device *sai_device = (struct sai_device*) arg;
	if (!sai_device->sai_callback) {
		log_error("No callback provided for PA thread. "
				"That's probably not what you want.");
		return NULL;
	}

	SCHED_WAIT(sai_device->sai_active);


	while (1) {
		SCHED_WAIT(sai_device->sai_active);

	}

	return NULL;
}

int sai_thread_init(struct sai_device *sai_device) {

	sai_device->sai_thread = thread_create(THREAD_FLAG_SUSPENDED,
								sai_thread_hnd, sai_device);
	schedee_priority_set(&sai_device->sai_thread->schedee, SCHED_PRIORITY_MAX);
	return 0;
}
