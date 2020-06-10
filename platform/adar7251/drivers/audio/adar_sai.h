/*
 * adar_sai.h
 *
 *  Created on: Jun 9, 2020
 *      Author: anton
 */

#ifndef PLATFORM_ADAR7251_DRIVERS_AUDIO_ADAR_SAI_H_
#define PLATFORM_ADAR7251_DRIVERS_AUDIO_ADAR_SAI_H_

#include <stdint.h>

#include <drivers/nucleo_f429zi_audio.h>

struct thread;

struct sai_device {
	uint32_t sai_buf[SAI_SAMPLES_BUFFER]; /* 2 * 1024  udp frames */
	struct thread *sai_thread;
	int (*sai_callback)(const void *input, unsigned long frameCount, void *userData );
	void *sai_user_data;

	volatile int sai_active;
	volatile uint8_t *sai_cur_buf;
};

extern struct sai_device sai_device;

extern int sai_thread_init(struct sai_device *sai_device);

#endif /* PLATFORM_ADAR7251_DRIVERS_AUDIO_ADAR_SAI_H_ */
