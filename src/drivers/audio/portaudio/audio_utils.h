/**
 * @file
 * @brief
 *
 * @author  Alex Kalmuk
 * @date    19.11.2018
 */

#ifndef AUDIO_UTILS_H_
#define AUDIO_UTILS_H_

/*
 * @param cur_chan  Current number of channels in @a buf
 * @param out_chan  Requested number of channels in resulting buffer
 * @param buf       Buffer to be converted
 * @param buf_sz    Buffer size in frames
 **/
void audio_convert_channels(int cur_chan, int out_chan, uint8_t *buf,
		size_t buf_sz);

/*
 * Rate convertion for STEREO audio
 *
 * @param cur_chan  Current rate
 * @param out_chan  Requested rate
 * @param buf       Buffer to be converted
 * @param buf_sz    Buffer size in frames
 **/
void audio_convert_rate(int cur_rate, int out_rate, uint8_t *buf,
		size_t buf_sz);

#endif /* AUDIO_UTILS_H_ */
