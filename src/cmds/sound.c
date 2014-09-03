/**
 * @file
 * @brief
 *
 * @date 27.08.14
 * @author Ilia Vaprol
 */

#include <stm32f4_discovery_audio_codec.h>
#include <stm32f4xx.h>

#include <embox/cmd.h>
#include <kernel/printk.h>
#include <kernel/irq.h>

#include <stdint.h>
#include <string.h>
#include <linux/byteorder.h>

EMBOX_CMD(exec);

extern const uint16_t AUDIO_SAMPLE[];  /* user-provided */

static volatile int playing;

static irq_return_t audio_i2s_dma_interrupt(unsigned int irq_num, void *dev_id) {
	extern void Audio_MAL_I2S_IRQHandler(void);
	Audio_MAL_I2S_IRQHandler();
	return IRQ_HANDLED;
}

#define log(s) printk(#s " = %d\n", s);
static int exec(int argc, char **argv) {
	irq_attach(AUDIO_I2S_DMA_IRQ + 16, audio_i2s_dma_interrupt, 0, NULL, "");
	uint32_t sample_rate = le32_to_cpu(*(uint32_t *)((uint8_t *)AUDIO_SAMPLE + 24));
	uint32_t subchunk2size = le32_to_cpu(*(uint32_t *)((uint8_t *)AUDIO_SAMPLE + 40));
	printk("rate: %u\n", sample_rate);
	printk("subchunk2size: %u\n", subchunk2size);

	EVAL_AUDIO_SetAudioInterface(AUDIO_INTERFACE_I2S);
	EVAL_AUDIO_Init(OUTPUT_DEVICE_HEADPHONE, 70, sample_rate);
	playing = 1;
	EVAL_AUDIO_Play((uint16_t *)((uint8_t *)AUDIO_SAMPLE + 44), subchunk2size - 8);
	while (playing) { }

	EVAL_AUDIO_Stop(CODEC_PDWN_SW);
	EVAL_AUDIO_DeInit();

	return 0;
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
	printk("halftransfer\n");
}

void EVAL_AUDIO_TransferComplete_CallBack(uint32_t pBuffer, uint32_t Size) {
	printk("transfercomplete\n");
	//playing = 0;
}
