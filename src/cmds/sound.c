/**
 * @file
 * @brief
 *
 * @date 27.08.14
 * @author Ilia Vaprol
 */

#include <stm32f4_discovery_audio_codec.h>

#include <embox/cmd.h>
#include <kernel/printk.h>

#include <stdint.h>
#include <string.h>

EMBOX_CMD(exec);

extern const uint16_t AUDIO_SAMPLE[];  /* user-provided */

static int playing;

static int exec(int argc, char **argv) {

	EVAL_AUDIO_SetAudioInterface(AUDIO_INTERFACE_I2S);
	EVAL_AUDIO_Init(OUTPUT_DEVICE_HEADPHONE, 70, 48000);
	playing = 1;
	EVAL_AUDIO_Play((uint16_t*)(AUDIO_SAMPLE + 58), 100000 - 58);
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

void EVAL_AUDIO_TransferComplete_CallBack(uint32_t pBuffer, uint32_t Size) {
	printk("transfercomplete\n");
	playing = 0;
}
