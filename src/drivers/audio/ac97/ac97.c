/**
 * @file
 * @brief Driver for AC97
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-11-27
 *
 * In this implementation, there is no right/left channel control,
 * volume is always the same on all inputs and outputs
 */

#include <assert.h>
#include <stdint.h>

#include <drivers/audio/ac97.h>

/**
 * @brief Reset registers to default values
 *
 * @return Negative error code
 */
int ac97_reset(void) {
	/* Actually we can wrute anything to
	 * this register to reset codec */
	ac97_reg_write(AC97_RESET, 0xFFFF);

	return 0;
}

/**
 * @brief Changle volume settings
 *
 * @param vol New volume from 0x0...0x3F range
 *
 * @return Negative error code
 */
int ac97_set_vol(int vol) {
	assert(vol <= 0x3F);
	assert(vol >= 0);

	uint16_t tmp = vol | (vol << 8);

	ac97_reg_write(AC97_MASTER, tmp);;
	ac97_reg_write(AC97_HEADPHONE, tmp);
	ac97_reg_write(AC97_MASTER_MONO, tmp);
	ac97_reg_write(AC97_PCM, tmp);

	return 0;
}

/**
 * @brief Get master volume value
 *
 * @return Master volume value from 0x0...0x3F range
 */
int ac97_get_vol(void) {
	return 0x3F & ac97_reg_read(AC97_MASTER);
}

int ac97_set_rate(uint16_t rate, enum ac97_slot slot) {
	if ((rate != 8000) && (rate != 11025) &&
		(rate != 16000) && (rate != 22050) &&
		(rate != 44100) && (rate != 48000)) {
		return -1;
	}

	switch (slot) {
	case AC97_FRONT_DAC:
		ac97_reg_write(AC97_DAC_RATE, rate);
		break;
	case AC97_MIC_ADC:
		ac97_reg_write(AC97_ADC_RATE, rate);
		break;
	default:
		return -1;
	}
	return 0;
}

int ac97_get_rate(enum ac97_slot slot) {
	switch (slot) {
	case AC97_FRONT_DAC:
		return ac97_reg_read(AC97_DAC_RATE);
	case AC97_MIC_ADC:
		return ac97_reg_read(AC97_ADC_RATE);
	default:
		return -1;
	}
}

/**
 * @brief Initialize codec, setup static values and so on
 *
 * @param pci_dev PCI device of Intel Audio Controller
 * TODO handle other sound cards
 *
 * @return Negative error number
 */
int ac97_init(void) {
	ac97_reset();

	/* Set maximum master volume */
	ac97_set_vol(0x0);

	return 0;
}
