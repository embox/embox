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

#include <portaudio.h>

#include <kernel/printk.h>
#define D(fmt, ...) \
	do { \
		printk("%s: " fmt "\n", __func__, ##__VA_ARGS__); \
	} while (0)

#define PA_STUB_VALID_STREAM ((void *) 0xff0)

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
PaDeviceIndex Pa_GetDefaultInputDevice(void) { return 0; }

const char * Pa_GetErrorText(PaError errorCode) {
	D("errcode=%d", errorCode);
	return "Pa_GetErrorText not implemented";
}

const PaDeviceInfo * Pa_GetDeviceInfo(PaDeviceIndex device) {
	static const PaDeviceInfo info = {
		.structVersion = 1,
		.name = "portaudio_stub",
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

	D("index=%d info=%p", device, pa_info);
	return pa_info;
}

const PaHostApiInfo * Pa_GetHostApiInfo(PaHostApiIndex hostApi) {
	D("api_index=%d info=%p", hostApi, NULL);
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

	D("stream=%p info=%p", stream, pa_info);
	return pa_info;
}

PaError Pa_OpenStream(PaStream** stream,
		const PaStreamParameters *inputParameters,
		const PaStreamParameters *outputParameters,
		double sampleRate, unsigned long framesPerBuffer,
		PaStreamFlags streamFlags, PaStreamCallback *streamCallback,
		void *userData) {
	assert(stream != NULL);
	assert(streamFlags == paNoFlag || streamFlags == paClipOff);
	assert(streamCallback != NULL);

	D("stream=%p in_param=%p out_param=%p samplerate=%f"
			" frames/buffer=%lu flags=%lu callback=%p data=%p",
			stream, inputParameters, outputParameters, sampleRate,
			framesPerBuffer, streamFlags, streamCallback, userData);

	*stream = PA_STUB_VALID_STREAM;
	return paNoError;
}

PaError Pa_CloseStream(PaStream *stream) {
	D("stream=%p", stream);
	return paNoError;
}

PaError Pa_StartStream(PaStream *stream) {
	D("stream=%p", stream);
	return paNoError;
}

PaError Pa_StopStream(PaStream *stream) {
	D("stream=%p", stream);
	return paNoError;
}

void Pa_Sleep(long msec) {
	D("msec=%ld", msec);
	usleep(msec * 1000);
}
