/**
 * @file
 * @brief
 *
 * @author  Alex Kalmuk
 * @date    19.11.2018
 */

#include <assert.h>
#include <stdint.h>
#include <util/math.h>
#include <drivers/audio/audio_utils.h>

/**
 * @brief Duplicate left channel for the buffer
 */
static void audio_mono_to_stereo(void *buf, int len) {
	/* Assume data is 16-bit */
	uint16_t *b16 = buf;
	for (int i = len * 2 - 1; i >= 0; i--) {
		b16[i] = b16[i / 2];
	}
}

/**
 * @brief Remove right channel from audio buffer
 */
static void audio_stereo_to_mono(void *buf, int len) {
	/* Assume data is 16-bit */
	uint16_t *b16 = buf;
	for (int i = 0; i < len; i++) {
		b16[i] = b16[i * 2];
	}
}

void audio_convert_channels(int cur_chan, int out_chan, uint8_t *buf,
		size_t buf_sz) {
	if (cur_chan == out_chan) {
		return;
	}

	assert((cur_chan == 1 && out_chan == 2) ||
		(cur_chan == 2 && out_chan == 1));

	if (cur_chan == 1 && out_chan == 2) {
		audio_mono_to_stereo(buf, buf_sz);
	} else if (cur_chan == 2 && out_chan == 1) {
		audio_stereo_to_mono(buf, buf_sz);
	}
}

void audio_convert_rate(int cur_rate, int out_rate, uint8_t *buf,
		size_t buf_sz) {
	/* XXX 16-bit stereo buffer */
	uint32_t *buf32 = (uint32_t *) buf;
	int div, i;

	if (cur_rate == out_rate) {
		return;
	}

	div = max(cur_rate, out_rate) / min(cur_rate, out_rate);
	assert(div > 1);

	if (cur_rate > out_rate) {
		for (i = buf_sz - 1; i >= 0; i--) {
			buf32[i] = buf32[i / div];
		}
	} else {
		for (i = 0; i < buf_sz; i++) {
			buf32[i] = buf32[div * i];
		}
	}
}
