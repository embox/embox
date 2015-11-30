/**
 * @file
 * @brief Portaudio driver for AC97 controller
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-11-27
 */

#include <unistd.h>

#include <drivers/pci/pci.h>
#include <drivers/audio/portaudio.h>

/* Intel Corporation 82801AA AC'97 Audio Controller,
 * provided by QEMU with flag -soundhw ac97 */
#define AC97_VID 0x8086
#define AC97_PID 0x2415

PCI_DRIVER("ac97", ac97_init, AC97_VID, AC97_PID);

static int ac97_init(struct pci_slot_dev *pci_dev) {
	return 0;
}

PaError Pa_Initialize(void) {
	return paNoError;
}

PaError Pa_Terminate(void) {
	return paNoError;
}

PaHostApiIndex Pa_GetHostApiCount(void) {
	return 1;
}

PaHostApiIndex Pa_GetDefaultHostApi(void) {
	return 0;
}

PaDeviceIndex Pa_GetDeviceCount(void) {
	return 1;
}

PaDeviceIndex Pa_GetDefaultInputDevice(void) {
	return 0;
}

PaDeviceIndex Pa_GetDefaultOutputDevice(void) {
	return 0;
}

PaError Pa_OpenStream(PaStream** stream,
                       const PaStreamParameters *inputParameters,
                       const PaStreamParameters *outputParameters,
                       double sampleRate,
                       unsigned long framesPerBuffer,
                       PaStreamFlags streamFlags,
                       PaStreamCallback *streamCallback,
                       void *userData) {
	return paNoError;
}

PaError Pa_StartStream(PaStream *stream) {
	return paNoError;
}

PaError Pa_StopStream(PaStream *stream) {
	return paNoError;
}

PaError Pa_CloseStream(PaStream *stream) {
	return paNoError;
}

void Pa_Sleep(long msec) {
	usleep(msec * 1000);
}

const PaDeviceInfo *Pa_GetDeviceInfo(PaDeviceIndex device) {
	return 0;
}

const PaHostApiInfo *Pa_GetHostApiInfo(PaHostApiIndex hostApi) {
	return 0;
}

const PaStreamInfo *Pa_GetStreamInfo(PaStream *stream) {
	return 0;
}

const char *Pa_GetErrorText(PaError errorCode) {
	return "Pa_GetErrorText not implemented";
}
