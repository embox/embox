/**
 * @file
 * @brief
 *
 * @date 25.09.14
 * @author Ilia Vaprol
 */

#ifndef DRIVERS_AUDIO_H_
#define DRIVERS_AUDIO_H_

#include <stddef.h>

enum audio_play_mode {
	APM_SEQ, /* Sequently */
	APM_NOW  /* Now */
};

extern int audio_play(enum audio_play_mode mode, const void *data,
		size_t size, int need_copy);

#endif /* DRIVERS_AUDIO_H_ */
