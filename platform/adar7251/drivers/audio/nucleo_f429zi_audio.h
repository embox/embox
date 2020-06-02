/*
 * nucleo_f429zi_audio.h
 *
 *  Created on: Jun 1, 2020
 *      Author: anton
 */

#ifndef PLATFORM_ADAR7251_DRIVERS_AUDIO_NUCLEO_F429ZI_AUDIO_H_
#define PLATFORM_ADAR7251_DRIVERS_AUDIO_NUCLEO_F429ZI_AUDIO_H_

#include <stdint.h>

struct sai_device;

extern struct sai_device *sai_init(void);

extern int sai_recieve(struct sai_device *sai_dev, uint8_t *buf, int len);

#endif /* PLATFORM_ADAR7251_DRIVERS_AUDIO_NUCLEO_F429ZI_AUDIO_H_ */
