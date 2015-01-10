/**
 * @file
 * @brief
 *
 * @date 25.09.14
 * @author Ilia Vaprol
 */

#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <linux/byteorder.h>

#include <stm32f4_discovery_audio_codec.h>
#include <stm32f4xx.h>

#include <drivers/audio/portaudio.h>
#include <embox/unit.h>
#include <framework/mod/options.h>
#include <kernel/irq.h>
#include <kernel/thread.h>
#include <kernel/thread/thread_sched_wait.h>
#include <err.h>
#include <kernel/panic.h>
#include <kernel/printk.h>
#include <mem/misc/pool.h>
#include <util/dlist.h>

extern void Audio_MAL_I2S_IRQHandler(void);

#define MODOPS_VOLUME      OPTION_GET(NUMBER, volume)
#define MODOPS_SAMPLE_RATE OPTION_GET(NUMBER, sample_rate)
#define MODOPS_BUF_CNT     OPTION_GET(NUMBER, buf_cnt)

#define D(fmt, ...) \
	do { \
		printk("%s" fmt "\n", __VA_ARGS__); \
	} while (0)

#define STM32F4_AUDIO_I2S_DMA_IRQ (AUDIO_I2S_DMA_IRQ + 16)

#define MAX_FRAMES_PER_BUF 180
#define OUTPUT_CHAN_N 2
#define BUF_N 2

struct pa_strm {
	int started;
	int paused;
	int completed;
	int sample_format;
	int half;
	unsigned long frames_per_buf;
	PaStreamCallback *callback;
	void *callback_data;
	uint16_t in_buf[MAX_FRAMES_PER_BUF];
	uint16_t out_buf[MAX_FRAMES_PER_BUF * OUTPUT_CHAN_N * BUF_N];
};

static struct pa_strm *_strm = NULL; /* for EVAL_AUDIO_HalfTransfer_CallBack */
static struct thread *pa_thread;
static int pa_thread_waked;
#define PA_WAKED_CANARY_ADD 314

static void strm_get_data(struct pa_strm *strm, int buf_index);

static void *pa_thread_hnd(void *arg) {
	int buf_index;

	while (1) {
		SCHED_WAIT(pa_thread_waked);
		buf_index = pa_thread_waked - PA_WAKED_CANARY_ADD;
		pa_thread_waked = 0;

		if (!_strm->completed) {
			strm_get_data(_strm, buf_index);
		}
	}

	return NULL;
}

static irq_return_t stm32f4_audio_i2s_dma_interrupt(unsigned int irq_num, void *dev_id) {
	Audio_MAL_I2S_IRQHandler();
	return IRQ_HANDLED;
}

PaError Pa_Initialize(void) {
	D("", __func__);

	pa_thread = thread_create(0, pa_thread_hnd, NULL);
	if (err(pa_thread)) {
		goto err_out;
	}

	if (0 != irq_attach(STM32F4_AUDIO_I2S_DMA_IRQ, stm32f4_audio_i2s_dma_interrupt,
				0, NULL, "stm32f4_audio")) {
		goto err_thread_free;
	}

	EVAL_AUDIO_SetAudioInterface(AUDIO_INTERFACE_I2S);

	return paNoError;

err_thread_free:
	thread_delete(pa_thread);
err_out:
	return paUnanticipatedHostError;
}

PaError Pa_Terminate(void) {
	D("", __func__);

	if (0 != irq_detach(STM32F4_AUDIO_I2S_DMA_IRQ, NULL)) {
		return paUnanticipatedHostError;
	}

	thread_delete(pa_thread);

	return paNoError;
}

PaHostApiIndex Pa_GetHostApiCount(void) { return 1; }
PaDeviceIndex Pa_GetDeviceCount(void) { return 1; }
PaDeviceIndex Pa_GetDefaultOutputDevice(void) { return 0; }

const char * Pa_GetErrorText(PaError errorCode) {
	D(": %d", __func__, errorCode);
	return "Pa_GetErrorText not implemented";
}

void Pa_Sleep(long msec) {
	D(" %ld", __func__, msec);
	usleep(msec * USEC_PER_MSEC);
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
		.defaultSampleRate = MODOPS_SAMPLE_RATE
	};

	D(": %d = %p", __func__, device, device == 0 ? &info : NULL);
	return device == 0 ? &info : NULL;
}

const PaHostApiInfo * Pa_GetHostApiInfo(PaHostApiIndex hostApi) {
	D(": %d = NULL", __func__, hostApi);
	return NULL;
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
	static struct pa_strm strm;

	assert(stream != NULL);
	assert(inputParameters == NULL);
	assert(outputParameters != NULL
			&& outputParameters->device == 0
			&& outputParameters->channelCount == 1
			&& (outputParameters->sampleFormat == paInt16)
			&& outputParameters->hostApiSpecificStreamInfo == 0);
	assert(streamFlags == paNoFlag || streamFlags == paClipOff);
	assert(streamCallback != NULL);

	D(": %p %p %p %f %lu %lu %p %p", __func__, stream, inputParameters,
			outputParameters, sampleRate, framesPerBuffer, streamFlags, streamCallback, userData);

	if (0 != EVAL_AUDIO_Init(OUTPUT_DEVICE_HEADPHONE, MODOPS_VOLUME,
				sampleRate)) {
		return paInternalError;
	}

	strm.started = 0;
	strm.paused = 0;
	strm.completed = 0;
	strm.sample_format = outputParameters->sampleFormat;
	strm.frames_per_buf = framesPerBuffer;
	strm.callback = streamCallback;
	strm.callback_data = userData;
	*stream = (void *)&strm;
	_strm = &strm;

	assert(_strm->frames_per_buf <= MAX_FRAMES_PER_BUF);

	return paNoError;
}

PaError Pa_CloseStream(PaStream *stream) {
	D(": %p", __func__, stream);
	EVAL_AUDIO_Stop(CODEC_PDWN_SW);
	EVAL_AUDIO_DeInit();
	return paNoError;
}

static void strm_get_data(struct pa_strm *strm, int buf_index) {
	uint16_t *buf;
	int i_in, rc;

	/* assert(_strm->half == buf_index);*/

	rc = strm->callback(NULL, _strm->in_buf, _strm->frames_per_buf, NULL, 0, strm->callback_data);
	if (rc == paComplete) {
		strm->completed = 1;
	}
	else assert(rc == paContinue);

	buf = _strm->out_buf + _strm->half * _strm->frames_per_buf * OUTPUT_CHAN_N;
	for (i_in = 0; i_in < _strm->frames_per_buf; ++i_in) {
		const uint16_t hw_frame = le16_to_cpu(_strm->in_buf[i_in]);
		int i_out;

		for (i_out = 0; i_out < OUTPUT_CHAN_N; ++i_out) {
			buf[i_in * OUTPUT_CHAN_N + i_out] = hw_frame;
		}
	}

	_strm->half = 1 - _strm->half;
}

PaError Pa_StartStream(PaStream *stream) {
	struct pa_strm *strm;

	D(": %p", __func__, stream);

	assert(stream != NULL);
	strm = (struct pa_strm *)stream;

	assert(!strm->started || strm->paused);

	if (!strm->started) {
		strm_get_data(strm, 0);
		strm_get_data(strm, 1);

		if (0 != EVAL_AUDIO_Play(strm->out_buf, strm->frames_per_buf * OUTPUT_CHAN_N * BUF_N * sizeof(uint16_t))) {
			return paInternalError;
		}
		printk("playing\n");
		strm->started = 1;
	}
	else {
		assert(strm->paused);

		if (0 != EVAL_AUDIO_PauseResume(AUDIO_RESUME)) {
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

	if (0 != EVAL_AUDIO_PauseResume(AUDIO_PAUSE)) {
		return paInternalError;
	}

	strm->paused = 1;

	return paNoError;
}

uint16_t EVAL_AUDIO_GetSampleCallBack(void) {
	panic("getsample");
	return -1;
}

uint32_t Codec_TIMEOUT_UserCallback(void) {
	panic("timeout");
	return -1;
}

static void stm32f4_audio_irq_fill_buffer(int buf_index) {
	/*assert(pa_thread_waked == 0);*/
	pa_thread_waked = PA_WAKED_CANARY_ADD + buf_index;
	sched_wakeup(&pa_thread->schedee);
	if (_strm->completed) {
		EVAL_AUDIO_Stop(CODEC_PDWN_SW);
	}
}

void EVAL_AUDIO_HalfTransfer_CallBack(uint32_t pBuffer, uint32_t Size) {
	stm32f4_audio_irq_fill_buffer(0);
}

void EVAL_AUDIO_TransferComplete_CallBack(uint32_t pBuffer, uint32_t Size) {
	stm32f4_audio_irq_fill_buffer(1);
}
