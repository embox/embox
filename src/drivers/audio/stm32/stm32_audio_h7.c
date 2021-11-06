/**
 * @file
 * @brief Driver for STM32
 * @author Alex Kalmuk
 * @date 07.11.2018
 */

#include <util/log.h>

#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <string.h>

#include <embox/unit.h>

#include <kernel/panic.h>
#include <kernel/irq.h>
#include <arm/cpu_cache.h>

#include <portaudio.h>
#include <drivers/audio/audio_dev.h>
#include <drivers/audio/stm32h7_audio.h>

EMBOX_UNIT_INIT(stm32_audio_init);

#define STM32_DEFAULT_VOLUME    OPTION_GET(NUMBER, volume)
#define STM32_AUDIO_BUF_LEN     OPTION_GET(NUMBER, audio_buf_len)

/* Rate is sharable between input and output, see comments
 * in stm32_audio_set_rate() below */
static int stm32_audio_rate = 0;

struct stm32_audio_dev_priv {
	enum {
		STM32_DIGITAL_OUT,
		STM32_DIGITAL_IN
	} dev_type;
	uint8_t *buf;
	uint32_t buf_len;
};
static struct stm32_audio_dev_priv stm32_adc;
static struct stm32_audio_dev_priv stm32_dac;

struct stm32_hw {
	void *stream;
	int stm32_audio_running;
};
static struct stm32_hw stm32_hw_out;
static struct stm32_hw stm32_hw_in;

/* Pointer to record_data */
uint32_t playbackPtr;

static uint16_t stm32_audio_in_bufs[STM32_AUDIO_BUF_LEN] SRAM_NOCACHE_SECTION;
static uint8_t stm32_audio_out_bufs[STM32_AUDIO_BUF_LEN * 2] SRAM_NOCACHE_SECTION;

#define AUDIO_FREQUENCY            16000U
#define AUDIO_IN_PDM_BUFFER_SIZE  (uint32_t)(128*AUDIO_FREQUENCY/16000*2)
#define AUDIO_BUFF_SIZE           STM32_AUDIO_BUF_LEN

uint16_t recordPDMBuf[AUDIO_IN_PDM_BUFFER_SIZE] __attribute__((aligned(32))) __attribute__((section(".ram_d3")));

static void stm32_audio_in_activate(void) {
	stm32_hw_in.stm32_audio_running = 0;

	if (0 != BSP_AUDIO_IN_RecordPDM(1, (uint8_t *) &recordPDMBuf[0],
			2*AUDIO_IN_PDM_BUFFER_SIZE)) {
		log_error("BSP_AUDIO_IN_Record error");
	}
}

static void stm32_audio_out_activate(void) {
	memset(&stm32_audio_out_bufs[0], 0, sizeof stm32_audio_out_bufs);
	stm32_hw_out.stm32_audio_running = 0;

	memset(stm32_audio_out_bufs, 0, sizeof(stm32_audio_out_bufs));
	if (0 != BSP_AUDIO_OUT_Play(0, (uint8_t*) &stm32_audio_out_bufs[0],
			2 * STM32_AUDIO_BUF_LEN)) {
		log_error("BSP_AUDIO_OUT_Play error");
	}
}

static int stm32_audio_init(void) {
	stm32h7_audio_init();

	/* Now activate and run INPUT and OUTPUT permanently */
	stm32_audio_out_activate();
	stm32_audio_in_activate();

	return 0;
}

static int stm32_audio_get_rate(void) {
	return stm32_audio_rate;
}

static void stm32_audio_set_rate(int rate) {
	BSP_AUDIO_IN_Pause(1);
	BSP_AUDIO_OUT_Pause(0);
	BSP_AUDIO_OUT_SetSampleRate(0, rate);
	BSP_AUDIO_IN_Resume(1);
	BSP_AUDIO_OUT_Resume(0);

	stm32_audio_rate = rate;
}

static int stm32_audio_ioctl(struct audio_dev *dev, int cmd, void *args) {
	struct stm32_audio_dev_priv *priv = dev->ad_priv;
	switch(cmd) {
	case ADIOCTL_IN_SUPPORT:
		return priv->dev_type == STM32_DIGITAL_IN
			? AD_STEREO_SUPPORT | AD_16BIT_SUPPORT : 0;
	case ADIOCTL_OUT_SUPPORT:
		return priv->dev_type == STM32_DIGITAL_IN
			? 0 : AD_STEREO_SUPPORT | AD_16BIT_SUPPORT;
	case ADIOCTL_BUFLEN:
		return STM32_AUDIO_BUF_LEN;
	case ADIOCTL_GET_RATE:
		return stm32_audio_get_rate();
	case ADIOCTL_SET_RATE:
	{
		int rate = *(int *) args;
		stm32_audio_set_rate(rate);
		return 0;
	}
	default:
		log_error("Unsupported ioctl cmd %d", cmd);
	}
	SET_ERRNO(EINVAL);
	return -1;
}

static void stm32_audio_pause_stub(struct audio_dev *dev) {
}

static void stm32_audio_resume_stub(struct audio_dev *dev) {
}

/******** Functions for INPUT audio device ********/
static void stm32_audio_in_start(struct audio_dev *dev) {
	stm32_hw_in.stm32_audio_running = 1;
}

static void stm32_audio_in_stop(struct audio_dev *dev) {
	stm32_hw_in.stm32_audio_running = 0;
}

static const struct audio_dev_ops stm32_audio_in_ops = {
	.ad_ops_start  = stm32_audio_in_start,
	.ad_ops_pause  = stm32_audio_pause_stub,
	.ad_ops_resume = stm32_audio_resume_stub,
	.ad_ops_stop   = stm32_audio_in_stop,
	.ad_ops_ioctl  = stm32_audio_ioctl
};

static struct stm32_audio_dev_priv stm32_adc = {
	.dev_type = STM32_DIGITAL_IN,
	.buf      = (uint8_t *)&stm32_audio_in_bufs[0],
	.buf_len  = STM32_AUDIO_BUF_LEN,
};

AUDIO_DEV_DEF("stm32_adc", (struct audio_dev_ops *)&stm32_audio_in_ops, &stm32_adc, AUDIO_DEV_INPUT);

uint8_t *audio_dev_get_in_cur_ptr(struct audio_dev *audio_dev) {
	struct stm32_audio_dev_priv *priv = audio_dev->ad_priv;
	return priv->dev_type == STM32_DIGITAL_IN ? stm32_adc.buf : NULL;
}

void audio_dev_open_in_stream(struct audio_dev *audio_dev, void *stream) {
	stm32_hw_in.stream = stream;
}

#if 0
static void stm32_audio_in_update_buffer(int buf_index) {
	stm32_adc.buf = &stm32_audio_in_bufs[0] + buf_index * STM32_AUDIO_BUF_LEN;
	Pa_StartStream(stm32_hw_in.stream);
}

void BSP_AUDIO_IN_HalfTransfer_CallBack(uint32_t Instance) {
	if (stm32_hw_in.stm32_audio_running) {
		stm32_audio_in_update_buffer(0);
	}
}

void BSP_AUDIO_IN_TransferComplete_CallBack(uint32_t Instance) {
	if (stm32_hw_in.stm32_audio_running) {
		stm32_audio_in_update_buffer(1);
	}
}
#endif

static void stm32_audio_in_update_buffer(int buf_index) {
	if (playbackPtr == AUDIO_BUFF_SIZE / 2) {
		stm32_adc.buf = (uint8_t *)&stm32_audio_in_bufs[playbackPtr];
		Pa_StartStream(stm32_hw_in.stream);
	}
	playbackPtr += AUDIO_IN_PDM_BUFFER_SIZE / 4 / 2;
	if (playbackPtr >= AUDIO_BUFF_SIZE) {
		playbackPtr = 0;
	}
}

void BSP_AUDIO_IN_HalfTransfer_CallBack(uint32_t Instance) {
	if (stm32_hw_in.stm32_audio_running) {
		/* Invalidate Data Cache to get the updated content of the SRAM*/
		SCB_InvalidateDCache_by_Addr((uint32_t*) &recordPDMBuf[0],
				AUDIO_IN_PDM_BUFFER_SIZE * 2);

		BSP_AUDIO_IN_PDMToPCM(Instance, (uint16_t*) &recordPDMBuf[0],
				&stm32_audio_in_bufs[playbackPtr]);

		/* Clean Data Cache to update the content of the SRAM */
		SCB_CleanDCache_by_Addr((uint32_t*) &stm32_audio_in_bufs[playbackPtr],
				AUDIO_IN_PDM_BUFFER_SIZE / 4);

		stm32_audio_in_update_buffer(0);

	}
}

void BSP_AUDIO_IN_TransferComplete_CallBack(uint32_t Instance) {
	if (stm32_hw_in.stm32_audio_running) {
		/* Invalidate Data Cache to get the updated content of the SRAM*/
		SCB_InvalidateDCache_by_Addr(
				(uint32_t*) &recordPDMBuf[AUDIO_IN_PDM_BUFFER_SIZE / 2],
				AUDIO_IN_PDM_BUFFER_SIZE * 2);

		BSP_AUDIO_IN_PDMToPCM(Instance,
				(uint16_t*) &recordPDMBuf[AUDIO_IN_PDM_BUFFER_SIZE / 2],
				&stm32_audio_in_bufs[playbackPtr]);

		/* Clean Data Cache to update the content of the SRAM */
		SCB_CleanDCache_by_Addr((uint32_t*) &stm32_audio_in_bufs[playbackPtr],
				AUDIO_IN_PDM_BUFFER_SIZE / 4);

		stm32_audio_in_update_buffer(1);
	}
}

void BSP_AUDIO_IN_Error_CallBack(uint32_t Instance) {
	log_error("");
}

/******** Functions for OUTPUT audio device ********/
static void stm32_audio_out_start(struct audio_dev *dev) {
	stm32_hw_out.stm32_audio_running = 1;
}

static void stm32_audio_out_stop(struct audio_dev *dev) {
	memset(&stm32_audio_out_bufs[0], 0, sizeof stm32_audio_out_bufs);
	stm32_hw_out.stm32_audio_running = 0;
}

static const struct audio_dev_ops stm32_audio_out_ops = {
	.ad_ops_start  = stm32_audio_out_start,
	.ad_ops_pause  = stm32_audio_pause_stub,
	.ad_ops_resume = stm32_audio_resume_stub,
	.ad_ops_stop   = stm32_audio_out_stop,
	.ad_ops_ioctl  = stm32_audio_ioctl
};

static struct stm32_audio_dev_priv stm32_dac = {
	.dev_type = STM32_DIGITAL_OUT,
	.buf      = &stm32_audio_out_bufs[0],
	.buf_len  = STM32_AUDIO_BUF_LEN
};

AUDIO_DEV_DEF("stm32_dac", (struct audio_dev_ops *)&stm32_audio_out_ops, &stm32_dac, AUDIO_DEV_OUTPUT);

uint8_t *audio_dev_get_out_cur_ptr(struct audio_dev *audio_dev) {
	struct stm32_audio_dev_priv *priv = audio_dev->ad_priv;
	return priv->dev_type == STM32_DIGITAL_OUT ? priv->buf : NULL;
}

void audio_dev_open_out_stream(struct audio_dev *audio_dev, void *stream) {
	stm32_hw_out.stream = stream;
}

static void stm32_audio_out_update_buffer(int buf_index) {
	stm32_dac.buf = &stm32_audio_out_bufs[0] + buf_index * STM32_AUDIO_BUF_LEN;
	Pa_StartStream(stm32_hw_out.stream);
}

void BSP_AUDIO_OUT_HalfTransfer_CallBack(uint32_t Instance) {
	if (stm32_hw_out.stm32_audio_running) {
		stm32_audio_out_update_buffer(0);
	}
}

void BSP_AUDIO_OUT_TransferComplete_CallBack(uint32_t Instance) {
	if (stm32_hw_out.stm32_audio_running) {
		stm32_audio_out_update_buffer(1);
	}
}

void BSP_AUDIO_OUT_Error_CallBack(uint32_t Instance) {
	log_error("");
}
