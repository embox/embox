/*
 * adar_sai.h
 *
 *  Created on: Jun 9, 2020
 *      Author: anton
 */

#ifndef PLATFORM_ADAR7251_DRIVERS_AUDIO_ADAR_SAI_H_
#define PLATFORM_ADAR7251_DRIVERS_AUDIO_ADAR_SAI_H_

#include <stdint.h>

struct thread;

struct sai_device {
	uint32_t sai_buf[0x800]; /* 2 * 1024  udp frames */
	struct thread *sai_thread;
	int (*sai_callback)(const void *input, unsigned long frameCount, void *userData );
	void *sai_user_data;

	volatile int sai_active;
};

extern int sai_thread_init(struct sai_device *sai_device);

#endif /* PLATFORM_ADAR7251_DRIVERS_AUDIO_ADAR_SAI_H_ */
