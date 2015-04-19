/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    18.04.2015
 */

#include <assert.h>
#include <stdlib.h>
#include <unistd.h>

#include <drivers/audio/portaudio.h>

#include <kernel/printk.h>
#define D(fmt, ...) \
	do { \
		printk("%s: " fmt "\n", __func__, ##__VA_ARGS__); \
	} while (0)

PaError Pa_Initialize(void) {
	D("");
	return paNoError;
}

PaError Pa_Terminate(void) {
	D("");
	return paNoError;
}

PaHostApiIndex Pa_GetHostApiCount(void) { return 1; }
PaDeviceIndex Pa_GetDeviceCount(void) { return 1; }
PaDeviceIndex Pa_GetDefaultOutputDevice(void) { return 0; }

const char * Pa_GetErrorText(PaError errorCode) {
	D("%d", errorCode);
	return "Pa_GetErrorText not implemented";
}

const PaDeviceInfo * Pa_GetDeviceInfo(PaDeviceIndex device) {
	static const PaDeviceInfo info = {
		.structVersion = 1,
		.name = "stm32f4_audio",
		.hostApi = 0,
		.maxInputChannels = 1,
		.maxOutputChannels = 1,
		.defaultLowInputLatency = 0,
		.defaultLowOutputLatency = 0,
		.defaultHighInputLatency = 0,
		.defaultHighOutputLatency = 0,
		.defaultSampleRate = 8000
	};
	const PaDeviceInfo *pa_info = device == 0 ? &info : NULL;

	D("%d = %p", device, pa_info);
	return pa_info;
}

const PaHostApiInfo * Pa_GetHostApiInfo(PaHostApiIndex hostApi) {
	D("%d = NULL", hostApi);
	return NULL;
}

const PaStreamInfo * Pa_GetStreamInfo(PaStream *stream) {
	static PaStreamInfo info = {
		.structVersion = 1,
		.inputLatency = 0,
		.outputLatency = 0,
		.sampleRate = 8000
	};
	PaStreamInfo *pa_info = stream != NULL ? &info : NULL;

	D("%p = %p", stream, pa_info);
	return pa_info;
}

PaError Pa_OpenStream(PaStream** stream,
		const PaStreamParameters *inputParameters,
		const PaStreamParameters *outputParameters,
		double sampleRate, unsigned long framesPerBuffer,
		PaStreamFlags streamFlags, PaStreamCallback *streamCallback,
		void *userData) {
	assert(stream != NULL);
	assert(inputParameters == NULL);
	assert(outputParameters != NULL
			&& outputParameters->device == 0
			&& outputParameters->channelCount == 1
			&& (outputParameters->sampleFormat == paInt16)
			&& outputParameters->hostApiSpecificStreamInfo == 0);
	assert(streamFlags == paNoFlag || streamFlags == paClipOff);
	assert(streamCallback != NULL);

	D("%p %p %p %f %lu %lu %p %p", stream, inputParameters, outputParameters,
			sampleRate, framesPerBuffer, streamFlags, streamCallback, userData);

	*stream = NULL;
	return paNoError;
}

PaError Pa_CloseStream(PaStream *stream) {
	D("%p", stream);
	return paNoError;
}

PaError Pa_StartStream(PaStream *stream) {
	D("%p", stream);
	return paNoError;
}

PaError Pa_StopStream(PaStream *stream) {
	D("%p", stream);
	return paNoError;
}

void Pa_Sleep(long msec) {
	D("%ld", msec);
	usleep(msec * 1000);
}
