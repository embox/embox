/**
 * @file
 * @brief
 *
 * @date 27.08.14
 * @author Ilia Vaprol
 */

#include <errno.h>
#include <stdio.h>
#include <stdint.h>

#include <drivers/audio/portaudio.h>
#include <kernel/printk.h>
#include <linux/byteorder.h>
#include <util/math.h>

extern const uint16_t AUDIO_SAMPLE[];  /* user-provided */

struct out_data {
	void *data;
	size_t left;
};

static int callback(const void *input, void *output, unsigned long frameCount,
		const PaStreamCallbackTimeInfo* paTimeInfo,
		PaStreamCallbackFlags statusFlags, void *userData) {
	struct out_data *data = (struct out_data *)userData;
	short *dest = (short *)output;
	int cnt = min(frameCount, data->left / 2);

	//printf("cb: in %p out %p cnt %lu time %p flag %lu data %p\n",
	//		input, output, frameCount, paTimeInfo, statusFlags, userData);

	while (cnt--) {
		*dest++ = 0;
		*dest++ = *(short *)data->data;
		data->data += sizeof(short);
		data->left -= sizeof(short);
	}

	return data->left ? paContinue : paComplete;
}

int main(int argc, char **argv) {
	static struct out_data data;
	uint32_t sample_rate, subchunk2size;
	PaStream *strm;
	PaStreamParameters out_params;
	PaError err;

	if (argc != 1) {
		printf("%s: error: arguments are not supported\n", argv[0]);
		return -EINVAL;
	}

	sample_rate = le32_to_cpu(*(uint32_t *)((uint8_t *)AUDIO_SAMPLE + 24));
	subchunk2size = le32_to_cpu(*(uint32_t *)((uint8_t *)AUDIO_SAMPLE + 40));

	data.data = (uint8_t *)AUDIO_SAMPLE + 44;
	data.left = subchunk2size - 8;

	printf("rate: %u\n", sample_rate);
	printf("subchunk2size: %u\n", subchunk2size);
	printf("data: %#2hhx %#2hhx\n", AUDIO_SAMPLE[44], AUDIO_SAMPLE[45]);

	err = Pa_Initialize();
	if (err) {
		printf("error on initializing, error code = %i\n", err);
		return -1;
	}

	out_params.device = Pa_GetDefaultOutputDevice();
	out_params.channelCount = 1;
	out_params.sampleFormat = paInt32;
	out_params.suggestedLatency = 0.2;
	out_params.hostApiSpecificStreamInfo = 0;

	err = Pa_OpenStream(&strm, NULL, &out_params,
			sample_rate, paFramesPerBufferUnspecified,
			paNoFlag, callback, &data);
	if (err) {
		printf("error opening stream, error code = %i\n", err);
		Pa_Terminate();
		return -1;
	}

	err = Pa_StartStream(strm);
	if (err) {
		printf("error starting stream, error code = %i\n", err);
		Pa_Terminate();
		return -1;
	}

	Pa_Sleep(12000);

	err = Pa_StopStream(strm);
	if (err) {
		printf("error stopping stream, error code = %i\n", err);
		Pa_Terminate();
		return -1;
	}

	err = Pa_Terminate();
	if (err) {
		printf("error on termination, error code = %i\n", err);
		return -1;
	}

	return 0;
}
