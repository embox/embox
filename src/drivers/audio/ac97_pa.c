/**
 * @file
 * @brief Portaudio driver for AC97 controller
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-11-27
 */

#include <unistd.h>

#include <asm/io.h>
#include <drivers/pci/pci.h>
#include <drivers/pci/pci_chip/pci_utils.h>
#include <drivers/pci/pci_driver.h>
#include <drivers/audio/portaudio.h>
#include <mem/misc/pool.h>

/* Some of this stuff probably shoud be placed into
 * separate module */
#define MAX_BUF_LEN 1024
struct pa_strm {
	int started;
	int paused;
	int completed;
	int sample_format;
	PaStreamCallback *callback;
	void *callback_data;
	size_t chan_buf_len;
	uint16_t in_buf[MAX_BUF_LEN];
	uint16_t out_buf[MAX_BUF_LEN];
};

#define STREAM_POOL_SZ 16

POOL_DEF(pa_stream_pool, struct pa_strm, STREAM_POOL_SZ);

/* Intel Corporation 82801AA AC'97 Audio Controller,
 * provided by QEMU with flag -soundhw ac97 */
#define AC97_VID 0x8086
#define AC97_PID 0x2415

#define AC97_RESET		0x00
#define AC97_MASTER		0x02
#define AC97_HEADPHONE		0x04
#define AC97_MASTER_MONO	0x06
#define AC97_MASTER_TONE	0x08
#define AC97_PC_BEEP		0x0A
#define AC97_PHONE		0x0C
#define AC97_MIC		0x0E
#define AC97_LINE		0x10
#define AC97_CD			0x12
#define AC97_VIDEO		0x14
#define AC97_AUX		0x16
#define AC97_PCM		0x18
#define AC97_REC_SEL		0x1A
#define AC97_REC_GAIN		0x1C
#define AC97_REC_GAIN_MIC	0x1E
#define AC97_GENERAL_PURPOSE	0x20
#define AC97_3D_CONTROL		0x22
#define AC97_INT_PAGING		0x24
#define AC97_POWERDOWN		0x26

static int ac97_bus;
static int ac97_devfn;
static int ac97_bar;

static int ac97_init(struct pci_slot_dev *pci_dev) {
	ac97_bus   = pci_dev->busn;
	ac97_devfn = PCI_DEVFN(pci_dev->slot, pci_dev->func);
	ac97_bar   = pci_dev->bar[0] & 0xFFFFFFFC;
	out16(0xFFFF, ac97_bar + AC97_RESET);
	out16(0x3F3F, ac97_bar + AC97_MASTER);

	return 0;
}

PCI_DRIVER("ac97", ac97_init, AC97_VID, AC97_PID);

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

/**
 * @brief Allocate and configure PaStream w/ given parameters
 * @note Now works only for audio output
 *
 * @param stream
 * @param inputParameters
 * @param outputParameters
 * @param sampleRate
 * @param framesPerBuffer
 * @param streamFlags
 * @param streamCallback
 * @param userData
 *
 * @return Error number
 */
PaError Pa_OpenStream(PaStream** stream,
                       const PaStreamParameters *inputParameters,
                       const PaStreamParameters *outputParameters,
                       double sampleRate,
                       unsigned long framesPerBuffer,
                       PaStreamFlags streamFlags,
                       PaStreamCallback *streamCallback,
                       void *userData) {
	struct pa_strm *strm = pool_alloc(&pa_stream_pool);

	if (!strm)
		return paInternalError;

	assert(outputParameters);

	strm->started = 0;
	strm->paused = 0;
	strm->completed = 0;
	strm->sample_format = outputParameters->sampleFormat;
	strm->chan_buf_len = (MAX_BUF_LEN / framesPerBuffer) * framesPerBuffer;
	strm->callback = streamCallback;
	strm->callback_data = userData;

	*stream = strm;

	return paNoError;
}

PaError Pa_StartStream(PaStream *stream) {
	return paNoError;
}

PaError Pa_StopStream(PaStream *stream) {
	return paNoError;
}

PaError Pa_CloseStream(PaStream *stream) {
	pool_free(&pa_stream_pool, stream);

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
