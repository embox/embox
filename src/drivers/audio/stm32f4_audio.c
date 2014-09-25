/**
 * @file
 * @brief
 *
 * @date 25.09.14
 * @author Ilia Vaprol
 */

#include <stdlib.h>
#include <string.h>

#include <stm32f4_discovery_audio_codec.h>
#include <stm32f4xx.h>

#include <drivers/audio.h>
#include <embox/unit.h>
#include <hal/ipl.h>
#include <framework/mod/options.h>
#include <kernel/irq.h>
#include <kernel/printk.h>
#include <mem/misc/pool.h>
#include <util/dlist.h>

#define MODOPS_VOLUME      OPTION_GET(NUMBER, volume)
#define MODOPS_SAMPLE_RATE OPTION_GET(NUMBER, sample_rate)
#define MODOPS_BUF_CNT     OPTION_GET(NUMBER, buf_cnt)

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

static irq_return_t audio_i2s_dma_interrupt(unsigned int irq_num, void *dev_id) {
	extern void Audio_MAL_I2S_IRQHandler(void);
	Audio_MAL_I2S_IRQHandler();
	return IRQ_HANDLED;
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
