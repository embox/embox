/**
 * @file
 * @brief Driver for Intel Corporation 82801AA AC'97 Audio Controller (rev 01),
 * which is based on PortAudio library
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-12-03
 */

#include <unistd.h>

#include <asm/io.h>
#include <drivers/pci/pci.h>
#include <drivers/pci/pci_chip/pci_utils.h>
#include <drivers/pci/pci_driver.h>
#include <drivers/audio/portaudio.h>
#include <drivers/audio/ac97.h>
#include <mem/misc/pool.h>

/* You may want to look at "Inte 82801AA (ICH) & Intel 82801AB(ICH0)
 * IO Controller Hub AC ’97 Programmer’s Reference Manual" for more
 * technical details */

/* Offsets of registers */
#define INTEL_AC_COM	0x04 /* Command */
#define INTEL_AC_NAMBA	0x10 /* Native Audio Mixer Base Address */
#define INTEL_AC_NABMBA	0x14 /* Native Audio Bus Mastering Base Address */
#define INTEL_AC_INTLN	0x3c /* Interrupt Line */

#define INTEL_AC_PCM_IN_BUF  0x00
#define INTEL_AC_PCM_OUT_BUF 0x10
#define INTEL_AC_MIC_BUF     0x20

/* Last Valid Index */
#define INTEL_AC_PCM_IN_LVI  0x05
#define INTEL_AC_PCM_OUT_LVI 0x15
#define INTEL_AC_MIC_LVI     0x25

/* Control registers */
#define INTEL_AC_PCM_IN_CR  0x0B
#define INTEL_AC_PCM_OUT_CR 0x1B
#define INTEL_AC_MIC_CR     0x2B

#define INTEL_AC_SAMPLE_SZ 2  /* Bytes */
#define INTEL_AC_BUFFER_SZ 32 /* Buffer descriptors */

struct intel_ac_buff_desc {
	uint32_t pointer;           /* 2-byte aligned */
	unsigned int ioc : 1;       /* Interrupt on completion */
	unsigned int bup : 1;       /* Buffer underrun policy */
	unsigned int reserved : 14; /* Must be zeroes */
	unsigned int length : 16;   /* Buffer length in samples */
} __attribute__((aligned(2)));

/* This buffers could be allocated in runtime */
static struct intel_ac_buff_desc pcm_out_buff_list[INTEL_AC_BUFFER_SZ];

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

static int intel_ac_buf_init(int n) {
	/* Stub */
	return 0;
}

/* Intel Corporation 82801AA AC'97 Audio Controller,
 * provided by QEMU with flag -soundhw ac97 */
#define INTEL_AC_VID 0x8086
#define INTEL_AC_PID 0x2415

static int intel_ac_init(struct pci_slot_dev *pci_dev) {
	int err;
	int i;
	uint8_t tmp;
	uint32_t audio_base = pci_dev->bar[1] & 0xFF00;
	assert(pci_dev);

	/* Codec init */
	if ((err = ac97_init(pci_dev)))
		return err;

	/* DMA init */
	out32((uint32_t)&pcm_out_buff_list, audio_base + INTEL_AC_PCM_OUT_BUF);
	/* Setup buffers, currently just zeroes */
	for (i = 0; i < INTEL_AC_BUFFER_SZ; i++) {
		intel_ac_buf_init(i);
	}

	/* Setup Last Valid Index */
	out8(INTEL_AC_BUFFER_SZ - 1, audio_base + INTEL_AC_PCM_OUT_LVI);

	/* Set run bit */
	tmp = in8(audio_base + INTEL_AC_PCM_OUT_CR);
	tmp |= 0x1;
	out8(tmp, audio_base + INTEL_AC_PCM_OUT_CR);

	return 0;
}

PCI_DRIVER("Intel Corporation 82801AA AC'97 Audio Controller", intel_ac_init, INTEL_AC_VID, INTEL_AC_PID);

/**
 * @brief Configure AC97 codec
 * @note This function does nothing, everything was done on intel_ac_init
 * @return PortAudio error code
 */
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
