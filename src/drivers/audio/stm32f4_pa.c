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
#define BUF_SZ 512

struct pa_strm {
	int started;
	int paused;
	int completed;
	int sample_format;
	unsigned long frames_per_buf;
	PaStreamCallback *callback;
	void *callback_data;
	uint16_t buf[BUF_SZ];
};

static struct pa_strm *_strm = NULL; /* for EVAL_AUDIO_HalfTransfer_CallBack */
static struct thread *pa_thread;
static volatile int pa_thread_waked;

static void strm_get_data(struct pa_strm *strm, uint16_t *buf, size_t len);

static void *pa_thread_hnd(void *arg) {
	static int half = 1;

	while (1) {
		SCHED_WAIT(pa_thread_waked);
		pa_thread_waked = 0;

		if (!_strm->completed) {
			strm_get_data(_strm, half ? _strm->buf : &_strm->buf[ARRAY_SIZE(_strm->buf) / 2], ARRAY_SIZE(_strm->buf) / 2);
			half = !half;
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
			&& (outputParameters->sampleFormat == paInt16
				|| outputParameters->sampleFormat == paInt32)
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

	return paNoError;
}

PaError Pa_CloseStream(PaStream *stream) {
	D(": %p", __func__, stream);
	EVAL_AUDIO_Stop(CODEC_PDWN_SW);
	EVAL_AUDIO_DeInit();
	return paNoError;
}

static void strm_get_data(struct pa_strm *strm, uint16_t *buf, size_t len) {
	static uint32_t callback_buf[BUF_SZ];
	uint16_t *callback_buf16;
	int rc;

	assert(strm != NULL);
	assert(buf != NULL);
	assert(len <= ARRAY_SIZE(callback_buf));

	rc = strm->callback(NULL, callback_buf, len, NULL, 0,
			strm->callback_data);
	if (rc == paComplete) {
		strm->completed = 1;
	}
	else assert(rc == paContinue);

	callback_buf16 = (uint16_t *)callback_buf;
	while (len--) {
		callback_buf16++;
		//printk("%#2hhx ", *((unsigned char *)callback_buf16));
		//printk("%#2hhx ", *((unsigned char *)callback_buf16 + 1));
		//if (len % 8 == 0)
		//	printk("\n");
		*buf++ = le16_to_cpu(*callback_buf16++);
	}
}

PaError Pa_StartStream(PaStream *stream) {
	struct pa_strm *strm;

	D(": %p", __func__, stream);

	assert(stream != NULL);
	strm = (struct pa_strm *)stream;

	assert(!strm->started || strm->paused);

	if (!strm->started) {
		strm_get_data(strm, strm->buf, ARRAY_SIZE(strm->buf));

		if (0 != EVAL_AUDIO_Play(strm->buf, sizeof strm->buf)) {
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

void EVAL_AUDIO_HalfTransfer_CallBack(uint32_t pBuffer, uint32_t Size) {
	pa_thread_waked = 1;
	sched_wakeup(&pa_thread->schedee);
	//printk("half: %x %x\n", pBuffer, Size);
//	assert(_strm != NULL);
//	printk("half: %x %x\n", pBuffer, Size);
//	if (!_strm->completed) {
//		strm_get_data(_strm, _strm->buf, ARRAY_SIZE(_strm->buf));
//	}
}

void EVAL_AUDIO_TransferComplete_CallBack(uint32_t pBuffer, uint32_t Size) {
	//printk("compl: %x %x\n", pBuffer, Size);
	if (_strm->completed) {
		EVAL_AUDIO_Stop(CODEC_PDWN_SW);
	}
}

#if 0
EMBOX_UNIT_INIT(stm32f4_audio_init);

struct audio_buf {
	struct dlist_head lnk;
	size_t size;
	int is_copied;
	void *data;
};

static struct audio_buf *audio_current = NULL;
static DLIST_DEFINE(audio_playlist);
POOL_DEF(audio_buf_pool, struct audio_buf, MODOPS_BUF_CNT);

static void play_finish(void) {
	ipl_t sp;
	void *tmp_data;

	sp = ipl_save();
	{
		if (audio_current) {
			tmp_data = audio_current->is_copied ? audio_current->data : NULL;
			pool_free(&audio_buf_pool, audio_current);
			audio_current = NULL;
		}
	}
	ipl_restore(sp);

	free(tmp_data);

	EVAL_AUDIO_Stop(CODEC_PDWN_SW);
}

static int play_next_if_finished(void) {
	ipl_t sp;
	int need_play = 0;

	sp = ipl_save();
	{
		if ((audio_current == NULL) && !dlist_empty(&audio_playlist)) {
			printk("play next\n");
			audio_current = dlist_first_entry(&audio_playlist,
					struct audio_buf, lnk);
			dlist_del(&audio_current->lnk);
			need_play = 1;
		}
	}
	ipl_restore(sp);

	if (need_play) {
		assert(audio_current);
		printk("play enable\n");
		EVAL_AUDIO_Play((uint16_t *)audio_current->data, audio_current->size);
		EVAL_AUDIO_PauseResume(AUDIO_RESUME);
	}

	return 0;
}

int audio_play(enum audio_play_mode mode, const void *data,
		size_t size, int need_copy) {
	struct audio_buf *buf;
	ipl_t sp;

	sp = ipl_save();
	{
		buf = pool_alloc(&audio_buf_pool);
	}
	ipl_restore(sp);
	assert(buf);

	dlist_head_init(&buf->lnk);
	buf->size = size;
	buf->is_copied = need_copy;
	if (need_copy) {
		buf->data = malloc(size);
		assert(buf->data);
		memcpy(buf->data, data, size);
	}
	else {
		buf->data = (void *)data;
	}

	sp = ipl_save();
	{
		switch (mode) {
		case APM_SEQ:
			dlist_add_prev(&buf->lnk, &audio_playlist);
			break;
		case APM_NOW:
			dlist_add_next(&buf->lnk, &audio_playlist);
			break;
		default:
			assert(0);
			break;
		}
	}
	ipl_restore(sp);
	printk("added to queue\n");

	return play_next_if_finished();
}

uint16_t EVAL_AUDIO_GetSampleCallBack(void) {
	printk("getsample\n");
	return 1;
}

uint32_t Codec_TIMEOUT_UserCallback(void) {
	printk("timeout\n");
	while (1) { }
	return 1;
}

void EVAL_AUDIO_HalfTransfer_CallBack(uint32_t pBuffer, uint32_t Size) {
}

void EVAL_AUDIO_TransferComplete_CallBack(uint32_t pBuffer, uint32_t Size) {
	play_finish();
	play_next_if_finished();
}

static int stm32f4_audio_init(void) {
	int rc;

	rc = irq_attach(AUDIO_I2S_DMA_IRQ + 16, audio_i2s_dma_interrupt, 0, NULL, "stm32f4_audio");
	if (rc != 0) {
		return rc;
	}

	EVAL_AUDIO_SetAudioInterface(AUDIO_INTERFACE_I2S);

	rc = EVAL_AUDIO_Init(OUTPUT_DEVICE_HEADPHONE, MODOPS_VOLUME,
			MODOPS_SAMPLE_RATE);
	if (rc != 0) {
		return rc;
	}

	return 0;
}

int stm32f4_audio_fini(void) {
	EVAL_AUDIO_Stop(CODEC_PDWN_SW);
	EVAL_AUDIO_DeInit();
	return 0;
}
#endif
