/**
 * @file
 * @brief
 *
 * @date 27.08.14
 * @author Ilia Vaprol
 */

#include <errno.h>
#include <stdio.h>
#include <stdint.h>

#include <drivers/audio.h>
#include <embox/cmd.h>
#include <kernel/printk.h>
#include <linux/byteorder.h>

EMBOX_CMD(exec);

extern const uint16_t AUDIO_SAMPLE[];  /* user-provided */

static int exec(int argc, char **argv) {
	uint32_t sample_rate, subchunk2size;

	if (argc != 1) {
		printf("%s: error: arguments are not supported\n", argv[0]);
		return -EINVAL;
	}

	sample_rate = le32_to_cpu(*(uint32_t *)((uint8_t *)AUDIO_SAMPLE + 24));
	subchunk2size = le32_to_cpu(*(uint32_t *)((uint8_t *)AUDIO_SAMPLE + 40));

	printf("rate: %u\n", sample_rate);
	printf("subchunk2size: %u\n", subchunk2size);

	audio_play(APM_SEQ, (uint8_t *)AUDIO_SAMPLE + 44, subchunk2size - 8, 0);

	return 0;
}
