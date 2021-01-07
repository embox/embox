/**
 * @file
 * @brief
 *
 * @date 25.09.14
 * @author Ilia Vaprol
 * @author Alexander Kalmuk
 *           Adaptation for STM32F4/F7 Cube
 */

#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <linux/byteorder.h>
#include <util/err.h>

#include <framework/mod/options.h>
#include <kernel/irq.h>
#include <kernel/thread.h>
#include <kernel/thread/thread_sched_wait.h>
#include <kernel/panic.h>
#include <kernel/printk.h>
#include <mem/misc/pool.h>
#include <util/dlist.h>
#include <util/bit.h>

#include <portaudio.h>

#include <drivers/audio/stm32_audio.h>

#define MODOPS_VOLUME      OPTION_GET(NUMBER, volume)
#define MODOPS_SAMPLE_RATE OPTION_GET(NUMBER, sample_rate)
#define MODOPS_BUF_CNT     OPTION_GET(NUMBER, buf_cnt)

#define D(fmt, ...) \
	do { \
		printk("%s" fmt "\n", __VA_ARGS__); \
	} while (0)

#define MAX_BUF_LEN (160 * 6)
#define CHAN_N      OPTION_GET(NUMBER, chan_n)
#define SLOT_N      OPTION_GET(NUMBER, slot_n)
/* TODO chan_n - (1 for mono, 2 for stereo). Each of channels can include several
 * slots (slot_n). Currently we interprete each OUTPUT slot as OUTPUT channel.
 * E.g. for STM32F7-Discovery we have 2 channels with 2 slots on each channel
 * (64 bit audio frame).
 * So if we play mono wav, we just copy each 16 bit sample 4 times. */
#define OUTPUT_CHAN_N (CHAN_N * SLOT_N)
#define BUF_N 2

struct pa_strm {
	int started;
	int paused;
	int completed;
	int sample_format;
	PaStreamCallback *callback;
	void *callback_data;
	size_t chan_buf_len;
	uint16_t in_buf[MAX_BUF_LEN];
	uint16_t out_buf[MAX_BUF_LEN * OUTPUT_CHAN_N * BUF_N];
	volatile unsigned char out_buf_empty_mask;
};
static_assert(BUF_N <= 8);

static struct thread *pa_thread;
static struct pa_strm pa_stream;

static irq_return_t stm32_audio_dma_interrupt(unsigned int irq_num, void *dev_id);

static void strm_get_data(struct pa_strm *strm, int buf_index) {
	uint16_t *buf;
	int i_in, rc;

	rc = strm->callback(NULL, strm->in_buf, strm->chan_buf_len, NULL, 0, strm->callback_data);
	if (rc == paComplete) {
		strm->completed = 1;
	}
	else assert(rc == paContinue);

	buf = strm->out_buf + buf_index * strm->chan_buf_len * OUTPUT_CHAN_N;
	for (i_in = 0; i_in < strm->chan_buf_len; ++i_in) {
		const uint16_t hw_frame = le16_to_cpu(strm->in_buf[i_in]);
		int i_out;

		for (i_out = 0; i_out < OUTPUT_CHAN_N; ++i_out) {
			buf[i_in * OUTPUT_CHAN_N + i_out] = hw_frame;
		}
	}
}

static void *pa_thread_hnd(void *arg) {
	struct pa_strm *strm = arg;

	while (1) {
		SCHED_WAIT(strm->out_buf_empty_mask);

		if (!strm->completed) {
			unsigned char empty_mask;
			int buf_index;

			empty_mask = strm->out_buf_empty_mask;

			static_assert(BUF_N == 2);
			if (empty_mask < 3) {
				/* 0 - impossible; 1 -> 0; 2 -> 1 */
				buf_index = empty_mask >> 1;
			} else {
				/* there are some empty buffers, but should be 1 */
				printk("stm32_pa: underrun!\n");
				buf_index = bit_ffs(empty_mask);
			}

			if (buf_index == 1) {
				BSP_AUDIO_OUT_Play(strm->out_buf,
						strm->chan_buf_len * OUTPUT_CHAN_N * BUF_N * sizeof(uint16_t));
			}

			strm_get_data(strm, buf_index);

			irq_lock();
			strm->out_buf_empty_mask &= ~(1 << buf_index);
			irq_unlock();
		} else {
			BSP_AUDIO_OUT_Stop(CODEC_PDWN_HW);
		}
	}

	return NULL;
}

PaError Pa_Initialize(void) {
	D("", __func__);

	return paNoError;
}

PaError Pa_Terminate(void) {
	D("", __func__);

	return paNoError;
}

PaHostApiIndex Pa_GetHostApiCount(void) { return 1; }
PaDeviceIndex Pa_GetDeviceCount(void) { return 1; }
PaDeviceIndex Pa_GetDefaultOutputDevice(void) { return 0; }

PaDeviceIndex Pa_GetDefaultInputDevice(void) { return 1; }

const char * Pa_GetErrorText(PaError errorCode) {
	D(": %d", __func__, errorCode);
	return "Pa_GetErrorText not implemented";
}

const PaDeviceInfo * Pa_GetDeviceInfo(PaDeviceIndex device) {
	static const PaDeviceInfo info = {
		.structVersion = 1,
		.name = "stm32_audio",
		.hostApi = 0,
		.maxInputChannels = 1,
		.maxOutputChannels = 1,
		.defaultLowInputLatency = 0,
		.defaultLowOutputLatency = 0,
		.defaultHighInputLatency = 0,
		.defaultHighOutputLatency = 0,
		.defaultSampleRate = MODOPS_SAMPLE_RATE
	};

	D(": %d = %p", __func__, device, device == 0 ? &info : NULL);
	return device == 0 ? &info : NULL;
}

const PaHostApiInfo * Pa_GetHostApiInfo(PaHostApiIndex hostApi) {
	static const PaHostApiInfo info = {
		.structVersion = 1,
		.name = "stm32f4_audio_host_api",
	};
	D(": %d = %p", __func__, hostApi, hostApi == 0 ? &info : NULL);
	return hostApi == 0 ? &info : NULL;
}

const PaStreamInfo * Pa_GetStreamInfo(PaStream *stream) {
	static PaStreamInfo info = {
		.structVersion = 1,
		.inputLatency = 0,
		.outputLatency = 0,
		.sampleRate = MODOPS_SAMPLE_RATE
	};

	D(": %p = %p", __func__, stream, stream != NULL ? &info : NULL);
	return stream != NULL ? &info : NULL;
}

PaError Pa_OpenStream(PaStream** stream,
		const PaStreamParameters *inputParameters,
		const PaStreamParameters *outputParameters,
		double sampleRate, unsigned long framesPerBuffer,
		PaStreamFlags streamFlags, PaStreamCallback *streamCallback,
		void *userData) {
	struct pa_strm *strm;

	assert(stream != NULL);
	/*assert(inputParameters == NULL);*/
	assert(outputParameters != NULL);
	assert(outputParameters->device == 0);
	assert(outputParameters->channelCount == 1);
	assert(outputParameters->sampleFormat == paInt16);
	assert(outputParameters->hostApiSpecificStreamInfo == 0);
	assert(streamFlags == paNoFlag || streamFlags == paClipOff);
	assert(streamCallback != NULL);
	assert(framesPerBuffer <= MAX_BUF_LEN);

	D(": %p %p %p %f %lu %lu %p %p", __func__, stream, inputParameters,
			outputParameters, sampleRate, framesPerBuffer, streamFlags, streamCallback, userData);

	strm = &pa_stream;
	strm->started = 0;
	strm->paused = 0;
	strm->completed = 0;
	strm->sample_format = outputParameters->sampleFormat;
	strm->chan_buf_len = (MAX_BUF_LEN / framesPerBuffer) * framesPerBuffer;
	strm->callback = streamCallback;
	strm->callback_data = userData;
	strm->out_buf_empty_mask = 0;

	pa_thread = thread_create(0, pa_thread_hnd, strm);
	if (err(pa_thread)) {
		goto err_out;
	}

	if (0 != irq_attach(STM32_AUDIO_DMA_IRQ, stm32_audio_dma_interrupt,
				0, strm, "stm32_audio")) {
		goto err_thread_free;
	}

	if (0 != BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_HEADPHONE, MODOPS_VOLUME,
				sampleRate)) {
		goto err_irq_detach;
	}

	*stream = &pa_stream;
	return paNoError;

err_irq_detach:
	irq_detach(STM32_AUDIO_DMA_IRQ, NULL);
err_thread_free:
	thread_delete(pa_thread);
	pa_thread = NULL;
err_out:
	return paUnanticipatedHostError;
}

PaError Pa_CloseStream(PaStream *stream) {
	D(": %p", __func__, stream);

	BSP_AUDIO_OUT_Stop(CODEC_PDWN_HW);

	irq_detach(STM32_AUDIO_DMA_IRQ, NULL);

	thread_delete(pa_thread);
	pa_thread = NULL;

	return paNoError;
}

PaError Pa_StartStream(PaStream *stream) {
	struct pa_strm *strm;

	D(": %p", __func__, stream);

	assert(stream != NULL);
	strm = (struct pa_strm *)stream;

	assert(!strm->started || strm->paused);

	if (!strm->started) {
		strm->out_buf_empty_mask = 0;

		if (0 != BSP_AUDIO_OUT_Play(strm->out_buf,
					strm->chan_buf_len * OUTPUT_CHAN_N * BUF_N * sizeof(uint16_t))) {
			return paInternalError;
		}
		printk("playing\n");
		strm->started = 1;
	} else {
		assert(strm->paused);

		if (0 != BSP_AUDIO_OUT_Resume()) {
			return paInternalError;
		}
		strm->paused = 0;
	}

	return paNoError;
}

PaError Pa_StopStream(PaStream *stream) {
	struct pa_strm *strm;

	D(": %p", __func__, stream);

	assert(stream != NULL);
	strm = (struct pa_strm *)stream;

	assert(strm->started && !strm->paused);

	if (0 != BSP_AUDIO_OUT_Pause()) {
		return paInternalError;
	}

	strm->paused = 1;

	return paNoError;
}

void Pa_Sleep(long msec) {
	D(" %ld", __func__, msec);
	usleep(msec * USEC_PER_MSEC);
}

static void stm32_audio_irq_fill_buffer(int buf_index) {
	pa_stream.out_buf_empty_mask |= 1 << buf_index;
	sched_wakeup(&pa_thread->schedee);
}

void BSP_AUDIO_OUT_HalfTransfer_CallBack(void) {
	stm32_audio_irq_fill_buffer(0);
}

void BSP_AUDIO_OUT_TransferComplete_CallBack(void) {
	stm32_audio_irq_fill_buffer(1);
}

STATIC_IRQ_ATTACH(STM32_AUDIO_DMA_IRQ, stm32_audio_dma_interrupt, &pa_stream);
