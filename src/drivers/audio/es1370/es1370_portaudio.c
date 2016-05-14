/**
 * @file
 *
 * @date 14.05.2016
 * @author Anton Bondarev
 */
#include <stddef.h>
#include <assert.h>
#include <unistd.h>

#include <drivers/audio/portaudio.h>

#define PA_STUB_VALID_STREAM ((void *) 0xff0)

PaError Pa_Initialize(void) {
	return paNoError;
}

PaError Pa_Terminate(void) {
	return paNoError;
}

PaHostApiIndex Pa_GetHostApiCount(void) { return 1; }
PaDeviceIndex Pa_GetDeviceCount(void) { return 1; }
PaDeviceIndex Pa_GetDefaultOutputDevice(void) { return 0; }

const char * Pa_GetErrorText(PaError errorCode) {
	return "Pa_GetErrorText not implemented";
}

const PaDeviceInfo * Pa_GetDeviceInfo(PaDeviceIndex device) {
	static const PaDeviceInfo info = {
		.structVersion = 1,
		.name = "es1370 portaudio",
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

	return pa_info;
}

const PaHostApiInfo * Pa_GetHostApiInfo(PaHostApiIndex hostApi) {
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

	*stream = PA_STUB_VALID_STREAM;
	return paNoError;
}

PaError Pa_CloseStream(PaStream *stream) {
	return paNoError;
}

PaError Pa_StartStream(PaStream *stream) {
	return paNoError;
}

PaError Pa_StopStream(PaStream *stream) {
	return paNoError;
}

void Pa_Sleep(long msec) {
	usleep(msec * 1000);
}
